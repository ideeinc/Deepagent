#include "retagcontroller.h"
#include "containers/retagsearchcontainer.h"
#include "containers/retagshowcontainer.h"


void RetagController::search()
{
    switch (httpRequest().method()) {
    case Tf::Get:
    case Tf::Post: {
        auto req = httpRequest();
        QStringList args = { req.queryItemValue("tag0"), req.queryItemValue("tag1"), req.queryItemValue("tag2") };

        if (args.filter(QRegExp("\\S+")).isEmpty()) {
            auto container = service.search("", "", "", session());
            texport(container);
            render();
        } else {
            auto url = urla("search", args);
            redirect(url);
        }
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void RetagController::search(const QString &tag1)
{
    search(tag1, "", "");
}

void RetagController::search(const QString &tag1, const QString &tag2)
{
    search(tag1, tag2, "");
}

void RetagController::search(const QString &tag1, const QString &tag2, const QString &tag3)
{
    switch (httpRequest().method()) {
    case Tf::Get: {
        // serach, store session, show list
        auto container = service.search(tag1, tag2, tag3, session());
        texport(container);
        render();
        break; }

    case Tf::Post:
    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void RetagController::show(const QString &image)
{
    switch (httpRequest().method()) {
    case Tf::Get: {
        // show a image
        auto container = service.show(image, session());
        texport(container);
        render();
        break; }

    case Tf::Post:
    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void RetagController::save(const QString &image, int nextStep)
{
    switch (httpRequest().method()) {
    case Tf::Get: {
        // show a image
        service.saveForm(image, session());
        break; }

    case Tf::Post: {
        // save
        // nextStop 1:next 0:keep -1:prev
        service.save(image, nextStep, session());
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

// Don't remove below this line
T_DEFINE_CONTROLLER(RetagController)
