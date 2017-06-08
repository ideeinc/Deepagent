#include "tagcontroller.h"
#include "containers/uploadresultcontainer.h"
#include <QtCore>

const QString kTagImageListKey = "Tag_ImageListKey";


TagController::TagController(const TagController&)
    : ApplicationController()
{
}


void TagController::index()
{
    const QList<TagGroup> allGroups = service.allGroups();
    texport(allGroups);
    render();
}


void TagController::edit(const QString& groupName)
{
    TagGroup group(groupName);
    texport(group);
    render();
}

void TagController::show(const QString& groupName, const QString& tagName)
{
    const long limit = (httpRequest().hasQueryItem("limit") ? httpRequest().parameter("limit").toLong() : 200);
    const TagInfoContainer& container = service.info(groupName, tagName, httpRequest().parameter("page").toInt(), limit);

    if (container.available) {
        session().insert(kTagImageListKey, container.images);
        texport(container);

        const auto& allGroups = service.allGroups();
        texport(allGroups);

        render();
    }
    else {
        redirect(urla("index"));
    }
}


void TagController::show(const QString& groupName, const QString& tagName, const QString& index)
{
    const QStringList images = session().value(kTagImageListKey).toStringList();
    const long i = index.toLong();
    if ( (! images.isEmpty()) && ((0 <= i) && (i < images.count())) ) {
        const TaggedImageInfoContainer container = service.image(groupName, tagName, images, i);
        texport(container);

        const QList<TagGroup> allGroups = service.allGroups();
        texport(allGroups);
        render("image");
    }
    else {
        redirect(urla("show", tagName));
    }
}


void TagController::upload()
{
    const auto& allGroups = service.allGroups();
    texport(allGroups);

    if (Tf::Post == httpRequest().method()) {
        UploadResultContainer uploadResult;

        const QString groupName = httpRequest().formItemValue("group");
        QString tagName = httpRequest().formItemValue("tag");
        if (tagName.isEmpty()) {
            tagName = httpRequest().formItemValue("newTagName");
            if (! tagName.isEmpty()) {
                const QString displayName = httpRequest().formItemValue("newTagDisplayName");
                service.createTag(groupName, tagName, displayName);
            }
        }
        if ((! tagName.isEmpty()) && (! groupName.isEmpty())) {
            const bool cropImage = ((httpRequest().formItemValue("cropImage", "1")) == "1" ? true : false);
            uploadResult.errors = service.extractImages(httpRequest().multipartFormData().entityList("files[]"), groupName, tagName, cropImage);
        }
        uploadResult.completed = true;
        uploadResult.backPageURL = httpRequest().formItemValue("page");

        texport(uploadResult);
    }

    render();
}

void TagController::create()
{
    if (Tf::Post == httpRequest().method()) {
        const QString target = httpRequest().formItemValue("target");
        if (target == "group") {
            service.createGroup(httpRequest().formItemValue("groupName"));
        }
        if (target == "tag") {
            service.createTag(httpRequest().formItemValue("parentGroup"), httpRequest().formItemValue("tagId"), httpRequest().formItemValue("tagDisplayName"));
        }
        redirect(urla("index"));
    }
    else {
        renderErrorResponse(Tf::NotAcceptable);
    }
}


void TagController::destroy()
{
    if (Tf::Post == httpRequest().method()) {
        if (httpRequest().hasJson()) {
            QVariantMap response;
            const QJsonObject info = httpRequest().jsonData().object();
            const QString target = info.value("target").toString();
            if (target == "group") {
                service.destroyGroup(info.value("name").toString());
            }
            if (target == "tag") {
                service.destroyTag(info.value("group").toString(), info.value("name").toString());
            }
            renderJson(response);
        }
        else {
            const QString target = httpRequest().formItemValue("target");
            if (target == "group") {
                service.destroyGroup(httpRequest().formItemValue("name"));
            }
            if (target == "tag") {
                service.destroyTag(httpRequest().formItemValue("group"), httpRequest().formItemValue("name"));
            }
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

    if (Tf::Post == httpRequest().method()) {
        const QJsonObject info = httpRequest().jsonData().object();
        const bool refresh = info.value("refresh").toBool();
        const QString group = info.value("group").toString();
        const QString tag = info.value("tag").toString();
        const QStringList images = info.value("images").toVariant().toStringList();
        if (tag.isEmpty() || group.isEmpty() || (images.count() < 1)) {
            response["message"] = "error";
        }
        else {
            service.updateImages(images, {{group, tag}});

            if (refresh) {
                QStringList data = session().value(kTagImageListKey).toStringList();
                for (const QString& s : images) {
                    const auto i = std::find_if(data.begin(), data.end(), [=](const QString& path) {
                        return (QFileInfo(path).fileName() == QFileInfo(s).fileName());
                    });
                    if (i != data.end()) {
                        data.erase(i);
                    }
                }
                session().insert(kTagImageListKey, data);
            }
        }
    }

    renderJson(QJsonObject::fromVariantMap(response));
}


void TagController::remove()
{
    QVariantMap response;

    if (Tf::Post == httpRequest().method()) {
        const QJsonObject info = httpRequest().jsonData().object();
        const bool refresh = info.value("refresh").toBool();
        const QString group = info.value("group").toString();
        const QString tag = info.value("tag").toString();
        const QStringList images = info.value("images").toVariant().toStringList();
        service.removeImages(group, tag, images);

        if (refresh) {
            QStringList data = session().value(kTagImageListKey).toStringList();
            for (const QString& s : images) {
                const auto i = std::find_if(data.begin(), data.end(), [=](const QString& path) {
                    return (QFileInfo(path).fileName() == QFileInfo(s).fileName());
                });
                if (i != data.end()) {
                    data.erase(i);
                }
            }
            session().insert(kTagImageListKey, data);
        }
    }

    renderJson(QJsonObject::fromVariantMap(response));
}


void TagController::updateGroup()
{
    if (Tf::Post == httpRequest().method()) {
        if (httpRequest().hasJson()) {
            QVariantMap response;
            const QJsonObject info = httpRequest().jsonData().object();
            const QString srcGroupName = info.value("sourceName").toString();
            const QString dstGroupName = info.value("destinationName").toString();
            if (srcGroupName != dstGroupName) {
                if (! TagGroup(srcGroupName).setName(dstGroupName)) {
                    response["error"] = "Rename failed";
                }
            }
            renderJson(QJsonObject::fromVariantMap(response));
        }
        else {
            const QString srcGroupName = httpRequest().formItemValue("sourceName");
            const QString dstGroupName = httpRequest().formItemValue("destinationName");
            if (srcGroupName != dstGroupName) {
                TagGroup(srcGroupName).setName(dstGroupName);
            }
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
        bool success = false;

        if (httpRequest().hasJson()) {
            QVariantMap response;

            const QJsonObject info = httpRequest().jsonData().object();
            const QString inTargetGroupName = info.value("targetGroup").toString();
            const QString targetTagName = info.value("target").toString();
            const QVariantMap change = info.value("change").toObject().toVariantMap();
            success = service.updateTag(inTargetGroupName, targetTagName, change);
            if (success) {
            }

            response["result"] = success ? "success" : "failure";
            renderJson(QJsonObject::fromVariantMap(response));
        }
        else {
            const QString inTargetGroupName = httpRequest().formItemValue("group");
            const QString targetTagName = httpRequest().formItemValue("original");
            const QVariantMap change({
                {"name", httpRequest().formItemValue("name")},
                {"displayName", httpRequest().formItemValue("displayName")},
                {"groupName", httpRequest().formItemValue("group")}
            });
            service.updateTag(inTargetGroupName, targetTagName, change);
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
            const QJsonObject node = httpRequest().jsonData().object();
            const QStringList images = node.value("images").toVariant().toStringList();
            const QVariantMap tags = node.value("tags").toObject().toVariantMap();

            QVariantMap response;
            service.updateImages(images, tags);
            renderJson(response);
        }
        else {
            renderErrorResponse(Tf::NotImplemented);
        }
    }
    else {
        renderErrorResponse(Tf::MethodNotAllowed);
    }
}

T_REGISTER_CONTROLLER(tagcontroller)