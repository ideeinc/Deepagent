#include "tagcontroller.h"
#include "containers/uploadresultcontainer.h"
#include <QtCore>

const QString kTagImageListKey = "Tag_ImageListKey";
const QString kTagImageFindKey = "Tag_ImageFindKey";
const QString kTagImageTableInfo = "Tag_ImageTableInfo";


void TagController::index()
{
    const QList<TagGroup> allGroups = service.allGroups();
    texport(allGroups);

    if (session().contains(kTagImageTableInfo)) {
        session().remove(kTagImageTableInfo);
    }

    render();
}

// show group
void TagController::show(const QString& groupName)
{
    TagGroup group(groupName);
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
    const long limit = (httpRequest().hasQueryItem("limit") ? httpRequest().parameter("limit").toLong() : 200);
    const TagInfoContainer& container = service.info(groupName, tagName, httpRequest().parameter("page").toInt(), limit);

    if (container.available) {
        setThumbnailImages(container.images);
        texport(container);

        const auto& allGroups = service.allGroups();
        texport(allGroups);

        const auto& baseUrl = urla("show", { groupName, tagName });
        texport(baseUrl);

        render();
    }
    else {
        redirect(urla("index"));
    }
}

// show image (from tag, or find)
void TagController::show(const QString& groupName, const QString& tagName, const QString& index)
{
    const QStringList images = thumbnailImages();
    const long i = index.toLong();
    if ( (! images.isEmpty()) && ((0 <= i) && (i < images.count())) ) {
        const TaggedImageInfoContainer container = service.image(groupName, tagName, images, i);
        texport(container);

        QUrl listUrl(urla("show", { container.primaryGroup.name(), container.primaryTag.name() }));
        QVariantMap imageTableInfo = session().value(kTagImageTableInfo).toMap();
        if (imageTableInfo.contains("rowGroupName") && imageTableInfo.contains("colGroupName")) {
            listUrl = urla("table");
        }
        const QVariantMap findInfo = session().value(kTagImageFindKey).toMap();
        if (findInfo.contains("filter")) {
            listUrl = urla("find", QStringList(), findInfo);
        }
        texport(listUrl);

        const QList<TagGroup> allGroups = service.allGroups();
        texport(allGroups);
        render("image");
    }
    else {
        redirect(urla("show", { groupName, tagName }));
    }
}

// show image (from table)
void TagController::show(const QString& rowGroupName, const QString& rowTagName, const QString& colGroupName, const QString colTagName)
{
    const QPair<QStringList, TaggedImageInfoContainer> data = service.showTableImage(rowGroupName, rowTagName, colGroupName, colTagName);

    if (data.first.size() > 0) {
        QVariantMap imageTableInfo;
        imageTableInfo["rowGroupName"] = rowGroupName;
        imageTableInfo["colGroupName"] = colGroupName;

        session().insert(kTagImageTableInfo, imageTableInfo);
        setThumbnailImages(data.first, kTagImageTableInfo);
        const TaggedImageInfoContainer& container = data.second;
        texport(container);

        QUrl listUrl(urla("table"));
        texport(listUrl);

        const auto& allGroups = service.allGroups();
        texport(allGroups);

        render("image");
    } else {
        render("table");
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

void TagController::table()
{
    QString rowGroupName, colGroupName;

    if (Tf::Get == httpRequest().method()) {
        if (session().contains(kTagImageTableInfo)) {
            QVariantMap imageTableInfo = session().value(kTagImageTableInfo).toMap();
            rowGroupName = imageTableInfo["rowGroupName"].toString();
            colGroupName = imageTableInfo["colGroupName"].toString();
            session().remove(kTagImageTableInfo);
        } else {
            render();
            return;
        }
    } else if (Tf::Post == httpRequest().method()) {
        rowGroupName = httpRequest().formItemValue("rowGroupName");
        colGroupName = httpRequest().formItemValue("colGroupName");
    }

    const TagTableContainer& container = service.table(rowGroupName, colGroupName);
    texport(container);
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
                QStringList data = thumbnailImages();
                for (const QString& s : images) {
                    const auto i = std::find_if(data.begin(), data.end(), [=](const QString& path) {
                        return (QFileInfo(path).fileName() == QFileInfo(s).fileName());
                    });
                    if (i != data.end()) {
                        data.erase(i);
                    }
                }
                updateThumbnailImages(data);
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
            QStringList data = thumbnailImages();
            for (const QString& s : images) {
                const auto i = std::find_if(data.begin(), data.end(), [=](const QString& path) {
                    return (QFileInfo(path).fileName() == QFileInfo(s).fileName());
                });
                if (i != data.end()) {
                    data.erase(i);
                }
            }
            updateThumbnailImages(data);
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

void TagController::find()
{
    const QList<TagGroup> allGroups = service.allGroups();
    texport(allGroups);

    const TagInfoContainer container = service.find(httpRequest());
    if (container.available) {
        const QVariantMap query{
            { "filter", container.filter },
            { "group", container.groupName },
            { "tag", container.name }
        };
        setThumbnailImages(container.images, kTagImageFindKey);
        session().insert(kTagImageFindKey, query);
        texport(container);

        QUrl baseUrl = urla("find", QStringList(), query);
        texport(baseUrl);
    }

    render();
}

void TagController::setThumbnailImages(const QStringList& images, const QString& forKey)
{
    const QStringList keys{ kTagImageTableInfo, kTagImageFindKey };
    for (const auto& k : keys) {
        if (forKey.isEmpty() || (k != forKey)) {
            session().remove(k);
        }
    }

    updateThumbnailImages(images);
}

void TagController::updateThumbnailImages(const QStringList& images)
{
    session().insert(kTagImageListKey, images);
}

QStringList TagController::thumbnailImages() const
{
    return session().value(kTagImageListKey).toStringList();
}

T_DEFINE_CONTROLLER(TagController)
