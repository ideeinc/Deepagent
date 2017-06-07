#ifndef DATASETCONTROLLER_H
#define DATASETCONTROLLER_H

#include "applicationcontroller.h"
#include "services/datasetmakerservice.h"


class T_CONTROLLER_EXPORT DatasetMakerController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    DatasetMakerController() {};
    DatasetMakerController(const DatasetMakerController&);

public slots:
    void index();
    void preview();

private:
    DatasetMakerService service {};
};

#endif // DATASETCONTROLLER_H
