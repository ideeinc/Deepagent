#ifndef SSDDETECTOR_H
#define SSDDETECTOR_H

#include <caffe/caffe.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include <QtCore>
using namespace std;


class SsdDetector
{
public:
    SsdDetector(const string& model_file, const string& weights_file, const string& mean_file, const string& mean_value);
    vector<vector<float>> Detect(const cv::Mat& img);

    static QList<QVector<float>> detect(const QString &imgFile, float threshold, const QString& modelFile, const QString &weightsFile, const QString &meanValue);

private:
    void SetMean(const string& mean_file, const string& mean_value);
    void WrapInputLayer(std::vector<cv::Mat>* input_channels);
    void Preprocess(const cv::Mat& img, std::vector<cv::Mat>* input_channels);

    shared_ptr<caffe::Net<float>> net_;
    cv::Size input_geometry_;
    int num_channels_;
    cv::Mat mean_;
};

#endif // SSDDETECTOR_H
