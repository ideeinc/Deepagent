#ifndef TRAINCONTROLLER_H
#define TRAINCONTROLLER_H

#include "applicationcontroller.h"
#include "services/trainservice.h"


class T_CONTROLLER_EXPORT TrainController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    TrainController();

public slots:
    void index();
    void create();
    void createSsd();
    void classify(const QString &id);
    void detect(const QString &id);
    void show(const QString &id);
    void clone(const QString &id);
    void remove(const QString &id);
    //void getPrototxt(const QString &id, const QString &prototxt);
    void sendText(const QString &id, const QString &fileName);
    void uploadTrainedModel();

private:
    TrainService service;
};

#endif // TRAINCONTROLLER_H
