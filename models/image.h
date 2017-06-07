#ifndef IMAGE_H
#define IMAGE_H

#include <QString>
#include <QRect>
#include <opencv2/imgproc/imgproc.hpp>


class Image {
    cv::Mat _mat;
    Image(const cv::Mat &other);

public:
    Image() {}
    Image(const QString &path, bool color = true);
    Image(const Image &other);  // shallow copy
    Image &operator=(const Image &other);  // shallow copy
    ~Image();

    Image clone() const;

    int width() const;
    int height() const;
    int channels() const;
    bool isEmpty() const;

    // 明度の正規化
    void normalize();
    Image normalized() const;

    // 回転 (拡大縮小も可)
    void rotate(float angle, float scale=1.0);
    Image rotated(float angle, float scale=1.0) const;

    // リサイズ
    void resize(int width, int height);
    Image resized(int width, int height) const;

    // 切り抜き
    void crop(int x, int y, int width, int height);
    void crop(const cv::Rect &rect);
    Image cropped(int x, int y, int width, int height) const;
    Image cropped(const cv::Rect &rect) const;

    // トリミング
    void trim();
    Image trimmed() const;
    QRect getValidRect() const;

    // 保存
    bool save(const QString &path) const;

    // // 表示
    // void show(const QString &title)
    // {
    //     cv::namedWindow(qPrintable(title), cv::WINDOW_AUTOSIZE);
    //     cv::imshow(qPrintable(title), mat);
    // }

    friend class Prediction;
};

#endif // IMAGE_H
