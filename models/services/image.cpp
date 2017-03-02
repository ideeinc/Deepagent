#include "image.h"
#include <TDebug>


Image::Image(const QString &path)
{
    _mat = cv::imread(qPrintable(path), 1);
}

Image::Image(const Image &other)  // shallow copy
{
    _mat = other._mat;
}

Image &Image::operator=(const Image &other)  // shallow copy
{
    _mat = other._mat;
    return *this;
}

Image::~Image() {}

Image Image::clone() const
{
    return Image(_mat.clone());
}

int Image::width() const { return _mat.cols; }
int Image::height() const { return _mat.rows; }
int Image::channels() const { return _mat.channels(); }

// 明度の正規化
void Image::normalize()
{
    operator=(normalized());
}

// 明度の正規化
Image Image::normalized() const
{
    double min, max;
    cv::minMaxLoc(_mat, &min, &max);
    if ((int)min != 0 || (int)max != 255) {
        Mat dst;
        cv::convertScaleAbs(_mat, dst, 255.0/(max-min), -255.0*min/(max-min));
        return Image(dst);
    }
    return clone();
}

// 回転 (拡大縮小も可)
Image Image::rotated(float angle, float scale) const
{
    if (scale < 1.0) {
        // zoom out
        Mat dst = Mat::ones(height(), width(), CV_8UC3);  // 元のサイズ
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

    Mat dst;
    Point2f center(_mat.cols/2.0, _mat.rows/2.0); // 画像の中心
    Mat matrix = cv::getRotationMatrix2D(center, angle, scale);
    warpAffine(_mat, dst, matrix, _mat.size(), cv::INTER_CUBIC);
    return Image(dst);
}

// 回転 (拡大縮小も可)
void Image::rotate(float angle, float scale)
{
    operator=(rotated(angle, scale));
}

// リサイズ
void Image::resize(int width, int height)
{
    if (width == _mat.cols && height == _mat.rows) {
        return;
    }
    operator=(resized(width, height)); // shallow copy
}

    // リサイズ
Image Image::resized(int width, int height) const
{
    if (width == _mat.cols && height == _mat.rows) {
        return Image(_mat.clone());
    }

    // 出力画像
    Mat dst = Mat::ones(height, width, CV_8U);
    if (width < _mat.cols || height < _mat.rows) {
        // 縮小
        cv::resize(_mat, dst, dst.size(), 0.5, 0.5, cv::INTER_AREA);
    } else {
        // 拡大
        cv::resize(_mat, dst, dst.size(), cv::INTER_CUBIC);
    }
    return Image(dst); // shallow copy
}

// 切り抜き
Image Image::clip(int x, int y, int width, int height) const
{
    return clip(cv::Rect(x, y, width, height));
}

// 切り抜き
Image Image::clip(const cv::Rect &rect) const
{
    return Image(Mat(_mat, rect));
}

// トリミング
void Image::trim()
{
    operator=(trimmed()); // shallow copy
}

// トリミング
Image Image::trimmed() const
{
    const int THRESH_MIN = 16;
    cv::Mat gray, binary;
    cvtColor(_mat, gray,CV_RGB2GRAY);
    cv::threshold(gray, binary, THRESH_MIN, 255, cv::THRESH_BINARY);

    cv::vector<cv::vector<Point>> contours;
    findContours(binary, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    cv::vector<Point> points;
    for (auto &con : contours) {
        for (auto &p : con) {
            points.push_back(p);
        }
    }

    cv::Rect brect = cv::boundingRect(cv::Mat(points));
    //printf("x:%d ,y:%d  w:%d  h:%d\n", brect.x, brect.y, brect.width,  brect.height);
    return clip(brect.x+1, brect.y+1, brect.width-2, brect.height-2);
}

// 間引く
bool Image::substract(const Image &mean)
{
    if (width() != mean.width() || height() != mean.height()
        || channels() != mean.channels()) {
        tWarn() << "Bad mean image";
        return false;
    }
    operator=(substracted(mean));
    return true;
}

// 間引く
Image Image::substracted(const Image &mean) const
{
    if (width() != mean.width() || height() != mean.height()
        || channels() != mean.channels()) {
        tWarn() << "Bad mean image";
        return clone();
    }

    Image ret = clone();
    for (int h = 0; h < height(); ++h) {
        uchar *ptr = ret._mat.ptr<uchar>(h);
        const uchar* meanPtr = mean._mat.ptr<uchar>(h);
        int imgIndex = 0;
        for (int w = 0; w < width(); ++w) {
            for (int c = 0; c < channels(); ++c) {
                ptr[imgIndex] = qMax((int)ptr[imgIndex] - meanPtr[imgIndex], 0);
                imgIndex++;
            }
        }
    }
    return ret;
}

// 保存
bool Image::save(const QString &path) const
{
    std::vector<int> params(2);
    params[0] = CV_IMWRITE_JPEG_QUALITY; // JPEG品質
    params[1] = 95;
    return imwrite(qPrintable(path), _mat, params);
}
