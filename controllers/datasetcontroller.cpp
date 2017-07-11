#include "datasetcontroller.h"
#include "dataset.h"


void DatasetController::index()
{
    auto datasetList = Dataset::getAll();
    texport(datasetList);
    render();
}

void DatasetController::show(const QString &id)
{
    auto dataset = Dataset::get(id);
    texport(dataset);
    render();
}

void DatasetController::create()
{
    switch (httpRequest().method()) {
    case Tf::Get:
        render();
        break;

    case Tf::Post: {
        auto dataset = httpRequest().formItems("dataset");
        auto model = Dataset::create(dataset);

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

void DatasetController::save(const QString &id)
{
    switch (httpRequest().method()) {
    case Tf::Get: {
        auto model = Dataset::get(id);
        if (!model.isNull()) {
            auto dataset = model.toVariantMap();
            texport(dataset);
            render();
        }
        break; }

    case Tf::Post: {
        QString error;
        auto model = Dataset::get(id);

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

void DatasetController::remove(const QString &id)
{
    if (httpRequest().method() != Tf::Post) {
        renderErrorResponse(Tf::NotFound);
        return;
    }

    auto dataset = Dataset::get(id);
    dataset.remove();
    redirect(urla("index"));
}

void DatasetController::sendText(const QString &id, const QString &fileName)
{
     auto dataset = Dataset::get(id);
    if (dataset.isNull()) {
        renderErrorResponse(Tf::NotFound);
    } else {
        QString filePath = QUrl(".").resolved(QUrl(fileName)).toString();
        sendFile(dataset.dirPath() + filePath, "text/plain");
    }
}

// Don't remove below this line
T_DEFINE_CONTROLLER(DatasetController)
