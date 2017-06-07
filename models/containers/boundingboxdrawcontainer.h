#ifndef BOUNDINGBOXDRAWCONTAINER_H
#define BOUNDINGBOXDRAWCONTAINER_H

#include <QtCore/QtCore>


class BoundingBoxDrawContainer
{
public:
    QString user;
    QString group;
    QString label;
    QString displayName;
    int index {0};
    int jpegCount {0};
    QFileInfo jpegFile;
    int prevIndex {0};
    int nextIndex {0};
    QSize originalSize;
};

Q_DECLARE_METATYPE(BoundingBoxDrawContainer)

#endif // BOUNDINGBOXDRAWCONTAINER_H
