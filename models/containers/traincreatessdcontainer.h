#ifndef TRAINCREATESSDCONTAINER_H
#define TRAINCREATESSDCONTAINER_H

#include <QtCore>


class TrainCreateSsdContainer
{
public:
    QVariantMap ssdParam;

    friend QDataStream &operator<<(QDataStream &stream, const TrainCreateSsdContainer &container);
    friend QDataStream &operator>>(QDataStream &stream, TrainCreateSsdContainer &container);
};

Q_DECLARE_METATYPE(TrainCreateSsdContainer)

#endif // TRAINCREATESSDCONTAINER_H
