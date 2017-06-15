#ifndef TRAINDETECTCONTAINER_H
#define TRAINDETECTCONTAINER_H

#include <QtCore>
#include "caffemodel.h"


class TrainDetectContainer
{
public:
    CaffeModel caffeModel;
    QList<QByteArray> jpegBinList;
    QList<QList<QVector<float>>> detectionsList;
};

Q_DECLARE_METATYPE(TrainDetectContainer)

#endif // TRAINDETECTCONTAINER_H
