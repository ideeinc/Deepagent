#include <QtCore/QtCore>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <random>
#include <cmath>
using namespace cv;

const int OUTPUT_WIDTH  = 256;
const int OUTPUT_HEIGHT = 256;
const float ROTATION_RANGE = 180.0; // 回転±
const float ZOOM_RANGE = 0.2; // ズーム幅(縮小はしない、回転すると黒の余白が増えるため)
const int MULTIPLE = 1;


class Image {
    Mat mat;
    Image(const Mat &other) { mat = other; }

public:
    Image() {}

    Image(const QString &path)
    {
        mat = cv::imread(qPrintable(path), 1);
    }

    Image(const Image &other)  // shallow copy
    {
        mat = other.mat;
    }

    Image &operator=(const Image &other)  // shallow copy
    {
        mat = other.mat;
        return *this;
    }

    ~Image() {}

    Image clone() const
    {
        return Image(mat.clone());
    }

    int width() const { return mat.cols; }
    int height() const { return mat.rows; }

    // 明度の正規化
    void normalize()
    {
        operator=(normalized());
    }

    // 明度の正規化
    Image normalized() const
    {
        double min, max;
        cv::minMaxLoc(mat, &min, &max);
        if ((int)min != 0 || (int)max != 255) {
            Mat dst;
            cv::convertScaleAbs(mat, dst, 255.0/(max-min), -255.0*min/(max-min));
            return Image(dst);
        }
        return clone();
    }

    // 回転 (拡大縮小も可)
    Image rotated(float angle, float scale=1.0) const
    {
        if (scale < 1.0) {
            // zoom out
            Mat dst = Mat::ones(height(), width(), CV_8UC3);  // 元のサイズ
            int w = mat.cols * scale;
            int h = mat.rows * scale;
            int x = (width()-w)/2;
            int y = (height()-h)/2;
            Image img = resized(w, h);

            cv::Rect rect(x,y,w,h);
            cv::Mat submat = dst(rect);
            img.mat.copyTo(submat);  // 貼り付け
            return img.rotated(angle, 1.0);
        }

        Mat dst;
        Point2f center(mat.cols/2.0, mat.rows/2.0); // 画像の中心
        Mat matrix = cv::getRotationMatrix2D(center, angle, scale);
        warpAffine(mat, dst, matrix, mat.size(), cv::INTER_CUBIC);
        return Image(dst);
    }

    // 回転 (拡大縮小も可)
    void rotate(float angle, float scale=1.0)
    {
        operator=(rotated(angle, scale));
    }

    // リサイズ
    void resize(int width, int height)
    {
        if (width == mat.cols && height == mat.rows) {
            return;
        }
        operator=(resized(width, height)); // shallow copy
    }

        // リサイズ
    Image resized(int width, int height) const
    {
        if (width == mat.cols && height == mat.rows) {
            return Image(mat.clone());
        }

        // 出力画像
        Mat dst = Mat::ones(height, width, CV_8U);
        if (width < mat.cols || height < mat.rows) {
            // 縮小
            cv::resize(mat, dst, dst.size(), 0.5, 0.5, cv::INTER_AREA);
        } else {
            // 拡大
            cv::resize(mat, dst, dst.size(), cv::INTER_CUBIC);
        }
        return Image(dst); // shallow copy
    }

    // 切り抜き
    Image clip(int x, int y, int width, int height) const
    {
        return clip(cv::Rect(x, y, width, height));
    }

    // 切り抜き
    Image clip(const cv::Rect &rect) const
    {
        return Image(Mat(mat, rect));
    }

    // トリミング
    void trim()
    {
        auto image = trimmed();
        operator=(image); // shallow copy
    }

    // トリミング
    Image trimmed() const
    {
        const int THRESH_MIN = 16;
        cv::Mat gray, binary;
        cvtColor(mat, gray,CV_RGB2GRAY);
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

    // 保存
    bool save(const QString &path) const
    {
        std::vector<int> params(2);
        params[0] = CV_IMWRITE_JPEG_QUALITY; // JPEG品質
        params[1] = 95;
        return imwrite(qPrintable(path), mat, params);
    }

    // 表示
    void show(const QString &title)
    {
        cv::namedWindow(qPrintable(title), cv::WINDOW_AUTOSIZE);
        cv::imshow(qPrintable(title), mat);
    }
};


class ImageGenerator {
    float rotationRange  {ROTATION_RANGE};
    float zoomRange  {ZOOM_RANGE};

public:
    static uint rand(uint min, uint max)
    {
        static std::random_device randev;
        static std::mt19937 mt(randev());
        static QMutex mutex;

        mutex.lock();
        std::uniform_int_distribution<uint> uniform(min, max);
        uint ret = uniform(mt);
        mutex.unlock();
        return ret;
    }

    static float randf(float min, float max)
    {
        float num = rand(0, UINT_MAX)/(float)UINT_MAX;  // 0.0 - 1.0
        float ret = std::abs(max-min) * num + qMin(max, min);
        return ret;
    }

    // 生成
    void generate(const QString &srcDirPath, const QString &dstDirPath, int multiple) const
    {
        if (QFileInfo(srcDirPath).absolutePath() == QFileInfo(dstDirPath).absolutePath()) {
            qDebug() << "change dst directory";
            return;
        }

        QDir dstDir(dstDirPath);
        if (!dstDir.exists()) {
            dstDir.mkpath(".");
        }
        QDir srcDir(srcDirPath);

        const QStringList filter = { "*.jpg", "*.jpeg" };
        for (auto &f : srcDir.entryInfoList(filter, QDir::Files | QDir::QDir::Readable)) {
            auto jpg = f.absoluteFilePath();
            Image origImg(qPrintable(jpg));
            origImg.normalize();
            origImg.trim();

            for (int i = 0; i < multiple; i++) {
                Image img;
                if (i > 0) {
                    // zoom and rotate
                    float deg = randf(-rotationRange, rotationRange);
                    float zoom = randf(0, zoomRange);
                    zoom = (zoom >= 0.0) ? 1.0+zoom : 1.0/(1.0+std::abs(zoom));
                    //qDebug() << "deg:" << deg << "zoom:" << zoom;
                    img = origImg.rotated(deg, zoom);
                } else {
                    img = origImg.clone();
                }

                img.resize(OUTPUT_WIDTH, OUTPUT_HEIGHT); // resize
                QString dstName = f.completeBaseName() + "_" + QString::number(i) + "." + f.suffix();
                img.save(dstDir.absoluteFilePath(dstName));
                //img.show(jpg);
                //cv::waitKey(0);
            }
        }
    }
};


int main(int argc, const char *argv[])
{
    switch (argc) {
    case 2: {
        Image image(argv[1]);
        image.normalize();
        image.trim();
        //image.rotate(20, 1.2);
        image.resize(OUTPUT_WIDTH, OUTPUT_HEIGHT);
        image.show("RESIZED");
        cv::waitKey(0);
        break; }

    case 3:
        ImageGenerator().generate(argv[1], argv[2], MULTIPLE);
        break;

   case 4:
        ImageGenerator().generate(argv[1], argv[3], atoi(argv[2]));
        break;

    default:
        qDebug() << "bad arguments";
        return 1;
    }
    return 0;
}

// CompileOptions: -m64 -O2 -fPIC -pipe -std=c++0x -D_REENTRANT -DQT_NO_DEBUG -DQT_CORE_LIB -isystem /usr/include/x86_64-linux-gnu/qt5 -I. -Wl,-O2 -lopencv_core -lopencv_highgui -lopencv_imgproc -lQt5Core
// command: cpi /path/to/input_dir multiple /path/to/output_dir
