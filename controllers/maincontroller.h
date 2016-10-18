#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "applicationcontroller.h"


class T_CONTROLLER_EXPORT MainController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    MainController() { }
    MainController(const MainController &other);

public slots:
    void index();
    void train();
    void train(const QString &no);
};

T_DECLARE_CONTROLLER(MainController, maincontroller)

#endif // MAINCONTROLLER_H
