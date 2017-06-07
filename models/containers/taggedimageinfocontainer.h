#ifndef TAGGEDIMAGEINFOCONTAINER_H
#define TAGGEDIMAGEINFOCONTAINER_H

#include <QtCore/QtCore>
#include "services/taggroup.h"


class TaggedImageInfoContainer
{
public:
    QString path;
    long index;
    long count;

    TagGroup primaryGroup;
    Tag primaryTag;
    QSet<QString> containedGroups;
    QList<Tag> containedTags;
};

Q_DECLARE_METATYPE(TaggedImageInfoContainer);

#endif // TAGGEDIMAGEINFOCONTAINER_H
