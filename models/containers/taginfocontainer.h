#ifndef TAGINFOCONTAINER_H
#define TAGINFOCONTAINER_H

#include <QtCore/QtCore>


class TagInfoContainer
{
public:
    bool available;
    QStringList arguments;
    QVariantMap query;
    QString filter;
    QString name;
    QString displayName;
    QString groupName;
    QStringList images;
    int limit { 200 };
    int page { 0 };
    int min { 0 };
    int max { 0 };
    int maxNumberOfPage { 0 };
};

Q_DECLARE_METATYPE(TagInfoContainer)

#endif // TAGINFOCONTAINER_H
