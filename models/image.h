#ifndef IMAGE_H
#define IMAGE_H

#include <QtCore>
#include <opencv2/imgproc/imgproc.hpp>


class Image {
    cv::Mat _mat;

public:
    Image() {}
    Image(const QString &path, bool color = true);
    Image(const Image &other);  // shallow copy
    Image(const cv::Mat &other);
    Image &operator=(const Image &other);  // shallow copy
    ~Image();

    Image clone() const;
    const cv::Mat &mat() const { return _mat; }

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

    // 矩形描画
    void drawRectangle(int x1, int y1, int x2, int y2, const cv::Scalar &color, int thickness=1, int lineType=8);

    // 文字列描画
    void putText(const QString &text, int x, int y, int fontFace=cv::FONT_HERSHEY_SIMPLEX, double fontScale=1.0,
                 const cv::Scalar &color=CV_RGB(0,0,0), int thickness=1);
    void drawLabel(const QString &text, int x, int y, int fontFace=cv::FONT_HERSHEY_SIMPLEX, double fontScale=1.0,
                   const cv::Scalar &color=CV_RGB(0,0,0), int thickness=1, const cv::Scalar &bgColor=CV_RGB(255,255,255),
                   double alpha=1.0);

    // 保存
    bool save(const QString &path) const;

    // エンコード
    QByteArray toEncoded(const QString &ext, const QVector<int> &params=QVector<int>());
};

#endif // IMAGE_H
