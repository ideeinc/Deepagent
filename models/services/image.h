#include <QtCore/QtCore>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

// const int OUTPUT_WIDTH  = 256;
// const int OUTPUT_HEIGHT = 256;
// const float ROTATION_RANGE = 180.0; // 回転±
// const float ZOOM_RANGE = 0.2; // ズーム幅(縮小はしない、回転すると黒の余白が増えるため)
// const int MULTIPLE = 1;


class Image {
    Mat _mat;
    Image(const Mat &other) { _mat = other; }

public:
    Image() {}
    Image(const QString &path);
    Image(const Image &other);  // shallow copy
    Image &operator=(const Image &other);  // shallow copy
    ~Image();

    Image clone() const;

    int width() const;
    int height() const;
    int channels() const;

    // 明度の正規化
    void normalize();

    // 明度の正規化
    Image normalized() const;

    // 回転 (拡大縮小も可)
    Image rotated(float angle, float scale=1.0) const;

    // 回転 (拡大縮小も可)
    void rotate(float angle, float scale=1.0);

    // リサイズ
    void resize(int width, int height);

    // リサイズ
    Image resized(int width, int height) const;

    // 切り抜き
    Image clip(int x, int y, int width, int height) const;

    // 切り抜き
    Image clip(const cv::Rect &rect) const;

    // トリミング
    void trim();

    // トリミング
    Image trimmed() const;

    // 間引く
    bool substract(const Image &mean);

    // 間引く
    Image substracted(const Image &mean) const;

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
