#ifndef REWTAGSEARCHCONTAINER_H
#define REWTAGSEARCHCONTAINER_H

#include <QtCore>
#include <TPaginator>
#include "taggroup.h"


class RetagSearchContainer
{
public:
    QStringList images;
    QList<TagGroup> groups;
    Tag selectedTag[3]  {Tag(), Tag(), Tag()};
    int imageCount {0};
    TPaginator pager;
};

Q_DECLARE_METATYPE(RetagSearchContainer)

#endif // REWTAGSEARCHCONTAINER_H
