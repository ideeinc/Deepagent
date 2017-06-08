#ifndef INDEXCONTROLLER_H
#define INDEXCONTROLLER_H

#include "applicationcontroller.h"


class T_CONTROLLER_EXPORT IndexController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    IndexController() : ApplicationController() {}

public slots:
    void index();
};

#endif // INDEXCONTROLLER_H
