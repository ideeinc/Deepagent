#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "applicationcontroller.h"


class T_CONTROLLER_EXPORT MainController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    MainController();

public slots:
    void index();
    void train(const QString &no);
};

#endif // MAINCONTROLLER_H
