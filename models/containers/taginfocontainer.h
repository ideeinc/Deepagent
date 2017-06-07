#ifndef TAGINFOCONTAINER_H
#define TAGINFOCONTAINER_H

#include <QtCore/QtCore>


class TagInfoContainer
{
public:
    bool available;
    QString name;
    QString displayName;
    QString groupName;
    QStringList images;
    long itemsPerPage { 200 };
    long page { 0 };
    long min { 0 };
    long max { 0 };
    long maxNumberOfPage { 0 };
};

Q_DECLARE_METATYPE(TagInfoContainer)

#endif // TAGINFOCONTAINER_H
