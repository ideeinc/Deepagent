#ifndef WORKSPACECONTROLLER_H
#define WORKSPACECONTROLLER_H

#include "applicationcontroller.h"


class T_CONTROLLER_EXPORT WorkspaceController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    WorkspaceController() { }
    WorkspaceController(const WorkspaceController &other);

public slots:
    void index();
    void show(const QString &pk);
    void entry();
    void create();
    void edit(const QString &pk);
    void save(const QString &pk);
    void remove(const QString &pk);

private:
    void renderEntry(const QVariantMap &workspace = QVariantMap());
    void renderEdit(const QVariantMap &workspace = QVariantMap());
};

T_DECLARE_CONTROLLER(WorkspaceController, workspacecontroller)

#endif // WORKSPACECONTROLLER_H
