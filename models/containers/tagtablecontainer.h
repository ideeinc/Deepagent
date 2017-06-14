#ifndef TAGTABLECONTAINER_H
#define TAGTABLECONTAINER_H

#include <QtCore/QtCore>


class TagTableContainer
{
public:
    QString rowGroupName;
    QString colGroupName;

    QStringList rowTagNames;
    QStringList colTagNames;

    QStringList headers;
    QList<QStringList> rows;
    QStringList footers;
};

Q_DECLARE_METATYPE(TagTableContainer)

#endif // TAGTABLECONTAINER_H

