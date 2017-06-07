#ifndef DATASETMAKERSERVICE_H
#define DATASETMAKERSERVICE_H

#include "containers/datasetmakerindexcontainer.h"
#include "containers/datasetmakerpreviewcontainer.h"

class THttpRequest;
class TSession;


class DatasetMakerService
{
public:
    DatasetMakerIndexContainer index();
    DatasetMakerPreviewContainer preview(THttpRequest&);
};

#endif // DATASETMAKERSERVICE_H
