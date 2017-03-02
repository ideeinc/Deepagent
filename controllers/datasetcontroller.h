#ifndef DATASETCONTROLLER_H
#define DATASETCONTROLLER_H

#include "applicationcontroller.h"


class T_CONTROLLER_EXPORT DatasetController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    DatasetController() { }
    DatasetController(const DatasetController &other);

public slots:
    void index();
    void show(const QString &id);
    void create();
    void save(const QString &id);
    void remove(const QString &id);
};

T_DECLARE_CONTROLLER(DatasetController, datasetcontroller)

#endif // DATASETCONTROLLER_H
