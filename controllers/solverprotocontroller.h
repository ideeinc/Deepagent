#ifndef SOLVERPROTOCONTROLLER_H
#define SOLVERPROTOCONTROLLER_H

#include "applicationcontroller.h"


class T_CONTROLLER_EXPORT SolverProtoController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    SolverProtoController() : ApplicationController() {}

public slots:
    void index();
    void show(const QString &pk);
    void entry();
    void create();
    void edit(const QString &pk);
    void save(const QString &pk);
    void remove(const QString &pk);

private:
    void renderEntry(const QVariantMap &solverProto = QVariantMap());
    void renderEdit(const QVariantMap &solverProto = QVariantMap());
};

#endif // SOLVERPROTOCONTROLLER_H
