#ifndef CAFFECONTEXTCONTROLLER_H
#define CAFFECONTEXTCONTROLLER_H

#include "applicationcontroller.h"


class T_CONTROLLER_EXPORT CaffeContextController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    CaffeContextController();

public slots:
    void index();
    void show(const QString &pk);
    void entry();
    void create();
    void edit(const QString &pk);
    void save(const QString &pk);
    void remove(const QString &pk);

private:
    void renderEntry(const QVariantMap &caffeContext = QVariantMap());
    void renderEdit(const QVariantMap &caffeContext = QVariantMap());
};

#endif // CAFFECONTEXTCONTROLLER_H
