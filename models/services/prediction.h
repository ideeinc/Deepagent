#ifndef PREDICTION_H
#define PREDICTION_H

#include <QStringList>
#include <QPair>
#include "caffetrainedmodel.h"

namespace caffe {
template<typename T> class Blob;
}
class Image;


class Prediction
{
public:
    Prediction(const CaffeTrainedModel &trainedModel);
    ~Prediction();

    bool init();
    void release();
    QList<QPair<QString, float>> predict(const QString &filePath) const;

    static void setGpuEnabled(bool enable) {}
    static void setGpuDevice(int device) {}
    static void convertImageToBlob(const Image &image, caffe::Blob<float> &blob);
    static Image convertBlobToImage(const caffe::Blob<float> &blob);

private:
    CaffeTrainedModel _trainedModel;
    void *_trainedNet {nullptr};
};

#endif // PREDICTION_H
