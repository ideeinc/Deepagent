#include "prediction.h"
#include <QtCore>
#include <TDebug>
#include <caffe/caffe.hpp>
using namespace caffe;

#define netPtr  ((Net<float> *)_trainedNet)

const QStringList imageSuffix = { "png", "jpeg", "jpg", "gif" };
bool Prediction::_gpuEnable = true;
int Prediction::_gpuDevice = 0;


// binaryproto読込
static void readFromBinaryProto(const QString &path, Blob<float> &blob, int num, int channels)
{
    if (!QFileInfo(path).exists()) {
        tError() << "file not found: " << path;
        return;
    }
    BlobProto proto;
    ReadProtoFromBinaryFileOrDie(path.toStdString(), &proto);  // binaryproto
    blob.FromProto(proto, true);

    num = qMax(num, 1);
    channels = qMax(channels, 1);
    if (blob.num() != num || blob.channels() != channels) {
        // reshape
        int width = sqrt(proto.data_size() / channels / num);
        vector<int> shape{ num, channels, width, width };
        blob.Reshape(shape);
    }

    tDebug() << "readFromBinaryProto() blob count:" << blob.count()
             << " num:" << blob.num()
             << " channels:" << blob.channels()
             << " width:" << blob.width()
             << " height:" << blob.height();
}


Prediction::Prediction(const QString &netPrototxtPath)
    : _netPrototxtPath(netPrototxtPath)
{ }


Prediction::~Prediction()
{
    release();
}


bool Prediction::init(const QString &trainedModelFilePath, const QString &meanFilePath)
{
    try {
#ifdef CPU_ONLY
//        Caffe::set_mode(Caffe::CPU);
#else
//        Caffe::set_mode(_gpuEnable ? Caffe::GPU : Caffe::CPU);
        Caffe::SetDevice(_gpuDevice);
#endif
        //Caffe::set_multiprocess(true);

        if (netPtr) {
            tWarn("Already initialized network");
            return true;
        }

        if (_netPrototxtPath.isEmpty()) {
            tDebug() << "getNeuralNetwork is empty";
            return false;
        }
        _trainedNet = new Net<float>(_netPrototxtPath.toStdString(), TEST);
        tDebug() << "Loads layer: " << _netPrototxtPath;
        netPtr->CopyTrainedLayersFrom(trainedModelFilePath.toStdString());
        tDebug() << "Loads trained-model: " << trainedModelFilePath;

        const vector<Blob<float>*> &netInputBlobs = netPtr->input_blobs();
        if (netInputBlobs.size() == 0) {
            tError() << "Bad prototxt file specified : " << _netPrototxtPath;
            release();
            return false;
        }

        tDebug() << "net blob num: " << inputNumOfBlobShape();
        tDebug() << "net blob channels:" << inputImageChannels();
        tDebug() << "net blob width:" << inputImageWidth();
        tDebug() << "net blob height:" << inputImageHeight();

        QFileInfo meanInfo(meanFilePath);

        if (!meanInfo.exists()) {
            tWarn() << "mean file not found: " << meanInfo.filePath();
        } else {
            auto suffix = meanInfo.suffix().toLower();
            tDebug() << "mean file: " << meanInfo.filePath();

            if (imageSuffix.contains(suffix)) {
                // JPEG or PNG
                _meanImage = Image(meanInfo.filePath());
            } else {
                // Binaryproto
                Blob<float> meanBlob;
                readFromBinaryProto(meanInfo.filePath(), meanBlob, 1, inputImageChannels());
                _meanImage = Prediction::convertMeanBlobToImage(meanBlob);
            }

            tDebug() << "mean width:" << _meanImage.width();
            tDebug() << "mean height:" << _meanImage.height();
            tDebug() << "mean channels:" << _meanImage.channels();

            if (_meanImage.isEmpty()) {
                tWarn() << "mean file is empty: " << meanInfo.filePath();
            } else {
                _meanImage.resize(inputImageWidth(), inputImageHeight());
            }
        }
        return true;

    } catch (...) {
        tError() << "Caught exception";
        return false;
    }
}


void Prediction::release()
{
    if (_trainedNet) {
        delete netPtr;
        _trainedNet = nullptr;
    }
    if (!_meanImage.isEmpty()) {
        _meanImage = Image();
    }
}


const Blob<float> &Prediction::inputShape() const
{
    static Blob<float> dummy;
    if (!netPtr) {
        tWarn() << "Not initialized network";
        return dummy;
    }
    const vector<Blob<float>*> &netInputBlobs = netPtr->input_blobs();
    if (netInputBlobs.size() == 0) {
        tWarn() << "Not initialized network";
        return dummy;
    }
    return *netInputBlobs[0];
}


int Prediction::inputNumOfBlobShape() const
{
    return inputShape().num();
}

int Prediction::inputImageWidth() const
{
    return inputShape().width();
}

int Prediction::inputImageHeight() const
{
return inputShape().height();
}

int Prediction::inputImageChannels() const
{
   return inputShape().channels();
}


// blob に負の値があると正しく変換できないので注意！
Image Prediction::convertMeanBlobToImage(const Blob<float> &blob)
{
    const int channels = blob.channels();
    const int height = blob.height();
    const int width = blob.width();

    int type = (channels == 3) ? CV_8UC3 : CV_8U;
    Image ret(cv::Mat(height, width, type));

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


void Prediction::convertImageToBlob(const Image &image, Blob<float> &blob) const
{
    const int imgChannels = image.channels();
    const int imgWidth = image.width();
    const int imgHeight = image.height();

    if (imgChannels != inputImageChannels()) {
        tWarn() << "mismatch channels: image:" << imgChannels << " network:" << inputImageChannels();
    }

    vector<int> shape{ inputNumOfBlobShape(), imgChannels, imgHeight, imgWidth };
    blob.Reshape(shape);

    float *transdata = blob.mutable_cpu_data();
    for (int h = 0; h < imgHeight; ++h) {
        const uchar *ptr = image._mat.ptr<uchar>(h);
        int imgIndex = 0;
        for (int w = 0; w < imgWidth; ++w) {
            for (int c = 0; c < imgChannels; ++c) {
                int idx = (c * imgHeight + h) * imgWidth + w;
                transdata[idx] = static_cast<float>(ptr[imgIndex++]);
            }
        }
    }
}


bool Prediction::substractMeanImage(const Image &src, const Image &mean, Blob<float> &blob) const
{
    if (src.channels() != mean.channels()) {
        tWarn() << "Bad channels  [" << __FILE__ ":" << __LINE__ << "]";
        return false;
    }

    const int imgChannels = src.channels();
    const int imgWidth = src.width();
    const int imgHeight = src.height();
    tDebug() << "imgHeight:" << imgHeight;
    tDebug() << "imgWidth:" << imgWidth;

    if (imgChannels != inputImageChannels()) {
        tWarn() << "mismatch channels: image:" << imgChannels << " network:" << inputImageChannels();
    }

    vector<int> shape{ inputNumOfBlobShape(), imgChannels, imgHeight, imgWidth };
    blob.Reshape(shape);

    float *transdata = blob.mutable_cpu_data();
    for (int h = 0; h < imgHeight; ++h) {
        const uchar *iptr = src._mat.ptr<uchar>(h);
        const uchar *mptr = mean._mat.ptr<uchar>(h);
        int imgIndex = 0;
        for (int w = 0; w < imgWidth; ++w) {
            for (int c = 0; c < imgChannels; ++c) {
                int idx  = (c * imgHeight + h) * imgWidth + w;
                transdata[idx] = (float)iptr[imgIndex] - mptr[imgIndex];
                imgIndex++;
            }
        }
    }
    return true;
}


QList<float> Prediction::predict(const QString &filePath) const
{
    QList<float> result;

    if (!netPtr) {
        tError() << "Predict Error, net is null";
        return result;
    }

    tDebug() << "predict: " << filePath;
    Image inputImg(filePath);
    if (inputImg.isEmpty()) {
        tError() << "Invalid image specified: " << filePath;
        return result;
    }
    inputImg.normalize();
    inputImg.trim();
    inputImg.resize(inputImageWidth(), inputImageHeight());

    // subtract mean
    Blob<float> inputBlob;
    if (_meanImage.isEmpty()) {
        convertImageToBlob(inputImg, inputBlob);
    } else {
        substractMeanImage(inputImg, _meanImage, inputBlob);
    }

    // Prediction
    netPtr->input_blobs()[0]->CopyFrom(inputBlob);
    const vector<Blob<float>*> &predict = netPtr->Forward();
    int resnum = predict[0]->channels();

    tDebug("prediction result(all:%d):", resnum);
    tDebug() << "input: " << filePath;

    for (int i = 0; i < resnum; i++) {
        result << predict[0]->cpu_data()[i];
    }
    return result;
}


QList<QPair<int, float>> Prediction::predictTop(const QString &filePath, int resultNum) const
{
    QList<QPair<int, float>> rank;

    const auto results = predict(filePath);
    for (int i = 0; i < results.count(); i++) {
        float prd = results[i];

        if (rank.count() < resultNum || prd > rank.last().second) {
            if (rank.count() >= resultNum) {
                rank.removeLast();
            }
            rank.append(qMakePair(i, prd));

            qSort(rank.begin(), rank.end(), [](const QPair<int, float> &p1, const QPair<int, float> &p2) {
                return p1.second > p2.second;
            });
        }
    }
    return rank;
}
