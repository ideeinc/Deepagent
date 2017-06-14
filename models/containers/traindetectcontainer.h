#ifndef TRAINDETECTCONTAINER_H
#define TRAINDETECTCONTAINER_H

#include <QtCore>
#include "caffemodel.h"


class TrainDetectContainer
{
public:
    CaffeModel caffeModel;
    QByteArray jpegBin;
    QList<QVector<float>> detections;
};

Q_DECLARE_METATYPE(TrainDetectContainer)

#endif // TRAINDETECTCONTAINER_H
