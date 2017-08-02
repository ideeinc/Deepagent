#ifndef JPEGINFO_H
#define JPEGINFO_H

#include <QtCore>


class JpegInfo
{
public:
    JpegInfo(const QString &file);

    bool isEmpty() const { return _width <= 0 && _height <= 0; }
    int width() const { return _width; }
    int height() const { return _height; }
    QSize size() const { return QSize(_width, _height); }
    int colorComponents() const { return _comps; }
    int bitsPerSample() const { return _bit; }

private:
    void parse(const QString &file);

    quint16 _width {0};
    quint16 _height {0};
    quint8 _comps {0};
    quint8 _bit {0};
};

#endif // JPEGINFO_H
