#include "tagservice.h"
#include "managedfile.h"
#include "logics/managedfilecontext.h"
#include <QtCore>
#include <functional>

const QString kTagImageListKey = "Tag_ImageListKey";
const QString kTagImageTableInfo = "Tag_ImageTableInfo";
const QString kTagImageFindKey = "Tag_ImageFindKey";


TagService::TagService()
{
}

void
TagService::create(const THttpRequest& request)
{
    if (Tf::Post == request.method()) {
        const QString target = request.formItemValue("target");
        if (target == "group") {
            _repository.createGroup(request.formItemValue("groupName"));
        }
        if (target == "tag") {
            _repository.createTag(request.formItemValue("parentGroup"), request.formItemValue("tagId"), request.formItemValue("tagDisplayName"));
        }
    }
}

void
TagService::destroy(const THttpRequest& request)
{
    if (Tf::Post == request.method()) {
        if (request.hasJson()) {
            destroy(request.jsonData().object());
        }
        else {
            const QString target = request.formItemValue("target");
            if (target == "group") {
                _repository.destroyGroup(request.formItemValue("name"));
            }
            if (target == "tag") {
                _repository.destroyTag(request.formItemValue("group"), request.formItemValue("name"));
            }
        }
    }
}

void
TagService::destroy(const QJsonObject& request)
{
    const QString target = request.value("target").toString();
    if (target == "group") {
        _repository.destroyGroup(request.value("name").toString());
    }
    if (target == "tag") {
        _repository.destroyTag(request.value("group").toString(), request.value("name").toString());
    }
}

bool
TagService::update(const THttpRequest& request)
{
    bool success = false;
    if (Tf::Post == request.method()) {
        if (request.hasJson()) {
            const QJsonObject info = request.jsonData().object();
            const QString inTargetGroupName = info.value("targetGroup").toString();
            const QString targetTagName = info.value("target").toString();
            const QVariantMap change = info.value("change").toObject().toVariantMap();
            success = _repository.updateTag(inTargetGroupName, targetTagName, change);
        }
        else {
            const QString inTargetGroupName = request.formItemValue("group");
            const QString targetTagName = request.formItemValue("original");
            const QVariantMap change({
                {"name", request.formItemValue("name")},
                {"displayName", request.formItemValue("displayName")},
                {"groupName", request.formItemValue("group")}
            });
            success = _repository.updateTag(inTargetGroupName, targetTagName, change);
        }
    }
    return success;
}

bool
TagService::updateGroup(const THttpRequest& request)
{
    bool succeed = false;
    if (Tf::Post == request.method()) {
        if (request.hasJson()) {
            const QJsonObject info = request.jsonData().object();
            const QString srcGroupName = info.value("sourceName").toString();
            const QString dstGroupName = info.value("destinationName").toString();
            TagGroup targetGroup = _repository.findGroup(srcGroupName);
            if (targetGroup.exists()) {
                succeed = targetGroup.setName(dstGroupName);
            }
        }
        else {
            const QString srcGroupName = request.formItemValue("sourceName");
            const QString dstGroupName = request.formItemValue("destinationName");
            TagGroup targetGroup = _repository.findGroup(srcGroupName);
            if (targetGroup.exists()) {
                succeed = targetGroup.setName(dstGroupName);
            }
        }
    }
    return succeed;
}

bool
TagService::append(const THttpRequest& request, TSession& session)
{
    bool succeed = false;

    if (Tf::Post == request.method()) {
        const QJsonObject info = request.jsonData().object();
        const bool refresh = info.value("refresh").toBool();
        const QString group = info.value("group").toString();
        const QString tag = info.value("tag").toString();
        const QStringList images = info.value("images").toVariant().toStringList();
        if (tag.isEmpty() || group.isEmpty() || (images.count() < 1)) {
            return succeed;
        }
        else {
            for (auto &img : images) {
                ManagedFile::updateTag(img, group, tag);
            }

            if (refresh) {
                QStringList images = session.value(kTagImageListKey).toStringList();
                for (const QString& s : images) {
                    const auto i = std::find_if(images.begin(), images.end(), [=](const QString& path) {
                        return (QFileInfo(path).fileName() == QFileInfo(s).fileName());
                    });
                    if (i != images.end()) {
                        images.erase(i);
                    }
                }
                session.insert(kTagImageListKey, images);
            }

            succeed = true;
        }
    }

    return succeed;
}

void
TagService::remove(const THttpRequest& request, TSession& session)
{
    if (Tf::Post == request.method()) {
        const QJsonObject info = request.jsonData().object();
        const bool refresh = info.value("refresh").toBool();
        const QString group = info.value("group").toString();
        const QString tag = info.value("tag").toString();
        const QStringList images = info.value("images").toVariant().toStringList();

        const auto rmtag = TagRepository().findTag(tag);
        for (auto &img : images) {
           auto mngf = ManagedFile::fromFileName(img);
           mngf.removeTag(rmtag);
        }

        if (refresh) {
            QStringList data = session.value(kTagImageListKey).toStringList();
            for (const QString& s : images) {
                const auto i = std::find_if(data.begin(), data.end(), [=](const QString& path) {
                    return (QFileInfo(path).fileName() == QFileInfo(s).fileName());
                });
                if (i != data.end()) {
                    data.erase(i);
                }
            }
            session.insert(kTagImageListKey, data);
        }
    }
}

void
TagService::batchUpdate(const THttpRequest& request)
{
    if (Tf::Post == request.method()) {
        if (request.hasJson()) {
            const QJsonObject node = request.jsonData().object();
            const QStringList images = node.value("images").toVariant().toStringList();
            const QVariantMap tags = node.value("tags").toObject().toVariantMap();

            //_repository.updateImages(images, tags);
            QList<Tag> addtags;
            QList<TagGroup> rmgroups;
            TagRepository repo;

            for (const QString& groupName : tags.keys()) {
                const QString tagName = tags.value(groupName).toString();
                if (tagName.isEmpty()) {
                    // 削除
                    rmgroups << repo.findGroup(groupName);
                } else {
                    // 追加タグ
                    addtags << repo.findTag(tagName);
                }
            }

            for (auto& img : images) {
                ManagedFile::fromFileName(img).updateTag(addtags, rmgroups);
            }
        }
    }
}

UploadResultContainer TagService::uploadImages(THttpRequest& request)
{
    UploadResultContainer uploadResult;

    if (Tf::Post == request.method()) {
        uploadResult.backPageURL = request.formItemValue("page");

        const QString groupName = request.formItemValue("group");
        QString tagName = request.formItemValue("tag");
        if (tagName.isEmpty()) {
            tagName = request.formItemValue("newTagName");
            if (! tagName.isEmpty()) {
                const QString displayName = request.formItemValue("newTagDisplayName");
                _repository.createTag(groupName, tagName, displayName);
            }
        }
        if (tagName.isEmpty() || groupName.isEmpty()) {
            return uploadResult;
        }

        const QList<TMimeEntity>& files = request.multipartFormData().entityList("files[]");
        const AppendingOption option {
            request.hasFormItem("trimmingMode") ? static_cast<TrimmingMode>(request.formItemValue("trimmingMode").toInt()) : TrimmingMode::Square,
            request.hasFormItem("duplicationMode") ? static_cast<DuplicationMode>(request.formItemValue("duplicationMode").toInt()) : DuplicationMode::ExcludeFiles
        };

        const auto results = ManagedFileContext().append(files, option);
        const auto images = std::get<0>(results);

        if (images.count() > 0) {
            // タグを更新: アップロードは新規ファイルのみなので「追加」のみ行う
            const auto tag = TagRepository().findTag(tagName);
            for (auto& img : images) {
                auto mngf = ManagedFile::fromFileName(img);
                mngf.addTag(tag, true);
            }
        }

        for (const auto& err : std::get<1>(results)) {
            uploadResult.errors[ err.typeName() ] << err.file();
        }

        uploadResult.completed = true;
    }

    return uploadResult;
}

TagInfoContainer TagService::find(const THttpRequest& request, TSession& session)
{
    TagInfoContainer container;

    QString filter;
    TagGroup targetGroup;
    Tag targetTag;
    int page = 0, limit = 200;

    if (request.method() == Tf::Get) {
        filter = request.queryItemValue("filter");
        if ((! filter.isEmpty()) && request.hasQueryItem("group")) {
            targetGroup = _repository.findGroup(request.queryItemValue("group"));
            if (targetGroup.exists() && request.hasQueryItem("tag")) {
                targetTag = targetGroup.findTag(request.queryItemValue("tag"));
            }
        }
        if (request.hasQueryItem("page")) {
            page = request.queryItemValue("page").toLong();
        }
        if (request.hasQueryItem("limit")) {
            limit = request.queryItemValue("limit").toLong();
        }
    }
    else if (request.method() == Tf::Post) {
        if (request.hasJson()) {
            const QJsonObject json = request.jsonData().object();
            filter = json["filter"].toString();
            if ((! filter.isEmpty()) && (! json["group"].toString().isEmpty())) {
                targetGroup = _repository.findGroup(json["group"].toString());
                if (targetGroup.exists() && (! json["tag"].toString().isEmpty())) {
                    targetTag = targetGroup.findTag(json["tag"].toString());
                }
            }
            if (! json["page"].toString().isEmpty()) {
                page = json["page"].toInt();
            }
            if (! json["limit"].toString().isEmpty()) {
                limit = json["limit"].toInt();
            }
        }
        else {
            filter = request.formItemValue("filter");
            if ((! filter.isEmpty()) && request.hasFormItem("group")) {
                targetGroup = _repository.findGroup(request.formItemValue("group"));
                if (targetGroup.exists() && request.hasFormItem("tag")) {
                    targetTag = targetGroup.findTag(request.formItemValue("tag"));
                }
            }
            if (request.hasFormItem("page")) {
                page = request.formItemValue("page").toLong();
            }
            if (request.hasFormItem("limit")) {
                limit = request.formItemValue("limit").toLong();
            }
        }
    }

    if (! filter.isEmpty()) {
        QList<ManagedFile> list;
        if (targetTag.exists()) {
            container.name = targetTag.name();
            container.displayName = targetTag.displayName();
            container.groupName = targetTag.groupName();
            list = ManagedFileContext::findInDirectory(filter, targetTag.path());
        }
        else if (targetGroup.exists()) {
            container.groupName = targetGroup.name();
            list = ManagedFileContext::findInDirectory(filter, targetGroup.path().absolutePath());
        }
        else {
            list = ManagedFileContext::find(filter);
        }

        for (const auto& file : list) {
            const QString p = file.path();
            tDebug() << "file: " << p;
            container.images << p;
        }

        if (container.images.count() > 0) {
            container.available = true;
            container.filter = filter;
            if ((0 < limit) && (limit < 1000)) {
                container.limit = limit;
            }
            container.page = std::min(page, (container.images.count() / container.limit));
            if (container.page < 0) container.page = 0;
            container.min = (container.page * container.limit);
            container.max = std::min((container.min + container.limit), container.images.count());
            container.maxNumberOfPage = (container.images.count() / container.limit) + ((container.images.count() % container.limit) > 0 ? 1 : 0);

            container.query = {
                { "filter", container.filter },
                { "group", container.groupName },
                { "tag", container.name }
            };

            session.remove(kTagImageTableInfo);
            session.insert(kTagImageListKey, container.images);
            session.insert(kTagImageFindKey, container.query);
        }
    }

    return container;
}

TagInfoContainer TagService::showTagInfo(const THttpRequest& request, TSession& session, const QString& groupName, const QString& tagName) const
{
    const TagGroup group = _repository.findGroup(groupName);
    const Tag tag = group.findTag(tagName);

    TagInfoContainer container;
    container.available = tag.exists();
    if (container.available) {
        container.arguments = QStringList{ groupName, tagName };
        container.name = tag.name();
        container.displayName = tag.displayName();
        container.groupName = group.name();
        container.images = tag.images();

        QCollator collator;
        collator.setNumericMode(true);
        qSort(container.images.begin(), container.images.end(), [&collator](const QString& p1, const QString& p2) {
            return (collator.compare(ManagedFile::fromLink(p1).name(), ManagedFile::fromLink(p2).name()) < 0);
        });

        const int limit = (request.hasQueryItem("limit") ? request.parameter("limit").toInt() : 200);
        if ((0 < limit) && (limit < 1000)) {
            container.limit = limit;
        }
        const int page = (request.hasQueryItem("page") ? request.parameter("page").toInt() : 0);
        container.page = std::min(page, (container.images.count() / container.limit));
        if (container.page < 0) container.page = 0;
        container.min = (container.page * container.limit);
        container.max = std::min((container.min + container.limit), container.images.count());
        container.maxNumberOfPage = (container.images.count() / container.limit) + ((container.images.count() % container.limit) > 0 ? 1 : 0);

        session.insert(kTagImageListKey, container.images);
    }

    return container;
}

TaggedImageInfoContainer TagService::showTagImage(const THttpRequest&, const TSession& session, const QString& groupName, const QString& tagName, const long& index) const
{
    TaggedImageInfoContainer container;

    const QStringList images = session.value(kTagImageListKey).toStringList();
    if (images.isEmpty() || (index < 0) || (index >= images.count())) {
        return container;
    }

    container.path = images[index];
    container.displayName = ManagedFile::fromLink(container.path).name();
    container.index = index;
    container.numberOfImages = images.count();

    const TagGroup primaryGroup = _repository.findGroup(groupName);
    if (primaryGroup.exists()) {
        container.primaryGroup = primaryGroup.name();
        const Tag primaryTag = primaryGroup.findTag(tagName);
        if (primaryTag.exists()) {
            container.primaryTag = primaryTag.name();
        }
    }

    container.containedTags = ManagedFile::fromFileName(container.path).getTags();

    container.listName = "show";
    container.listArgs = QStringList{ container.primaryGroup, container.primaryTag };

    QVariantMap imageTableInfo = session.value(kTagImageTableInfo).toMap();
    if (imageTableInfo.contains("rowGroupName") && imageTableInfo.contains("colGroupName")) {
        container.listName = "table";
        container.listArgs = QStringList();
    }

    const QVariantMap findInfo = session.value(kTagImageFindKey).toMap();
    if (findInfo.contains("filter")) {
        container.listName = "find";
        container.listArgs = QStringList();
        container.listQuery = findInfo;
    }

    return container;
}

TagTableContainer TagService::table(const THttpRequest& request, TSession& session) const
{
    TagTableContainer container;

    if (Tf::Get == request.method()) {
        if (session.contains(kTagImageTableInfo)) {
            QVariantMap imageTableInfo = session.value(kTagImageTableInfo).toMap();
            container.rowGroupName = imageTableInfo["rowGroupName"].toString();
            container.colGroupName = imageTableInfo["colGroupName"].toString();
            session.remove(kTagImageTableInfo);
        } else {
            return container;
        }
    } else if (Tf::Post == request.method()) {
        container.rowGroupName = request.formItemValue("rowGroupName");
        container.colGroupName = request.formItemValue("colGroupName");
    }

    if (container.rowGroupName.isEmpty() || container.colGroupName.isEmpty()) {
        return container;
    }

    const TagGroup colGroup = _repository.findGroup(container.colGroupName);
    QList<QSet<QString>> columns;

    // create data for table header
    container.headers << " 残数 ";
    container.colTagNames << "残数";
    for (const Tag& colTag : colGroup.tags()) {
        container.headers << colTag.displayName();
        container.colTagNames << colTag.name();
        columns << colTag.imageNames().toSet();
    }
    container.headers << " 総数 ";
    container.colTagNames << "総数";

    const TagGroup rowGroup = _repository.findGroup(container.rowGroupName);
    if (rowGroup.exists() && colGroup.exists()) {
        // create data for table body
        for (const Tag& rowTag : rowGroup.tags()) {
            QStringList rowContent;
            const QSet<QString> rowFolder = rowTag.imageNames().toSet();
            QSet<QString> taggedImageNamesAll, noTaggedImageNamesAll;

            container.rowTagNames << rowTag.name();
            rowContent << rowTag.displayName();
            for (const auto &colImageNames : columns) {
                const QSet<QString> taggedImageNames = (colImageNames & rowFolder);
                taggedImageNamesAll += taggedImageNames;
                rowContent << QString::number(taggedImageNames.count());
            }

            noTaggedImageNamesAll = (rowFolder - taggedImageNamesAll);
            rowContent.insert(1, QString::number(noTaggedImageNamesAll.count()));
            rowContent << QString::number(noTaggedImageNamesAll.count() + taggedImageNamesAll.count());
            container.rows << rowContent;
        }

        // calculate total amount of images for each column for the table footer
        for (long i = 0; i < container.headers.count(); i++) {
            long imagesTotalAmount = 0;
            for (long j = 0; j < container.rows.count(); j++) {
                imagesTotalAmount += container.rows[j][i + 1].toLong();
            }

            container.footers << QString::number(imagesTotalAmount);
        }
    }

    return container;
}

TaggedImageInfoContainer TagService::showTableImage(const THttpRequest&, TSession& session, const QString& rowGroupName, const QString& rowTagName, const QString& colGroupName, const QString colTagName) const
{
    TaggedImageInfoContainer container;

    const TagGroup rowGroup = _repository.findGroup(rowGroupName);
    const TagGroup colGroup = _repository.findGroup(colGroupName);

    if (rowGroup.exists() && colGroup.exists()) {
        const Tag rowTag = rowGroup.findTag(rowTagName);
        const Tag colTag = colGroup.findTag(colTagName);

        container.primaryGroup = rowGroup.name();
        container.primaryTag = rowTag.name();

        QStringList images, primaryFolder = rowTag.images();

        if (colTag.exists()) {
            QStringList taggedImages;
            for (const auto &path : primaryFolder) {
                if (colTag.hasImage(QFileInfo(path).fileName())) {
                    taggedImages << path;
                }
            }
            images = taggedImages;
        } else {
            if (colTagName == "残数") {
                QStringList taggedImagesAll;
                for (const auto& path : primaryFolder) {
                    for (const Tag& tag : colGroup.tags()) {
                        if (tag.hasImage(QFileInfo(path).fileName())) {
                            taggedImagesAll << path;
                        }
                    }
                }
                images = (primaryFolder.toSet() - taggedImagesAll.toSet()).toList();
            } else { // colTagName == "総数"
                images = rowTag.images();
            }
        }

        container.path = images[0];
        container.index = 0;
        container.numberOfImages = images.count();
        container.containedTags = ManagedFile::fromFileName(container.path).getTags();

        if (images.size()) {
            session.insert(kTagImageTableInfo, QVariantMap{
                { "rowGroupName", rowGroupName },
                { "colGroupName", colGroupName }
            });
            session.insert(kTagImageListKey, images);
        }

        container.listName = "table";
        container.listArgs = QStringList();
    }

    return container;
}
