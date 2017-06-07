#ifndef TRAINSHOWCONTAINER_H
#define TRAINSHOWCONTAINER_H

#include <QtCore>
#include "caffemodel.h"


class TrainShowContainer
{
public:
    CaffeModel trainingModel;
    QString solverLink;
    QString trainLink;
    QString deployLink;
    QString logLink;
    QStringList graphImages;
};

Q_DECLARE_METATYPE(TrainShowContainer)

#endif // TRAINSHOWCONTAINER_H
