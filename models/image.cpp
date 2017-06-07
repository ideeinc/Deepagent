#include "image.h"
#include <TDebug>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

#ifdef STANDALONE_USE
# undef tError
# undef tWarn
# undef tDebug
# define tError qCritical
# define tWarn qWarning
# define tDebug qDebug
#endif


Image::Image(const QString &path, bool color)
    : _mat(imread(qPrintable(path), (color ? 1 : 0)))
{ }

Image::Image(const Image &other)  // shallow copy
    : _mat(other._mat)
{ }

Image &Image::operator=(const Image &other)  // shallow copy
{
    _mat.release();
    _mat = other._mat;
    return *this;
}

Image::Image(const Mat &other)
    : _mat(other)
{ }

Image::~Image() {}

Image Image::clone() const
{
    return Image(_mat.clone());
}

int Image::width() const { return _mat.cols; }
int Image::height() const { return _mat.rows; }
int Image::channels() const { return _mat.channels(); }
bool Image::isEmpty() const { return _mat.empty(); }

// 明度の正規化
void Image::normalize()
{
    if (isEmpty()) {
        return;
    }
    operator=(normalized());
}

// 明度の正規化
Image Image::normalized() const
{
    if (isEmpty()) {
        return clone();
    }

    double min, max;
    cv::minMaxLoc(_mat, &min, &max);
    if ((int)min == 0 && (int)max == 255) {
        return clone();
    }
    Mat dst;
    cv::convertScaleAbs(_mat, dst, 255.0/(max-min), -255.0*min/(max-min));
    return Image(dst);
}

// 回転 (拡大縮小も可)
void Image::rotate(float angle, float scale)
{
    if (isEmpty()) {
        return;
    }
    operator=(rotated(angle, scale));
}

// 回転 (拡大縮小も可)
Image Image::rotated(float angle, float scale) const
{
    if (isEmpty()) {
        return clone();
    }

    if (scale < 1.0) {
        // zoom out
        int type = (channels() == 3) ? CV_8UC3 : CV_8U;
        Mat dst = Mat::ones(height(), width(), type);  // 元のサイズ
        int w = _mat.cols * scale;
        int h = _mat.rows * scale;
        int x = (width()-w)/2;
        int y = (height()-h)/2;
        Image img = resized(w, h);

        cv::Rect rect(x,y,w,h);
        cv::Mat submat = dst(rect);
        img._mat.copyTo(submat);  // 貼り付け
        return img.rotated(angle, 1.0);
    }

    // zoom in
    Mat dst;
    Point2f center(_mat.cols/2.0, _mat.rows/2.0); // 画像の中心
    Mat matrix = cv::getRotationMatrix2D(center, angle, scale);
    warpAffine(_mat, dst, matrix, _mat.size(), cv::INTER_CUBIC);
    return Image(dst);
}

// リサイズ
void Image::resize(int w, int h)
{
    if ((w == width() && h == height()) || isEmpty()) {
        return;
    }
    operator=(resized(w, h)); // shallow copy
}

    // リサイズ
Image Image::resized(int w, int h) const
{
    if ((w == width() && h == height()) || isEmpty()) {
        return clone();
    }

    // 出力画像
    int type = (channels() == 3) ? CV_8UC3 : CV_8U;
    Mat dst = Mat::ones(h, w, type);
    if (w < width() || h < height()) {
        // 縮小
        cv::resize(_mat, dst, dst.size(), 0.5, 0.5, cv::INTER_AREA);
    } else {
        // 拡大
        cv::resize(_mat, dst, dst.size(), cv::INTER_CUBIC);
    }
    return Image(dst); // shallow copy
}

// 切り抜き
void Image::crop(int x, int y, int width, int height)
{
    if (isEmpty()) {
        return;
    }
    operator=(cropped(x, y, width, height)); // shallow copy
}

void Image::crop(const cv::Rect &rect)
{
    if (isEmpty()) {
        return;
    }
    operator=(cropped(rect)); // shallow copy
}

// 切り抜き
Image Image::cropped(int x, int y, int width, int height) const
{
    if (isEmpty()) {
        return clone();
    }
    return cropped(cv::Rect(x, y, width, height));
}

// 切り抜き
Image Image::cropped(const cv::Rect &rect) const
{
    if (isEmpty()) {
        return clone();
    }

    if (rect.x + rect.width > width() || rect.y + rect.height > height() || rect.x < 0 || rect.y < 0) {
        tError("Invalid crop size x:%d y:%d w:%d h:%d  orig img w:%d, h:%d", rect.x, rect.y,
               rect.width, rect.height, width(), height());
        return Image();
    }

    Image img(Mat(_mat, rect));
    tDebug("cropped image w:%d  h:%d", img.width(), img.height());
    return img;
}

// トリミング
void Image::trim()
{
    if (isEmpty()) {
        return;
    }
    operator=(trimmed()); // shallow copy
}

// トリミング
Image Image::trimmed() const
{
    if (isEmpty()) {
        return clone();
    }
    auto rect = getValidRect();
    return cropped(rect.x(), rect.y(), rect.width(), rect.height());
}

QRect Image::getValidRect() const
{
    const int THRESHOLD_MIN = 16;

    if (isEmpty()) {
        return QRect();
    }

    cv::Mat gray, binary;
    cvtColor(_mat, gray,CV_RGB2GRAY);
    cv::threshold(gray, binary, THRESHOLD_MIN, 255, cv::THRESH_BINARY);

    vector<vector<Point>> contours;
    findContours(binary, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    vector<Point> points;
    for (auto &con : contours) {
        for (auto &p : con) {
            points.push_back(p);
        }
    }

    cv::Rect brect = cv::boundingRect(cv::Mat(points));
    int x = qMax(brect.x-1, 0);
    int y = qMax(brect.y-1, 0);
    tDebug("trim image x:%d ,y:%d  w:%d  h:%d", x, y, brect.width+2,  brect.height+2);
    return QRect(x, y, brect.width+2, brect.height+2);
}

// JPEG保存
bool Image::save(const QString &path) const
{
    if (isEmpty()) {
        tError("Cannot save empty image");
        return false;
    }

    std::vector<int> params(2);
    params[0] = CV_IMWRITE_JPEG_QUALITY; // JPEG品質
    params[1] = 95;
    return imwrite(qPrintable(path), _mat, params);
}
