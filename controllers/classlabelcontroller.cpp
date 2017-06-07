#include "classlabelcontroller.h"
#include "classlabel.h"


ClassLabelController::ClassLabelController()
    : ApplicationController()
{ }

void ClassLabelController::index()
{
    auto classLabelList = ClassLabel::getAll();
    texport(classLabelList);
    render();
}

void ClassLabelController::show(const QString &id)
{
    auto classLabel = ClassLabel::get(id.toInt());
    texport(classLabel);
    render();
}

void ClassLabelController::create()
{
    switch (httpRequest().method()) {
    case Tf::Get:
        render();
        break;

    case Tf::Post: {
        auto classLabel = httpRequest().formItems("classLabel");
        auto model = ClassLabel::create(classLabel);

        if (!model.isNull()) {
            QString notice = "Created successfully.";
            tflash(notice);
            redirect(urla("show", model.id()));
        } else {
            QString error = "Failed to create.";
            texport(error);
            texport(classLabel);
            render();
        }
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void ClassLabelController::save(const QString &id)
{
    switch (httpRequest().method()) {
    case Tf::Get: {
        auto model = ClassLabel::get(id.toInt());
        if (!model.isNull()) {
            session().insert("classLabel_lockRevision", model.lockRevision());
            auto classLabel = model.toVariantMap();
            texport(classLabel);
            render();
        }
        break; }

    case Tf::Post: {
        QString error;
        int rev = session().value("classLabel_lockRevision").toInt();
        auto model = ClassLabel::get(id.toInt(), rev);

        if (model.isNull()) {
            error = "Original data not found. It may have been updated/removed by another transaction.";
            tflash(error);
            redirect(urla("save", id));
            break;
        }

        auto classLabel = httpRequest().formItems("classLabel");
        model.setProperties(classLabel);
        if (model.save()) {
            QString notice = "Updated successfully.";
            tflash(notice);
            redirect(urla("show", model.id()));
        } else {
            error = "Failed to update.";
            texport(error);
            texport(classLabel);
            render();
        }
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void ClassLabelController::remove(const QString &id)
{
    if (httpRequest().method() != Tf::Post) {
        renderErrorResponse(Tf::NotFound);
        return;
    }

    auto classLabel = ClassLabel::get(id.toInt());
    classLabel.remove();
    redirect(urla("index"));
}


// Don't remove below this line
T_DEFINE_CONTROLLER(ClassLabelController)
