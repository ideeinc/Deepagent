#ifndef DATASETCONTROLLER_H
#define DATASETCONTROLLER_H

#include "applicationcontroller.h"


class T_CONTROLLER_EXPORT DatasetController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    DatasetController() : ApplicationController() {}

public slots:
    void index();
    void show(const QString &id);
    void create();
    void save(const QString &id);
    void remove(const QString &id);

    void sendText(const QString &id, const QString &fileName);
};

#endif // DATASETCONTROLLER_H
