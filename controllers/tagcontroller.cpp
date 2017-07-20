#include "tagcontroller.h"
#include <QtCore>


void TagController::index()
{
    render();
}

// show group
void TagController::show(const QString& groupName)
{
    TagGroup group = TagRepository().findGroup(groupName);
    if (group.exists()) {
        texport(group);
        render("showGroup");
    }
    else {
        redirect(urla("index"));
    }
}

// show tag
void TagController::show(const QString& groupName, const QString& tagName)
{
    const TagInfoContainer& container = service.showTagInfo(httpRequest(), session(), groupName, tagName);
    if (container.available) {
        texport(container);
        render();
    }
    else {
        redirect(urla("index"));
    }
}

// show image (from tag, or find)
void TagController::show(const QString& groupName, const QString& tagName, const QString& index)
{
    const TaggedImageInfoContainer container = service.showTagImage(httpRequest(), session(), groupName, tagName, index.toLong());
    if (container.numberOfImages > 0) {
        texport(container);
        render("image");
    }
    else {
        redirect(urla("show", { groupName, tagName }));
    }
}

// show image (from table)
void TagController::show(const QString& rowGroupName, const QString& rowTagName, const QString& colGroupName, const QString colTagName)
{
    const TaggedImageInfoContainer container = service.showTableImage(httpRequest(), session(), rowGroupName, rowTagName, colGroupName, colTagName);
    if (container.numberOfImages > 0) {
        texport(container);
        render("image");
    } else {
        render("table");
    }
}


void TagController::upload()
{
    if (Tf::Post == httpRequest().method()) {
        UploadResultContainer uploadResult = service.uploadImages(httpRequest());

        if ((uploadResult.errors.count() < 1) && (! uploadResult.backPageURL.isEmpty())) {
            redirect(uploadResult.backPageURL);
        } else {
            texport(uploadResult);
            render("uploadResult");
        }
    }
    else {
        render();
    }
}

void TagController::table()
{
    const TagTableContainer& container = service.table(httpRequest(), session());
    texport(container);
    render();
}


void TagController::create()
{
    if (Tf::Post == httpRequest().method()) {
        service.create(httpRequest());

        const QUrl back = httpRequest().formItemValue("backToURL");
        if (! back.isEmpty()) {
            redirect(back);
        } else {
            redirect(urla("index"));
        }
    }
    else {
        renderErrorResponse(Tf::NotAcceptable);
    }
}


void TagController::destroy()
{
    if (Tf::Post == httpRequest().method()) {
        if (httpRequest().hasJson()) {
            service.destroy(httpRequest().jsonData().object());
            renderJson(QVariantMap());
        }
        else {
            service.destroy(httpRequest());
            redirect(urla("index"));
        }
    }
    else {
        renderErrorResponse(Tf::MethodNotAllowed);
    }
}

void TagController::append()
{
    QVariantMap response;

    if (! service.append(httpRequest(), session())) {
        response["message"] = "error";
    }

    renderJson(response);
}


void TagController::remove()
{
    QVariantMap response;

    if (Tf::Post == httpRequest().method()) {
        service.remove(httpRequest(), session());
    }

    renderJson(response);
}


void TagController::updateGroup()
{
    if (Tf::Post == httpRequest().method()) {
        if (httpRequest().hasJson()) {
            QVariantMap response;
            if (! service.updateGroup(httpRequest())) {
                response["error"] = "Rename failed";
            }
            renderJson(response);
        }
        else {
            service.updateGroup(httpRequest());
            redirect(urla("index"));
        }
    }
    else {
        renderErrorResponse(Tf::MethodNotAllowed);
    }
}

void TagController::update()
{
    if (Tf::Post == httpRequest().method()) {
        if (httpRequest().hasJson()) {
            const bool success = service.update(httpRequest());

            QVariantMap response{
                {"result", (success ? "success" : "failure")}
            };
            renderJson(response);
        }
        else {
            service.update(httpRequest());
            redirect(urla("index"));
        }
    }
    else {
        renderErrorResponse(Tf::MethodNotAllowed);
    }
}

void TagController::batchUpdate()
{
    if (Tf::Post == httpRequest().method()) {
        if (httpRequest().hasJson()) {
            service.batchUpdate(httpRequest());
            renderJson(QVariantMap());
        }
        else {
            renderErrorResponse(Tf::NotImplemented);
        }
    }
    else {
        renderErrorResponse(Tf::MethodNotAllowed);
    }
}

void TagController::find()
{
    const TagInfoContainer container = service.find(httpRequest(), session());
    if (container.available) {
        texport(container);
    }

    render();
}

T_DEFINE_CONTROLLER(TagController)
