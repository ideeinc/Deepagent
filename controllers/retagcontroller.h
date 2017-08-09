#ifndef RETAGCONTROLLER_H
#define RETAGCONTROLLER_H

#include "applicationcontroller.h"
#include "services/retagservice.h"


class T_CONTROLLER_EXPORT RetagController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    RetagController() : ApplicationController() { }

public slots:
    void search();
    void search(const QString &tag0);
    void search(const QString &tag0, const QString &tag1);
    void search(const QString &tag0, const QString &tag1, const QString &tag2);
    void sequential(const QString &index);
    void show(const QString &image);
    void save(const QString &image, int nextStep);

private:
    RetagService service;
};

#endif // RETAGCONTROLLER_H
