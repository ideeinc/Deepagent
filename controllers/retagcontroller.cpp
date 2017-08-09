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
            auto container = service.search("", "", "", httpRequest(), session());
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

void RetagController::search(const QString &tag0)
{
    search(tag0, "", "");
}

void RetagController::search(const QString &tag0, const QString &tag1)
{
    search(tag0, tag1, "");
}

void RetagController::search(const QString &tag0, const QString &tag1, const QString &tag2)
{
    switch (httpRequest().method()) {
    case Tf::Get: {
        // serach, store session, show list
        auto container = service.search(tag0, tag1, tag2, httpRequest(), session());
        texport(container);
        render();
        break; }

    case Tf::Post:
    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void RetagController::sequential(const QString &index)
{
    switch (httpRequest().method()) {
    case Tf::Get: {
        auto container = service.sequential(index, httpRequest(), session());
        if (container.imagePath.isEmpty()) {
            redirect(QUrl(recentAccessPath()));
        } else {
            texport(container);
            render("show");
        }
        break; }

    case Tf::Post: {
        auto container = service.startSequential(recentAccessPath(), httpRequest(), session());
        if (container.imagePath.isEmpty()) {
            redirect(QUrl(recentAccessPath()));
        } else {
            texport(container);
            render("show");
        }
        break; }

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
