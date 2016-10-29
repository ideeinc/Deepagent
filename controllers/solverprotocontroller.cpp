#include "solverprotocontroller.h"
#include "solverproto.h"


SolverProtoController::SolverProtoController(const SolverProtoController &)
    : ApplicationController()
{ }

void SolverProtoController::index()
{
    QList<SolverProto> solverProtoList = SolverProto::getAll();
    texport(solverProtoList);
    render();
}

void SolverProtoController::show(const QString &pk)
{
    auto solverProto = SolverProto::get(pk.toInt());
    texport(solverProto);
    render();
}

void SolverProtoController::entry()
{
    renderEntry();
}

void SolverProtoController::create()
{
    if (httpRequest().method() != Tf::Post) {
        return;
    }

    auto form = httpRequest().formItems("solverProto");
    auto solverProto = SolverProto::create(form);
    if (!solverProto.isNull()) {
        QString notice = "Created successfully.";
        tflash(notice);
        redirect(urla("show", solverProto.id()));
    } else {
        QString error = "Failed to create.";
        texport(error);
        renderEntry(form);
    }
}

void SolverProtoController::renderEntry(const QVariantMap &solverProto)
{
    texport(solverProto);
    render("entry");
}

void SolverProtoController::edit(const QString &pk)
{
    auto solverProto = SolverProto::get(pk.toInt());
    if (!solverProto.isNull()) {
        session().insert("solverProto_lockRevision", solverProto.lockRevision());
        renderEdit(solverProto.toVariantMap());
    } else {
        redirect(urla("entry"));
    }
}

void SolverProtoController::save(const QString &pk)
{
    if (httpRequest().method() != Tf::Post) {
        return;
    }

    QString error;
    int rev = session().value("solverProto_lockRevision").toInt();
    auto solverProto = SolverProto::get(pk.toInt(), rev);
    if (solverProto.isNull()) {
        error = "Original data not found. It may have been updated/removed by another transaction.";
        tflash(error);
        redirect(urla("edit", pk));
        return;
    }

    auto form = httpRequest().formItems("solverProto");
    solverProto.setProperties(form);
    if (solverProto.save()) {
        QString notice = "Updated successfully.";
        tflash(notice);
        redirect(urla("show", pk));
    } else {
        error = "Failed to update.";
        texport(error);
        renderEdit(form);
    }
}

void SolverProtoController::renderEdit(const QVariantMap &solverProto)
{
    texport(solverProto);
    render("edit");
}

void SolverProtoController::remove(const QString &pk)
{
    if (httpRequest().method() != Tf::Post) {
        return;
    }

    auto solverProto = SolverProto::get(pk.toInt());
    solverProto.remove();
    redirect(urla("index"));
}


// Don't remove below this line
T_REGISTER_CONTROLLER(solverprotocontroller)
