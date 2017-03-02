#include "caffetrainedmodelcontroller.h"
#include "caffetrainedmodel.h"


CaffeTrainedModelController::CaffeTrainedModelController(const CaffeTrainedModelController &)
    : ApplicationController()
{ }

void CaffeTrainedModelController::index()
{
    auto caffeTrainedModelList = CaffeTrainedModel::getAll();
    texport(caffeTrainedModelList);
    render();
}

void CaffeTrainedModelController::show(const QString &id)
{
    auto caffeTrainedModel = CaffeTrainedModel::get(id.toInt());
    texport(caffeTrainedModel);
    render();
}

void CaffeTrainedModelController::create()
{
    switch (httpRequest().method()) {
    case Tf::Get:
        render();
        break;

    case Tf::Post: {
        int id = service.create(httpRequest());
        if (id > 0) {
            QString notice = "Created successfully.";
            tflash(notice);
            redirect(urla("show", id));
        } else {
            QString error = "Failed to create.";
            texport(error);
            auto caffeTrainedModel = httpRequest().formItems("caffeTrainedModel");
            texport(caffeTrainedModel);
            render();
        }

        // auto caffeTrainedModel = httpRequest().formItems("caffeTrainedModel");
        // auto model = CaffeTrainedModel::create(caffeTrainedModel);

        // if (!model.isNull()) {
        //     QString notice = "Created successfully.";
        //     tflash(notice);
        //     redirect(urla("show", model.id()));
        // } else {
        //     QString error = "Failed to create.";
        //     texport(error);
        //     texport(caffeTrainedModel);
        //     render();
        // }
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void CaffeTrainedModelController::save(const QString &id)
{
    switch (httpRequest().method()) {
    case Tf::Get: {
        auto model = CaffeTrainedModel::get(id.toInt());
        if (!model.isNull()) {
            session().insert("caffeTrainedModel_lockRevision", model.lockRevision());
            auto caffeTrainedModel = model.toVariantMap();
            texport(caffeTrainedModel);
            render();
        }
        break; }

    case Tf::Post: {
        QString error;
        int rev = session().value("caffeTrainedModel_lockRevision").toInt();
        auto model = CaffeTrainedModel::get(id.toInt(), rev);

        if (model.isNull()) {
            error = "Original data not found. It may have been updated/removed by another transaction.";
            tflash(error);
            redirect(urla("save", id));
            break;
        }

        auto caffeTrainedModel = httpRequest().formItems("caffeTrainedModel");
        model.setProperties(caffeTrainedModel);
        if (model.save()) {
            QString notice = "Updated successfully.";
            tflash(notice);
            redirect(urla("show", model.id()));
        } else {
            error = "Failed to update.";
            texport(error);
            texport(caffeTrainedModel);
            render();
        }
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void CaffeTrainedModelController::remove(const QString &id)
{
    if (httpRequest().method() != Tf::Post) {
        renderErrorResponse(Tf::NotFound);
        return;
    }

    auto caffeTrainedModel = CaffeTrainedModel::get(id.toInt());
    caffeTrainedModel.remove();
    redirect(urla("index"));
}


void CaffeTrainedModelController::predict(const QString &id)
{
    switch (httpRequest().method()) {
    case Tf::Get: {
        auto caffeTrainedModel = CaffeTrainedModel::get(id.toInt());
        texport(caffeTrainedModel);
        render();
        break; }

    case Tf::Post: {
        service.predict(id.toInt(), httpRequest());
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

// Don't remove below this line
T_REGISTER_CONTROLLER(caffetrainedmodelcontroller)
