#include "containers/retagsearchcontainer.h"
#include <TGlobal>


QDataStream &operator<<(QDataStream &stream, const RetagSearchContainer &container)
{
    stream << container.images;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, RetagSearchContainer &container)
{
    stream >> container.images;
    return stream;
}

T_REGISTER_STREAM_OPERATORS(RetagSearchContainer)
