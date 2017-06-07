#ifndef FORMATCONVERTER_H
#define FORMATCONVERTER_H

#include <QString>
#include <QList>

struct BoundingBox {
    QString name;
    bool truncated {false};
    QRect bbox;
};


class FormatConverter {
public:
    enum class Type {
        KITTI = 0,
        PASCAL_VOC,
    };

    explicit FormatConverter(const QString &srcFilePath, Type type, const QString &imageFilePath);
    QString convertTo(Type type) const;
    bool convertTo(Type type, const QString &dstFilePath) const;

private:
    void parse(const QString &srcFilePath);

    Type _srcType;
    QString _imageFile;
    QList<BoundingBox> _bndboxList;
};

#endif // FORMATCONVERTER_H
