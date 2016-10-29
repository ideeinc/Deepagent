#include "caffecontextcontroller.h"
#include "caffecontext.h"


CaffeContextController::CaffeContextController(const CaffeContextController &)
    : ApplicationController()
{ }

void CaffeContextController::index()
{
    QList<CaffeContext> caffeContextList = CaffeContext::getAll();
    texport(caffeContextList);
    render();
}

void CaffeContextController::show(const QString &pk)
{
    auto caffeContext = CaffeContext::get(pk.toInt());
    texport(caffeContext);
    render();
}

void CaffeContextController::entry()
{
    renderEntry();
}

void CaffeContextController::create()
{
    if (httpRequest().method() != Tf::Post) {
        return;
    }

    auto form = httpRequest().formItems("caffeContext");
    auto caffeContext = CaffeContext::create(form);
    if (!caffeContext.isNull()) {
        QString notice = "Created successfully.";
        tflash(notice);
        redirect(urla("show", caffeContext.id()));
    } else {
        QString error = "Failed to create.";
        texport(error);
        renderEntry(form);
    }
}

void CaffeContextController::renderEntry(const QVariantMap &caffeContext)
{
    texport(caffeContext);
    render("entry");
}

void CaffeContextController::edit(const QString &pk)
{
    auto caffeContext = CaffeContext::get(pk.toInt());
    if (!caffeContext.isNull()) {
        session().insert("caffeContext_lockRevision", caffeContext.lockRevision());
        renderEdit(caffeContext.toVariantMap());
    } else {
        redirect(urla("entry"));
    }
}

void CaffeContextController::save(const QString &pk)
{
    if (httpRequest().method() != Tf::Post) {
        return;
    }

    QString error;
    int rev = session().value("caffeContext_lockRevision").toInt();
    auto caffeContext = CaffeContext::get(pk.toInt(), rev);
    if (caffeContext.isNull()) {
        error = "Original data not found. It may have been updated/removed by another transaction.";
        tflash(error);
        redirect(urla("edit", pk));
        return;
    }

    auto form = httpRequest().formItems("caffeContext");
    caffeContext.setProperties(form);
    if (caffeContext.save()) {
        QString notice = "Updated successfully.";
        tflash(notice);
        redirect(urla("show", pk));
    } else {
        error = "Failed to update.";
        texport(error);
        renderEdit(form);
    }
}

void CaffeContextController::renderEdit(const QVariantMap &caffeContext)
{
    texport(caffeContext);
    render("edit");
}

void CaffeContextController::remove(const QString &pk)
{
    if (httpRequest().method() != Tf::Post) {
        return;
    }

    auto caffeContext = CaffeContext::get(pk.toInt());
    caffeContext.remove();
    redirect(urla("index"));
}


// Don't remove below this line
T_REGISTER_CONTROLLER(caffecontextcontroller)
