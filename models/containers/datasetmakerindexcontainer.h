#ifndef DATASETMAKERINDEXCONTAINER_H
#define DATASETMAKERINDEXCONTAINER_H

#include "services/taggroup.h"
#include <QtCore/QtCore>


class DatasetMakerIndexContainer
{
public:
    QList<Tag> excludes;
    QStringList archives;
};

Q_DECLARE_METATYPE(DatasetMakerIndexContainer)

#endif // DATASETMAKERINDEXCONTAINER_H
