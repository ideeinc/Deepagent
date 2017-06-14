#ifndef TRAININDEXCONTAINER_H
#define TRAININDEXCONTAINER_H

#include <QtCore>
#include "caffemodel.h"


class TrainIndexContainer
{
public:
    // class TrainModel
    // {
    // public:
    //     QString id;
    //     QString name;
    //     QString state;
    //     QString created;
    // };

    QList<CaffeModel> trainingModels;
};

//Q_DECLARE_METATYPE(TrainIndexContainer::TrainModel)
Q_DECLARE_METATYPE(TrainIndexContainer)

#endif // TRAININDEXCONTAINER_H
