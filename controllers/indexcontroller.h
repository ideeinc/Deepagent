#ifndef INDEXCONTROLLER_H
#define INDEXCONTROLLER_H

#include "applicationcontroller.h"


class T_CONTROLLER_EXPORT IndexController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    IndexController() { }
    IndexController(const IndexController &other);

public slots:
    void index();
};

T_DECLARE_CONTROLLER(IndexController, indexcontroller)

#endif // INDEXCONTROLLER_H
