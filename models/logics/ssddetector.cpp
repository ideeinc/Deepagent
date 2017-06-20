#include "ssddetector.h"
#include <caffe/caffe.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <algorithm>
#include <iomanip>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <TDebug>
using namespace caffe;


struct SsdDetector::SsdDetectorPrivate {
    std::shared_ptr<caffe::Net<float>> net_;
    cv::Size input_geometry_;
    int num_channels_ {0};
    cv::Mat mean_;
};


SsdDetector::SsdDetector(const string& model_file, const string& weights_file,
                         const string& mean_file, const string& mean_value)
{
    reset(model_file, weights_file, mean_file, mean_value);
}

void SsdDetector::reset(const string& model_file, const string& weights_file, const string& mean_file, const string& mean_value)
{
#ifndef CPU_ONLY
    Caffe::set_mode(Caffe::GPU);
    Caffe::SetDevice(0);
#endif

    if (d) {
        delete d;
    }
    d = new SsdDetectorPrivate;

    /* Load the network. */
    d->net_.reset(new Net<float>(model_file, TEST));
    d->net_->CopyTrainedLayersFrom(weights_file);

    CHECK_EQ(d->net_->num_inputs(), 1) << "Network should have exactly one input.";
    CHECK_EQ(d->net_->num_outputs(), 1) << "Network should have exactly one output.";

    Blob<float>* input_layer = d->net_->input_blobs()[0];
    d->num_channels_ = input_layer->channels();
    CHECK(d->num_channels_ == 3 || d->num_channels_ == 1) << "Input layer should have 1 or 3 channels.";
    d->input_geometry_ = cv::Size(input_layer->width(), input_layer->height());

    /* Load the binaryproto mean file. */
    SetMean(mean_file, mean_value);
}


SsdDetector::~SsdDetector()
{
    delete d;
}


QList<QVector<float>> SsdDetector::detect(const cv::Mat& img, float threshold) const
{
    Blob<float>* input_layer = d->net_->input_blobs()[0];
    input_layer->Reshape(1, d->num_channels_, d->input_geometry_.height, d->input_geometry_.width);
    /* Forward dimension change to all layers. */
    d->net_->Reshape();

    std::vector<cv::Mat> input_channels;
    WrapInputLayer(&input_channels);
    Preprocess(img, &input_channels);
    d->net_->Forward();
    tDebug() << "CNN forward ... done";

    /* Copy the output layer to a std::vector */
    Blob<float>* result_blob = d->net_->output_blobs()[0];
    const float* result = result_blob->cpu_data();
    const int num_det = result_blob->height();
    QList<QVector<float>> detections;
    for (int k = 0; k < num_det; ++k) {

        if (result[0] == -1) {
            // Skip invalid detection.
            result += 7;
            continue;
        }
        // Detection format: [image_id, label, score, xmin, ymin, xmax, ymax].
        const float score = result[2];
        if (score >= threshold) {
            detections << QVector<float>({result[0], result[1], score, qMax(result[3]*img.cols, 0.f), qMax(result[4]*img.rows, 0.f),
                                          qMin(result[5]*img.cols, (float)img.cols), qMin(result[6]*img.rows, (float)img.rows)});
        }
        result += 7;
    }
    return detections;
}

/* Load the mean file in binaryproto format. */
void SsdDetector::SetMean(const string& mean_file, const string& mean_value)
{
    cv::Scalar channel_mean;
    if (!mean_file.empty()) {
        CHECK(mean_value.empty()) << "Cannot specify mean_file and mean_value at the same time";
        BlobProto blob_proto;
        ReadProtoFromBinaryFileOrDie(mean_file.c_str(), &blob_proto);

        /* Convert from BlobProto to Blob<float> */
        Blob<float> mean_blob;
        mean_blob.FromProto(blob_proto);
        CHECK_EQ(mean_blob.channels(), d->num_channels_) << "Number of channels of mean file doesn't match input layer.";

        /* The format of the mean file is planar 32-bit float BGR or grayscale. */
        std::vector<cv::Mat> channels;
        float* data = mean_blob.mutable_cpu_data();
        for (int i = 0; i < d->num_channels_; ++i) {
            /* Extract an individual channel. */
          cv::Mat channel(mean_blob.height(), mean_blob.width(), CV_32FC1, data);
          channels.push_back(channel);
          data += mean_blob.height() * mean_blob.width();
        }

        /* Merge the separate channels into a single image. */
        cv::Mat mean;
        cv::merge(channels, mean);

        /* Compute the global mean pixel value and create a mean image
         * filled with this value. */
        channel_mean = cv::mean(mean);
        d->mean_ = cv::Mat(d->input_geometry_, mean.type(), channel_mean);
    }
    if (!mean_value.empty()) {
        CHECK(mean_file.empty()) << "Cannot specify mean_file and mean_value at the same time";
        stringstream ss(mean_value);
        vector<float> values;
        string item;
        while (getline(ss, item, ',')) {
          float value = std::atof(item.c_str());
          values.push_back(value);
        }
        CHECK(values.size() == 1 || values.size() == d->num_channels_) << "Specify either 1 mean_value or as many as channels: " << d->num_channels_;

        std::vector<cv::Mat> channels;
        for (int i = 0; i < d->num_channels_; ++i) {
            /* Extract an individual channel. */
          cv::Mat channel(d->input_geometry_.height, d->input_geometry_.width, CV_32FC1,
              cv::Scalar(values[i]));
          channels.push_back(channel);
      }
      cv::merge(channels, d->mean_);
    }
}

/* Wrap the input layer of the network in separate cv::Mat objects
 * (one per channel). This way we save one memcpy operation and we
 * don't need to rely on cudaMemcpy2D. The last preprocessing
 * operation will write the separate channels directly to the input
 * layer. */
void SsdDetector::WrapInputLayer(std::vector<cv::Mat>* input_channels) const
{
    Blob<float>* input_layer = d->net_->input_blobs()[0];

    int width = input_layer->width();
    int height = input_layer->height();
    float* input_data = input_layer->mutable_cpu_data();
    for (int i = 0; i < input_layer->channels(); ++i) {
        cv::Mat channel(height, width, CV_32FC1, input_data);
        input_channels->push_back(channel);
        input_data += width * height;
    }
}

void SsdDetector::Preprocess(const cv::Mat& img, std::vector<cv::Mat>* input_channels) const
{
    /* Convert the input image to the input image format of the network. */
    cv::Mat sample;
    if (img.channels() == 3 && d->num_channels_ == 1)
        cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
    else if (img.channels() == 4 && d->num_channels_ == 1)
        cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
    else if (img.channels() == 4 && d->num_channels_ == 3)
        cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
    else if (img.channels() == 1 && d->num_channels_ == 3)
        cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
    else
        sample = img;

    cv::Mat sample_resized;
    if (sample.size() != d->input_geometry_)
        cv::resize(sample, sample_resized, d->input_geometry_);
    else
        sample_resized = sample;

    cv::Mat sample_float;
    if (d->num_channels_ == 3)
        sample_resized.convertTo(sample_float, CV_32FC3);
    else
        sample_resized.convertTo(sample_float, CV_32FC1);

    cv::Mat sample_normalized;
    cv::subtract(sample_float, d->mean_, sample_normalized);

    /* This operation will write the separate BGR planes directly to the
     * input layer of the network because it is wrapped by the cv::Mat
     * objects in input_channels. */
    cv::split(sample_normalized, *input_channels);

    CHECK(reinterpret_cast<float*>(input_channels->at(0).data)
          == d->net_->input_blobs()[0]->cpu_data()) << "Input channels are not wrapping the input layer of the network.";
}

// Return value format: [image_id, label, score, xmin, ymin, xmax, ymax]
QList<QVector<float>> SsdDetector::detect(const QString &imgFile, float threshold, const QString& modelFile, const QString &weightsFile, const QString &meanValue)
{
    auto img = cv::imread(imgFile.toStdString(), -1);
    SsdDetector detector(modelFile.toStdString(), weightsFile.toStdString(), string(), meanValue.toStdString());
    return detector.detect(img, threshold);
}