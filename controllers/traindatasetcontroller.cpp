#include "traindatasetcontroller.h"
#include "traindataset.h"


TrainDatasetController::TrainDatasetController()
    : ApplicationController()
{ }

void TrainDatasetController::index()
{
    auto datasetList = TrainDataset::getAll();
    texport(datasetList);
    render();
}

void TrainDatasetController::show(const QString &id)
{
    auto dataset = TrainDataset::get(id.toInt());
    texport(dataset);
    render();
}

void TrainDatasetController::create()
{
    switch (httpRequest().method()) {
    case Tf::Get:
        render();
        break;

    case Tf::Post: {
        auto dataset = httpRequest().formItems("dataset");
        auto model = TrainDataset::create(dataset);

        if (!model.isNull()) {
            QString notice = "Created successfully.";
            tflash(notice);
            redirect(urla("show", model.id()));
        } else {
            QString error = "Failed to create.";
            texport(error);
            texport(dataset);
            render();
        }
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void TrainDatasetController::save(const QString &id)
{
    switch (httpRequest().method()) {
    case Tf::Get: {
        auto model = TrainDataset::get(id.toInt());
        if (!model.isNull()) {
            session().insert("dataset_lockRevision", model.lockRevision());
            auto dataset = model.toVariantMap();
            texport(dataset);
            render();
        }
        break; }

    case Tf::Post: {
        QString error;
        int rev = session().value("dataset_lockRevision").toInt();
        auto model = TrainDataset::get(id.toInt(), rev);

        if (model.isNull()) {
            error = "Original data not found. It may have been updated/removed by another transaction.";
            tflash(error);
            redirect(urla("save", id));
            break;
        }

        auto dataset = httpRequest().formItems("dataset");
        model.setProperties(dataset);
        if (model.save()) {
            QString notice = "Updated successfully.";
            tflash(notice);
            redirect(urla("show", model.id()));
        } else {
            error = "Failed to update.";
            texport(error);
            texport(dataset);
            render();
        }
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void TrainDatasetController::remove(const QString &id)
{
    if (httpRequest().method() != Tf::Post) {
        renderErrorResponse(Tf::NotFound);
        return;
    }

    auto dataset = TrainDataset::get(id.toInt());
    dataset.remove();
    redirect(urla("index"));
}

// Don't remove below this line
T_DEFINE_CONTROLLER(TrainDatasetController)
