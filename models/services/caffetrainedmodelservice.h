#ifndef CAFFETRAINEDMODELSERVICE_H
#define CAFFETRAINEDMODELSERVICE_H

#include "containers/caffetrainedmodelpredictcontainer.h"

class THttpRequest;


class CaffeTrainedModelService
{
public:
    CaffeTrainedModelService() {}

    int create(THttpRequest &request);
    void uploadTrainedModel(THttpRequest &request);
    CaffeTrainedModelPredictContainer predict(int modelId, THttpRequest &request);
};

#endif // CAFFETRAINEDMODELSERVICE_H
