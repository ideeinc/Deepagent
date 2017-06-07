#include "workspacecontroller.h"
#include "workspace.h"


WorkspaceController::WorkspaceController()
    : ApplicationController()
{ }

void WorkspaceController::index()
{
    QList<Workspace> workspaceList = Workspace::getAll();
    texport(workspaceList);
    render();
}

void WorkspaceController::show(const QString &pk)
{
    auto workspace = Workspace::get(pk.toInt());
    texport(workspace);
    render();
}

void WorkspaceController::entry()
{
    renderEntry();
}

void WorkspaceController::create()
{
    if (httpRequest().method() != Tf::Post) {
        return;
    }

    auto form = httpRequest().formItems("workspace");
    auto workspace = Workspace::create(form);
    if (!workspace.isNull()) {
        QString notice = "Created successfully.";
        tflash(notice);
        redirect(urla("show", workspace.id()));
    } else {
        QString error = "Failed to create.";
        texport(error);
        renderEntry(form);
    }
}

void WorkspaceController::renderEntry(const QVariantMap &workspace)
{
    texport(workspace);
    render("entry");
}

void WorkspaceController::edit(const QString &pk)
{
    auto workspace = Workspace::get(pk.toInt());
    if (!workspace.isNull()) {
        session().insert("workspace_lockRevision", workspace.lockRevision());
        renderEdit(workspace.toVariantMap());
    } else {
        redirect(urla("entry"));
    }
}

void WorkspaceController::save(const QString &pk)
{
    if (httpRequest().method() != Tf::Post) {
        return;
    }

    QString error;
    int rev = session().value("workspace_lockRevision").toInt();
    auto workspace = Workspace::get(pk.toInt(), rev);
    if (workspace.isNull()) {
        error = "Original data not found. It may have been updated/removed by another transaction.";
        tflash(error);
        redirect(urla("edit", pk));
        return;
    }

    auto form = httpRequest().formItems("workspace");
    workspace.setProperties(form);
    if (workspace.save()) {
        QString notice = "Updated successfully.";
        tflash(notice);
        redirect(urla("show", pk));
    } else {
        error = "Failed to update.";
        texport(error);
        renderEdit(form);
    }
}

void WorkspaceController::renderEdit(const QVariantMap &workspace)
{
    texport(workspace);
    render("edit");
}

void WorkspaceController::remove(const QString &pk)
{
    if (httpRequest().method() != Tf::Post) {
        return;
    }

    auto workspace = Workspace::get(pk.toInt());
    workspace.remove();
    redirect(urla("index"));
}


// Don't remove below this line
T_DEFINE_CONTROLLER(WorkspaceController)
