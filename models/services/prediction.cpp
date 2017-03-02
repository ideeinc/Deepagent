#include "prediction.h"
#include "image.h"
#include <caffe/caffe.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <TDebug>
using namespace caffe;

#define netPtr  ((Net<float> *)_trainedNet)

const int RankNum = 4;


// binaryproto読込
static void readFromBinaryProto(const QString &path, Blob<float> &blob)
{
    if (!QFileInfo(path).exists()) {
        return;
    }
    BlobProto proto;
    ReadProtoFromBinaryFileOrDie(path.toStdString(), &proto);  // binaryproto
    blob.FromProto(proto);
    tDebug() << "readFromBinaryProto() blob num:" << blob.num()
             << " channels:" << blob.channels()
             << " width:" << blob.width()
             << " height:" << blob.height();
}


// static void readImageToBlob(const QString &imgPath, Blob<float> &blob)
// {
//     auto mat = cv::imread(qPrintable(imgPath), 1);
//     TransformationParameter param;
//     DataTransformer<float> transformer(param, TEST);
//     transformer.Transform(mat, &blob);
// }


void Prediction::convertImageToBlob(const Image &image, Blob<float> &blob)
{
    const int channels = image.channels();
    const int height = image.height();
    const int width = image.width();

    vector<int> shape{ 1, channels, height, width };
    blob.Reshape(shape);

    float *transdata = blob.mutable_cpu_data();
    for (int h = 0; h < height; ++h) {
        const uchar *ptr = image._mat.ptr<uchar>(h);
        int imgIndex = 0;
        for (int w = 0; w < width; ++w) {
            for (int c = 0; c < channels; ++c) {
                int idx = (c * height + h) * width + w;
                transdata[idx] = static_cast<float>(ptr[imgIndex++]);
            }
        }
    }
}


Image Prediction::convertBlobToImage(const Blob<float> &blob)
{
    const int channels = blob.channels();
    const int height = blob.height();
    const int width = blob.width();

    Image ret;
    ret._mat = Mat(height, width, CV_8UC3);

    const float *transdata = blob.cpu_data();
    for (int h = 0; h < height; ++h) {
        uchar *ptr = ret._mat.ptr<uchar>(h);
        int imgIndex = 0;
        for (int w = 0; w < width; ++w) {
            for (int c = 0; c < channels; ++c) {
                int idx = (c * height + h) * width + w;
                ptr[imgIndex++] = transdata[idx];
            }
        }
    }
    return ret;
}


static void setBlobToNet(const QString &path, const QString &meanPath, Net<float> &net)
{
    tDebug() << "setBlobToNet() path:" << path << " meanPath:" << meanPath;

    const vector<Blob<float>*> &netInputBlobs = net.input_blobs();
    if (netInputBlobs.size() == 0) {
        tError() << "Bad netInputBlobs [" << __FILE__ << ":" << __LINE__ << "]";
        return;
    }

    int netnum = netInputBlobs[0]->num();
    int width = netInputBlobs[0]->width();
    int height = netInputBlobs[0]->height();
    int channels = netInputBlobs[0]->channels();
    tDebug() << "width:" << width;
    tDebug() << "height:" << height;
    tDebug() << "channels:" << channels;
    tDebug() << "num: " << netnum;

    const QStringList imageSuffix = { "png", "jpeg", "jpg", "gif" };
    Image inputImg(path);
    tDebug() << "img width:" << inputImg.width();
    tDebug() << "img height:" << inputImg.height();
    tDebug() << "img channels:" << inputImg.channels();

    inputImg.normalize();
    inputImg.trim();
    inputImg.resize(width, height);

    // Mean file
    auto suffix = QFileInfo(meanPath).suffix().toLower();
    if (imageSuffix.contains(suffix)) {
        Image mean(meanPath);
        mean.resize(width, height);
        inputImg.substract(mean);
#if 0
        Blob<float> meanBlob;
        Prediction::convertImageToBlob(mean, meanBlob);
        auto img = Prediction::convertBlobToImage(meanBlob);
        img.substract(mean);
#endif
    } else {
        Blob<float> meanBlob;
        readFromBinaryProto(meanPath, meanBlob);
        Image mean = Prediction::convertBlobToImage(meanBlob);
        mean.resize(width, height);
        inputImg.substract(mean);
    }

    Blob<float> blob;
    Prediction::convertImageToBlob(inputImg, blob);

    // Format input for Caffe:
    // - convert to single
    // - resize to input dimensions (preserving number of channels)
    // - transpose dimensions to K x H x W
    // - reorder channels (for instance color to BGR)
    // - scale raw input (e.g. from [0, 1] to [0, 255] for ImageNet models)
    // - subtract mean
    // - scale feature

    // set to net
    net.input_blobs()[0]->CopyFrom(blob);
}


Prediction::Prediction(const CaffeTrainedModel &trainedModel)
    : _trainedModel(trainedModel)
{ }


Prediction::~Prediction()
{
    release();
}


bool Prediction::init()
{
    static bool once = false;
    if (!once) {
        once = true;
        google::InitGoogleLogging(qPrintable(QCoreApplication::arguments().at(0)));
        google::InstallFailureSignalHandler();
    }

    try {
        auto netPath = _trainedModel.getNeuralNetwork().absFilePath();
        if (netPath.isEmpty()) {
            tDebug() << "getNeuralNetwork is empty";
            return false;
        }

        _trainedNet = new Net<float>(netPath.toStdString(), TEST);
        netPtr->CopyTrainedLayersFrom(_trainedModel.modelPath().toStdString());
        return true;

    } catch (...) {
        return false;
    }
}


void Prediction::release()
{
    if (_trainedNet) {
        delete netPtr;
        _trainedNet = nullptr;
    }
}


QList<QPair<QString, float>> Prediction::predict(const QString &filePath) const
{
    setBlobToNet(filePath, _trainedModel.getDataset().meanPath(), *netPtr);
    const vector<Blob<float>*> &result = netPtr->Forward();
    int chan = result[0]->channels();

    tDebug() << "prediction result:";
    tDebug() << "JPEG: " << filePath;

    QMap<int, QString> labels;
    QList<QPair<QString, float>> rank;

    for (int i = 0; i < chan; i++) {
        float prd = result[0]->cpu_data()[i];

        if (rank.isEmpty() || prd > rank.last().second) {
            if (rank.count() >= RankNum) {
                rank.removeLast();
            }

            QString lbl = labels.value(i);
            if (lbl.isEmpty()) {
                lbl = QString::number(i);
            }
            rank.append(qMakePair(lbl, prd));

            qSort(rank.begin(), rank.end(), [](const QPair<QString, float> &p1, const QPair<QString, float> &p2) {
                return p1.second > p2.second;
            });
        }
    }

    int r = 1;
    for (auto &p : rank) {
        tDebug("#%d %s  %4.1f%%", r++, qPrintable(p.first), p.second * 100.0);
    }
    return rank;
}
