#ifndef SSDDETECTOR_H
#define SSDDETECTOR_H

#include <vector>
#include <QtCore>
#include <TGlobal>
using namespace std;

namespace caffe {
template<typename T> class Blob;
template<typename T> class Net;
}
namespace cv {
class Mat;
}


class SsdDetector
{
public:
    SsdDetector(const string& model_file, const string& weights_file, const string& mean_file, const string& mean_value);
    ~SsdDetector();

    QList<QVector<float>> detect(const cv::Mat& img, float threshold);
    void reset(const string& model_file, const string& weights_file, const string& mean_file, const string& mean_value);

    static QList<QVector<float>> detect(const QString &imgFile, float threshold, const QString& modelFile, const QString &weightsFile, const QString &meanValue);

private:
    void SetMean(const string& mean_file, const string& mean_value);
    void WrapInputLayer(std::vector<cv::Mat>* input_channels);
    void Preprocess(const cv::Mat& img, std::vector<cv::Mat>* input_channels);

    struct SsdDetectorPrivate;
    SsdDetectorPrivate *d {nullptr};

    T_DISABLE_COPY(SsdDetector)
    T_DISABLE_MOVE(SsdDetector)
};

#endif // SSDDETECTOR_H
