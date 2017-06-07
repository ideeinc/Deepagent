#ifndef BOUNDINGBOXSETTINGCONTAINER_H
#define BOUNDINGBOXSETTINGCONTAINER_H

#include <QtCore/QtCore>


class BoundingBoxSettingContainer
{
public:
    QString rowGroup;
    QString columnGroup;
    QVariantList excludeTags;
    QStringList authorizationRequiredColumns;
    QStringList columnOrders;
    QVariantMap savedActionTags;
    QStringList labelGroups;
};

Q_DECLARE_METATYPE(BoundingBoxSettingContainer)

#endif // BOUNDINGBOXSETTINGCONTAINER_H
