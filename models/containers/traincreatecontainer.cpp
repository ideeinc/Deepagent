#include "containers/traincreatecontainer.h"


QDataStream &operator<<(QDataStream &stream, const TrainCreateContainer &container)
{
    stream << container.trainingModel;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, TrainCreateContainer &container)
{
    stream >> container.trainingModel;
    return stream;
}

T_REGISTER_STREAM_OPERATORS(TrainCreateContainer)
