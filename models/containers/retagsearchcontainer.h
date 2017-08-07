#ifndef REWTAGSEARCHCONTAINER_H
#define REWTAGSEARCHCONTAINER_H

#include <QtCore>
#include "taggroup.h"


class RetagSearchContainer
{
public:
    QStringList images;
    QList<TagGroup> groups;
};

Q_DECLARE_METATYPE(RetagSearchContainer)

#endif // REWTAGSEARCHCONTAINER_H
