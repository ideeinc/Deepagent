#ifndef TRAINDATASETCONTROLLER_H
#define TRAINDATASETCONTROLLER_H

#include "applicationcontroller.h"


class T_CONTROLLER_EXPORT TrainDatasetController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    TrainDatasetController();

public slots:
    void index();
    void show(const QString &id);
    void create();
    void save(const QString &id);
    void remove(const QString &id);
};

#endif // TRAINDATASETCONTROLLER_H
