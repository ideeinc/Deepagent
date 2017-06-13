#ifndef TRAINCREATECONTAINER_H
#define TRAINCREATECONTAINER_H

#include <QtCore>
#include "caffemodel.h"


class TrainCreateContainer
{
public:
    CaffeModel caffeModel;

    friend QDataStream &operator<<(QDataStream &stream, const TrainCreateContainer &container);
    friend QDataStream &operator>>(QDataStream &stream, TrainCreateContainer &container);
};

Q_DECLARE_METATYPE(TrainCreateContainer)

#endif // TRAINCREATECONTAINER_H
