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
    SsdDetector(const QString &modelFile, const QString &weightsFile, const QString &meanFile, const QString &meanValue);
    ~SsdDetector();

    QList<QVector<float>> detect(const cv::Mat& img, float threshold) const;
    bool detect(const QString& movieFile, float threshold, const QString& outputFile) const;
    void reset(const QString &modelFile, const QString &weightsFile, const QString &meanFile, const QString &meanValue);

    static QList<QVector<float>> detect(const QString &imgFile, float threshold, const QString& modelFile, const QString &weightsFile, const QString &meanValue);

private:
    void setMean(const QString &meanFile, const QString &meanValue);
    std::vector<cv::Mat> wrapInputLayer() const;
    void preprocess(const cv::Mat& img, const std::vector<cv::Mat>& inputChannels) const;

    struct SsdDetectorPrivate;
    SsdDetectorPrivate *d {nullptr};

    T_DISABLE_COPY(SsdDetector)
    T_DISABLE_MOVE(SsdDetector)
};

#endif // SSDDETECTOR_H
