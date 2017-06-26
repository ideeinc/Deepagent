#ifndef TRAINCLASSIFYCONTAINER_H
#define TRAINCLASSIFYCONTAINER_H

#include <QtCore>
#include "caffemodel.h"


class TrainClassifyContainer
{
public:
    CaffeModel caffeModel;
    QList<QByteArray> jpegBinList;
    QList<QList<QPair<QString, float>>> predictionsList;
};

Q_DECLARE_METATYPE(TrainClassifyContainer)

#endif // TRAINCLASSIFYCONTAINER_H
