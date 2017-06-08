#ifndef DATASETMAKERSERVICE_H
#define DATASETMAKERSERVICE_H

#include "containers/datasetmakerindexcontainer.h"
#include "containers/datasetmakerpreviewcontainer.h"
#include "containers/datasetmakerresultcontainer.h"

class THttpRequest;
class TSession;


class DatasetMakerService
{
public:
    DatasetMakerIndexContainer index();
    DatasetMakerPreviewContainer preview(THttpRequest&);
    DatasetMakerResultContainer make(THttpRequest&, const QVariantList&, const QString& = "");
    QVariantMap check(TSession&, const QString&);
    void previewAsync(THttpRequest&, TSession&);
    DatasetMakerPreviewContainer previewResult(TSession&);
    void makeAsync(THttpRequest&, TSession&);
    DatasetMakerResultContainer makeResult(TSession&);
    QString archivePath(const QString&);
    bool removeArchive(const QString&);
};

#endif // DATASETMAKERSERVICE_H
