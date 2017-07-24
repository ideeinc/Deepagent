#include "ssddetector.h"
#include "image.h"
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
    std::shared_ptr<caffe::Net<float>> net;
    cv::Size inputGeometry;
    int numChannels {0};
    cv::Mat mean;
    int borderWidth {1};
};


SsdDetector::SsdDetector(const QString &modelFile, const QString &weightsFile,
                         const QString &meanFile, const QString &meanValue)
{
    reset(modelFile, weightsFile, meanFile, meanValue);
}

void SsdDetector::reset(const QString &modelFile, const QString &weightsFile, const QString &meanFile, const QString &meanValue)
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
    d->net.reset(new Net<float>(modelFile.toStdString(), TEST));
    d->net->CopyTrainedLayersFrom(weightsFile.toStdString());

    CHECK_EQ(d->net->num_inputs(), 1) << "Network should have exactly one input.";
    CHECK_EQ(d->net->num_outputs(), 1) << "Network should have exactly one output.";

    Blob<float>* inputLayer = d->net->input_blobs()[0];
    d->numChannels = inputLayer->channels();
    CHECK(d->numChannels == 3 || d->numChannels == 1) << "Input layer should have 1 or 3 channels.";
    d->inputGeometry = cv::Size(inputLayer->width(), inputLayer->height());

    /* Load the binaryproto mean file. */
    setMean(meanFile, meanValue);
}


SsdDetector::~SsdDetector()
{
    if (d) delete d;
}


QList<QVector<float>> SsdDetector::detect(const cv::Mat& img, float threshold) const
{
    Blob<float>* inputLayer = d->net->input_blobs()[0];
    inputLayer->Reshape(1, d->numChannels, d->inputGeometry.height, d->inputGeometry.width);
    /* Forward dimension change to all layers. */
    d->net->Reshape();

    std::vector<cv::Mat> inputChannels = wrapInputLayer();
    preprocess(img, inputChannels);
    d->net->Forward();
    tDebug() << "CNN forward ... done";

    /* Copy the output layer to a std::vector */
    Blob<float>* result_blob = d->net->output_blobs()[0];
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


static QRect getTrimSize(const QString& movieFilePath)
{
    cv::VideoCapture cap;

    if (! cap.open(movieFilePath.toStdString())) {
        tWarn() << "can not open movie file, " << movieFilePath;
        return QRect();
    }

    int totalFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);
    int inc = qMax(qMin(60, totalFrames/60), 1);
    int pos = (totalFrames >= 60 * 5) ? inc : 0;

    QList<QRect> rects;
    cv::Mat img;
    for (int i = 0; i < 5; i++) {
        cap.set(CV_CAP_PROP_POS_FRAMES, pos);
        if (! cap.grab()) {
            break;
        }
        cap.retrieve(img);
        QRect r = Image(img).getValidRect();
        rects << r;
        tDebug() << "rect candidate: x:" << r.x() << " y:" << r.y()
                 << " w:" << r.width() << " h:" << r.height();
        pos += inc;
    }
    cap.release();

    std::sort(rects.begin(), rects.end(), [](const QRect &a, const QRect &b) {
        return (a.width()*a.height()) > (b.width()*b.height()); // 矩形面積でソート
    });
    return rects.value(rects.length()/2);  // 真ん中のもの
}

/*!
  Detects objects
  labels: idx:0 -> background(unused), 1..n -> text & color
 */
bool SsdDetector::detect(const QString& movieFilePath, float threshold, const QList<QPair<QString, Color>> &labels,
                         const QString& outputFile) const
{
    cv::VideoCapture cap;
    auto rect = getTrimSize(movieFilePath);
    tDebug() << "movie trim: x:" << rect.x() << " y:" << rect.y() << " w:" << rect.width() << " h:" << rect.height();

    if (! cap.open(movieFilePath.toStdString())) {
        tWarn() << "can not open movie file, " << movieFilePath;
        return false;
    }

    cv::Size size(300, 300);
    cv::VideoWriter vw(outputFile.toStdString(), CV_FOURCC('D', 'I', 'V', 'X'), 30, size, true);
    cv::Mat cvimg;
    const Color defaultColor(255,255,255);

    while (cap.grab()) {
        cap.retrieve(cvimg);
        Image img(cvimg);
        img.crop(rect.x(), rect.y(), rect.width(), rect.height());
        img.resize(300, 300);
        auto dets = detect(img.mat(), threshold);

        for (const auto &c : dets) {
            int id = c[1];
            auto score = c[2];
            auto val = labels.value(id, qMakePair(QString::number(id), defaultColor));
            auto color = (CvScalar)val.second;

            img.drawRectangle(c[3], c[4], c[5], c[6], color, d->borderWidth);
            auto text = QString("%1 : %2").arg(val.first).arg(score, 0, 'g', 2);
            img.drawLabel(text, c[3], c[4], cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0), 1, color, 0.5); // label
            tInfo() << "class:" << id << " score:"  << score << " w:" << (int)(c[3]) << " h:"  << (int)c[4]
                    << " w:" << (int)c[5] << " h:"  << (int)c[6];
        }
        vw.write(img.mat());
    }
    cap.release();
    return true;
}

/* Load the mean file in binaryproto format. */
void SsdDetector::setMean(const QString &meanFile, const QString &meanValue)
{
    cv::Scalar channelMean;
    if (!meanFile.isEmpty()) {
        CHECK(meanValue.isEmpty()) << "Cannot specify meanFile and meanValue at the same time";
        BlobProto blobProto;
        ReadProtoFromBinaryFileOrDie(qPrintable(meanFile), &blobProto);

        /* Convert from BlobProto to Blob<float> */
        Blob<float> meanBlob;
        meanBlob.FromProto(blobProto);
        CHECK_EQ(meanBlob.channels(), d->numChannels) << "Number of channels of mean file doesn't match input layer.";

        /* The format of the mean file is planar 32-bit float BGR or grayscale. */
        std::vector<cv::Mat> channels;
        float* data = meanBlob.mutable_cpu_data();
        for (int i = 0; i < d->numChannels; ++i) {
            /* Extract an individual channel. */
            cv::Mat channel(meanBlob.height(), meanBlob.width(), CV_32FC1, data);
            channels.push_back(channel);
            data += meanBlob.height() * meanBlob.width();
        }

        /* Merge the separate channels into a single image. */
        cv::Mat mean;
        cv::merge(channels, mean);

        /* Compute the global mean pixel value and create a mean image
         * filled with this value. */
        channelMean = cv::mean(mean);
        d->mean = cv::Mat(d->inputGeometry, mean.type(), channelMean);
    }
    if (!meanValue.isEmpty()) {
        CHECK(meanFile.isEmpty()) << "Cannot specify meanFile and meanValue at the same time";
        stringstream ss(meanValue.toStdString());
        vector<float> values;
        string item;
        while (getline(ss, item, ',')) {
            float value = std::atof(item.c_str());
            values.push_back(value);
        }
        CHECK(values.size() == 1 || values.size() == d->numChannels) << "Specify either 1 meanValue or as many as channels: " << d->numChannels;

        std::vector<cv::Mat> channels;
        for (int i = 0; i < d->numChannels; ++i) {
            /* Extract an individual channel. */
            cv::Mat channel(d->inputGeometry.height, d->inputGeometry.width, CV_32FC1,
                            cv::Scalar(values[i]));
            channels.push_back(channel);
      }
      cv::merge(channels, d->mean);
    }
}

/* Wrap the input layer of the network in separate cv::Mat objects
 * (one per channel). This way we save one memcpy operation and we
 * don't need to rely on cudaMemcpy2D. The last preprocessing
 * operation will write the separate channels directly to the input
 * layer. */
std::vector<cv::Mat> SsdDetector::wrapInputLayer() const
{
    std::vector<cv::Mat> layers;
    Blob<float>* inputLayer = d->net->input_blobs()[0];

    int width = inputLayer->width();
    int height = inputLayer->height();
    float* inputData = inputLayer->mutable_cpu_data();
    for (int i = 0; i < inputLayer->channels(); ++i) {
        cv::Mat channel(height, width, CV_32FC1, inputData);
        layers.push_back(channel);
        inputData += width * height;
    }
    return layers;
}

void SsdDetector::preprocess(const cv::Mat& img, const std::vector<cv::Mat>& inputChannels) const
{
    /* Convert the input image to the input image format of the network. */
    cv::Mat sample;
    if (img.channels() == 3 && d->numChannels == 1)
        cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
    else if (img.channels() == 4 && d->numChannels == 1)
        cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
    else if (img.channels() == 4 && d->numChannels == 3)
        cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
    else if (img.channels() == 1 && d->numChannels == 3)
        cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
    else
        sample = img;

    cv::Mat sampleResized;
    if (sample.size() != d->inputGeometry)
        cv::resize(sample, sampleResized, d->inputGeometry);
    else
        sampleResized = sample;

    cv::Mat sampleFloat;
    if (d->numChannels == 3)
        sampleResized.convertTo(sampleFloat, CV_32FC3);
    else
        sampleResized.convertTo(sampleFloat, CV_32FC1);

    cv::Mat sampleNormalized;
    cv::subtract(sampleFloat, d->mean, sampleNormalized);

    /* This operation will write the separate BGR planes directly to the
     * input layer of the network because it is wrapped by the cv::Mat
     * objects in inputChannels. */
    cv::split(sampleNormalized, inputChannels);

    CHECK(reinterpret_cast<float*>(inputChannels.at(0).data)
          == d->net->input_blobs()[0]->cpu_data()) << "Input channels are not wrapping the input layer of the network.";
}

// Return value format: [image_id, label, score, xmin, ymin, xmax, ymax]
QList<QVector<float>> SsdDetector::detect(const QString &imgFile, float threshold, const QString& modelFile, const QString &weightsFile, const QString &meanValue)
{
    auto img = cv::imread(imgFile.toStdString(), -1);
    SsdDetector detector(modelFile, weightsFile, QString(), meanValue);
    return detector.detect(img, threshold);
}
