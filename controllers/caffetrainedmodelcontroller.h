#ifndef CAFFETRAINEDMODELCONTROLLER_H
#define CAFFETRAINEDMODELCONTROLLER_H

#include "applicationcontroller.h"
#include "services/caffetrainedmodelservice.h"


class T_CONTROLLER_EXPORT CaffeTrainedModelController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    CaffeTrainedModelController();

public slots:
    void index();
    void show(const QString &id);
    void create();
    void save(const QString &id);
    void remove(const QString &id);
    void uploadTrainedModel();
    void predict(const QString &id);

private:
    CaffeTrainedModelService service {};
};

#endif // CAFFETRAINEDMODELCONTROLLER_H
