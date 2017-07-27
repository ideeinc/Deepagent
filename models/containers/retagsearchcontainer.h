#ifndef REWTAGSEARCHCONTAINER_H
#define REWTAGSEARCHCONTAINER_H

#include <QtCore>


class RetagSearchContainer
{
public:
    QStringList images;

    friend QDataStream &operator<<(QDataStream &stream, const RetagSearchContainer &container);
    friend QDataStream &operator>>(QDataStream &stream, RetagSearchContainer &container);
};

Q_DECLARE_METATYPE(RetagSearchContainer)

#endif // REWTAGSEARCHCONTAINER_H
