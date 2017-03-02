#ifndef CLASSLABELCONTROLLER_H
#define CLASSLABELCONTROLLER_H

#include "applicationcontroller.h"


class T_CONTROLLER_EXPORT ClassLabelController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    ClassLabelController() { }
    ClassLabelController(const ClassLabelController &other);

public slots:
    void index();
    void show(const QString &id);
    void create();
    void save(const QString &id);
    void remove(const QString &id);
};

T_DECLARE_CONTROLLER(ClassLabelController, classlabelcontroller)

#endif // CLASSLABELCONTROLLER_H
