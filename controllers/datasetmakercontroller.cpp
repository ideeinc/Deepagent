#include "datasetmakercontroller.h"
#include "services/tagservice.h"
#include <QtCore>


DatasetMakerController::DatasetMakerController(const DatasetMakerController&)
: ApplicationController()
{
}

void DatasetMakerController::index()
{
    const QList<TagGroup> allGroups = TagService().allGroups();
    texport(allGroups);

    const auto container = service.index();
    texport(container);

    render();
}

void DatasetMakerController::preview()
{
    if (Tf::Post == httpRequest().method()) {
        // DatasetMakerPreviewContainer container = service.preview(httpRequest());
        // texport(container);
        // render();
        service.previewAsync(httpRequest(), session());
        render("check");
    }
    else {
        redirect(urla("index"));
    }
}

void DatasetMakerController::make()
{
    if (Tf::Post == httpRequest().method()) {
        service.makeAsync(httpRequest(), session());
        render("check");
    }
    else {
        redirect(urla("index"));
    }
}

void DatasetMakerController::download(const QString& archiveName)
{
    const QString path = service.archivePath(archiveName);
    if (QFileInfo(path).exists()) {
        sendFile(path, "application/tar+gzip");
    }
    else {
        renderErrorResponse(Tf::NotFound);
    }
}

void DatasetMakerController::remove(const QString& archiveName)
{
    service.removeArchive(archiveName);
    redirect(urla("index"));
}

void DatasetMakerController::check(const QString& action)
{
    const QVariantMap response = service.check(session(), action);
    renderJson(response);
}

void DatasetMakerController::result(const QString& action)
{
    if (action == "preview") {
        DatasetMakerPreviewContainer container = service.previewResult(session());
        texport(container);
        render(action);
    }
    else if (action == "make") {
        DatasetMakerResultContainer container = service.makeResult(session());
        texport(container);
        render(action);
    }
    else {
        renderErrorResponse(404);
    }
}

T_DEFINE_CONTROLLER(DatasetMakerController)
