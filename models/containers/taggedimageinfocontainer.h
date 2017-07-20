#ifndef TAGGEDIMAGEINFOCONTAINER_H
#define TAGGEDIMAGEINFOCONTAINER_H

#include <QtCore/QtCore>
#include "taggroup.h"


class TaggedImageInfoContainer
{
public:
    QString path;
    QString displayName;
    long index { 0 };
    long numberOfImages { 0 };

    QString primaryGroup;
    QString primaryTag;
    QSet<QString> containedGroups;
    QList<Tag> containedTags;

    QString listName;
    QStringList listArgs;
    QVariantMap listQuery;
};

Q_DECLARE_METATYPE(TaggedImageInfoContainer);

#endif // TAGGEDIMAGEINFOCONTAINER_H
