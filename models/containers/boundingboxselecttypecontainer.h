#ifndef BOUNDINGBOXSELECTTYPECONTAINER_H
#define BOUNDINGBOXSELECTTYPECONTAINER_H

#include <QtCore/QtCore>


class BoundingBoxSelectTypeContainer
{
public:
    QString user;
    QString listGroupName;
    QString searchText;
    QStringList cols;
    QList<QVariantMap> rows;
};

Q_DECLARE_METATYPE(BoundingBoxSelectTypeContainer)

#endif // BOUNDINGBOXSELECTTYPECONTAINER_H

