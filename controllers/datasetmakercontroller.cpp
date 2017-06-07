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
        DatasetMakerPreviewContainer container = service.preview(httpRequest());
        texport(container);
        render();
    }
    else {
        redirect(urla("index"));
    }
}

T_DEFINE_CONTROLLER(DatasetMakerController)
