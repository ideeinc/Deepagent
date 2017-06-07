#ifndef BOUNDINGBOXMAKEDATASETCONTAINER_H
#define BOUNDINGBOXMAKEDATASETCONTAINER_H

#include <QtCore/QtCore>


class BoundingBoxMakeDatasetContainer
{
public:
    QMap<QString, QString> availableLabels;
    QMap<QString, QString> savedLabels;
    QMap<QString, long> directories;
};

Q_DECLARE_METATYPE(BoundingBoxMakeDatasetContainer)

#endif // BOUNDINGBOXMAKEDATASETCONTAINER_H
