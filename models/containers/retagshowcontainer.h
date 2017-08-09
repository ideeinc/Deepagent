#ifndef REWTAGSHOWCONTAINER_H
#define REWTAGSHOWCONTAINER_H

#include <QtCore>
#include "tag.h"


class RetagShowContainer
{
public:
    QString imagePath;
    QString originalName;
    QSize imageSize;
    QList<Tag> tags;
    int index {0};
    int queueCount {0};
    QString backPath;

    // friend QDataStream &operator<<(QDataStream &stream, const RetagShowContainer &container);
    // friend QDataStream &operator>>(QDataStream &stream, RetagShowContainer &container);
};

Q_DECLARE_METATYPE(RetagShowContainer)

#endif // REWTAGSHOWCONTAINER_H
