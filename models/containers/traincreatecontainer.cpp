#include "containers/traincreatecontainer.h"


QDataStream &operator<<(QDataStream &stream, const TrainCreateContainer &container)
{
    stream << container.caffeModel;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, TrainCreateContainer &container)
{
    stream >> container.caffeModel;
    return stream;
}

T_REGISTER_STREAM_OPERATORS(TrainCreateContainer)
