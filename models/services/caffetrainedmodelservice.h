#ifndef CAFFETRAINEDMODELSERVICE_H
#define CAFFETRAINEDMODELSERVICE_H

class THttpRequest;


class CaffeTrainedModelService
{
public:
    CaffeTrainedModelService() {}

    int create(THttpRequest &request);
    void predict(int modelId, THttpRequest &request);
};

#endif // CAFFETRAINEDMODELSERVICE_H
