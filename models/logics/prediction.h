#ifndef PREDICTION_H
#define PREDICTION_H

#include <QString>
#include <QList>
#include <QPair>
#include "image.h"

namespace caffe {
template<typename T> class Blob;
}


class Prediction
{
public:
    Prediction(const QString &netPrototxtPath);
    ~Prediction();

    bool init(const QString &weightFilePath, const QString &meanFilePath);
    void release();
    QList<float> predict(const QString &filePath) const;
    QList<QPair<int, float>> predictTop(const QString &filePath, int resultNum) const;

    bool substractMeanImage(const Image &src, const Image &mean, caffe::Blob<float> &blob) const;
    void convertImageToBlob(const Image &image, caffe::Blob<float> &blob) const;
    int inputNumOfBlobShape() const;
    int inputImageWidth() const;
    int inputImageHeight() const;
    int inputImageChannels() const;

    static void setGpuEnabled(bool enable) { _gpuEnable = enable; }
    static void setGpuDevice(int device) { _gpuDevice = device; }
    static Image convertMeanBlobToImage(const caffe::Blob<float> &blob);

private:
    const caffe::Blob<float> &inputShape() const;

    QString _netPrototxtPath;
    Image _meanImage;
    void *_trainedNet {nullptr};
    static bool _gpuEnable;
    static int _gpuDevice;
};

#endif // PREDICTION_H
