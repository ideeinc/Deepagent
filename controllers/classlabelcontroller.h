#ifndef CLASSLABELCONTROLLER_H
#define CLASSLABELCONTROLLER_H

#include "applicationcontroller.h"


class T_CONTROLLER_EXPORT ClassLabelController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    ClassLabelController();

public slots:
    void index();
    void show(const QString &id);
    void create();
    void save(const QString &id);
    void remove(const QString &id);
};

#endif // CLASSLABELCONTROLLER_H
