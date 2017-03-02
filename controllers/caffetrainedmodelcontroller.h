#ifndef CAFFETRAINEDMODELCONTROLLER_H
#define CAFFETRAINEDMODELCONTROLLER_H

#include "applicationcontroller.h"
#include "services/caffetrainedmodelservice.h"


class T_CONTROLLER_EXPORT CaffeTrainedModelController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    CaffeTrainedModelController() { }
    CaffeTrainedModelController(const CaffeTrainedModelController &other);

public slots:
    void index();
    void show(const QString &id);
    void create();
    void save(const QString &id);
    void remove(const QString &id);

    void predict(const QString &id);

private:
    CaffeTrainedModelService service {};
};

T_DECLARE_CONTROLLER(CaffeTrainedModelController, caffetrainedmodelcontroller)

#endif // CAFFETRAINEDMODELCONTROLLER_H
