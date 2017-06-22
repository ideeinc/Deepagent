#include "tagservice.h"
#include "managedfileservice.h"
#include <QtCore>
#include <TWebApplication>
#include <functional>


TagService::TagService()
    : _listDir(Tf::conf("settings").value("TagsDir").toString())
{
}

QList<TagGroup> TagService::allGroups() const
{
    QList<TagGroup> groups;

    if (_listDir.exists()) {
        for (const QString& name : _listDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot, QDir::Name)) {
            groups << TagGroup(name);
        }
    }

    return groups;
}

QList<Tag> TagService::allTags() const
{
    QList<Tag> tags;

    if (_listDir.exists()) {
        for (const TagGroup& group : allGroups()) {
            tags << group.tags();
        }
    }

    return tags;
}

QStringList TagService::imagesWithTag(const QString& tagName, const QString& inGroupName) const
{
    QStringList images;

    if (_listDir.exists(inGroupName)) {
        images = TagGroup(inGroupName).tag(tagName).images();
    }

    return images;
}

bool TagService::createGroup(const QString& groupName) const
{
    bool created = false;
    if (! groupName.isEmpty()) {
        created = _listDir.mkpath(groupName);
    }
    return created;
}

bool TagService::createTag(const QString& inGroupName, const QString& tagName, const QString& displayName) const
{
    bool created = false;
    if ((! inGroupName.isEmpty()) && (! tagName.isEmpty())) {
        created = QDir(_listDir.filePath(inGroupName)).mkpath(tagName);
        if (! displayName.isEmpty()) {
            TagGroup(inGroupName).tag(tagName).setDisplayName(displayName);
        }
    }
    return created;
}

bool TagService::destroyGroup(const QString &groupName) const
{
    return QDir(_listDir.filePath(groupName)).removeRecursively();
}

bool TagService::destroyTag(const QString& inGroupName, const QString& tagName) const
{
    return TagGroup(inGroupName).path(tagName).removeRecursively();
}

bool TagService::exists(const QString& tagName) const
{
    return _listDir.exists(tagName);
}

bool TagService::updateTag(const QString& inGroupName, const QString& srcName, const QVariantMap& changes) const
{
    bool succeed = false;

    if (exists(inGroupName)) {
        const QString dstName = changes.value("name").toString();
        succeed = ((srcName != dstName) ? QDir(_listDir.filePath(inGroupName)).rename(srcName, dstName) : true);
        TagGroup(inGroupName).tag(dstName).setDisplayName(changes.value("displayName").toString());

        const QString dstGroupName = changes.value("groupName").toString();
        if (inGroupName != dstGroupName) {
            succeed = TagGroup(inGroupName).tag(dstName).changeGroup(dstGroupName);
        }
    }

    return succeed;
}

void TagService::appendImages(const QString& groupName, const QString& tagName, const QStringList& imagePaths) const
{
    const TagGroup group = TagGroup(groupName);
    if ((! group.exists()) || (! group.hasTag(tagName))) {
        createTag(groupName, tagName, tagName);
    }

    const Tag tag = group.tag(tagName);
    for (const QString& path : imagePaths) {
        tag.appendImage(QFileInfo(path).isSymLink() ? QFile::symLinkTarget(path) : path);
    }
}

void TagService::removeImages(const QString& groupName, const QString& tagName, const QStringList& imageNames) const
{
    const TagGroup group = TagGroup(groupName);
    if ((! group.exists()) || (! group.hasTag(tagName))) {
        return;
    }

    const Tag tag = group.tag(tagName);
    for (const QString& name : imageNames) {
        tag.removeImage(QFileInfo(name).fileName());
    }
}

void TagService::updateImages(const QStringList& images, const QVariantMap& tags) const
{
    QStringList realPaths;
    std::transform(images.begin(), images.end(), std::back_inserter(realPaths), [](const QString& s){
        return (QFileInfo(s).isSymLink() ? QFile::symLinkTarget(s) : s);
    });

    for (const QString& specifiedGroup : tags.keys()) {
        const QString specifiedTag = tags.value(specifiedGroup).toString();
        if (! specifiedTag.isEmpty()) {
            appendImages(specifiedGroup, specifiedTag, realPaths);
        }
        for (const Tag& t : TagGroup(specifiedGroup).tags()) {
            if (t.name() != specifiedTag) {
                removeImages(specifiedGroup, t.name(), realPaths);
            }
        }
    }
}

QMap<QString, QStringList> TagService::extractImages(const QList<TMimeEntity>& files, const QString& groupName, const QString& tagName, const bool isCropImage)
{
    const auto results = ManagedFileService().append(files, isCropImage);
    const auto images = std::get<0>(results);

    if (images.count() > 0) {
        // タグを更新
        updateImages(images, {{groupName, tagName}});
    }

    QMap<QString, QStringList> errors;
    for (const auto& err : std::get<1>(results)) {
        errors[ err.typeName() ] << err.file();
    }
    return errors;
}

TagInfoContainer TagService::find(THttpRequest& request)
{
    TagInfoContainer container;

    QString filter;
    TagGroup targetGroup;
    Tag targetTag;
    long page = 0, limit = 200;

    if (request.method() == Tf::Get) {
        filter = request.queryItemValue("filter");
        if ((! filter.isEmpty()) && request.hasQueryItem("group")) {
            targetGroup = TagGroup(request.queryItemValue("group"));
            if (targetGroup.exists() && request.hasQueryItem("tag")) {
                targetTag = Tag(request.queryItemValue("tag"), &targetGroup);
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
                targetGroup = TagGroup(json["group"].toString());
                if (targetGroup.exists() && (! json["tag"].toString().isEmpty())) {
                    targetTag = Tag(json["tag"].toString(), &targetGroup);
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
                targetGroup = TagGroup(request.formItemValue("group"));
                if (targetGroup.exists() && request.hasFormItem("tag")) {
                    targetTag = Tag(request.formItemValue("tag"), &targetGroup);
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
            list = ManagedFileService::findInDirectory(filter, targetTag.path());
        }
        else if (targetGroup.exists()) {
            container.groupName = targetGroup.name();
            list = ManagedFileService::findInDirectory(filter, targetGroup.path().absolutePath());
        }
        else {
            list = ManagedFileService::find(filter);
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
                container.itemsPerPage = limit;
            }
            container.page = std::min(page, (container.images.count() / container.itemsPerPage));
            if (container.page < 0) container.page = 0;
            container.min = (container.page * container.itemsPerPage);
            container.max = std::min(static_cast<int>(container.min + container.itemsPerPage), container.images.count());
            container.maxNumberOfPage = (container.images.count() / container.itemsPerPage) + ((container.images.count() % container.itemsPerPage) > 0 ? 1 : 0);
        }
    }

    return container;
}

TagInfoContainer TagService::info(const QString& groupName, const QString& tagName, const long& page, const long& limit) const
{
    const TagGroup group(groupName);
    const Tag tag(tagName, &group);
    TagInfoContainer container;

    container.available = group.hasTag(tagName);
    container.name = tag.name();
    container.displayName = tag.displayName();
    container.groupName = group.name();
    container.images = tag.images();
    qSort(container.images.begin(), container.images.end(), [](const QString& p1, const QString& p2) {
        return ManagedFile::fromLink(p1).name() < ManagedFile::fromLink(p2).name();
    });

    if ((0 < limit) && (limit < 1000)) {
        container.itemsPerPage = limit;
    }
    container.page = std::min(page, (container.images.count() / container.itemsPerPage));
    if (container.page < 0) container.page = 0;
    container.min = (container.page * container.itemsPerPage);
    container.max = std::min(static_cast<int>(container.min + container.itemsPerPage), container.images.count());
    container.maxNumberOfPage = (container.images.count() / container.itemsPerPage) + ((container.images.count() % container.itemsPerPage) > 0 ? 1 : 0);

    return container;
}

TaggedImageInfoContainer TagService::image(const QString& groupName, const QString& primaryTag, const QStringList& images, const long& index) const
{
    TaggedImageInfoContainer container;

    if (images.isEmpty() || (index < 0) || (index >= images.count())) {
        return container;
    }

    container.path = images[index];
    container.displayName = ManagedFile::fromLink(container.path).name();
    container.index = index;
    container.count = images.count();

    const TagGroup primaryGroup(groupName);
    container.primaryGroup = primaryGroup;
    container.primaryTag = primaryGroup.tag(primaryTag);

    const QString filename = QFileInfo(container.path).fileName();
    for (const TagGroup& g : allGroups()) {
        for (const Tag& t : g.tags()) {
            if (t.hasImage(filename)) {
                container.containedGroups << t.groupName();
                container.containedTags << t;
            }
        }
    }

    return container;
}

TagTableContainer TagService::table(const QString& rowGroupName, const QString& colGroupName) const
{
    TagTableContainer container;
    container.rowGroupName = rowGroupName;
    container.colGroupName = colGroupName;

    if (rowGroupName.isEmpty() || colGroupName.isEmpty()) {
        return container;
    }

    const TagGroup colGroup(colGroupName);
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

    const TagGroup rowGroup(rowGroupName);
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

QPair<QStringList, TaggedImageInfoContainer> TagService::showTableImage(const QString& rowGroupName, const QString& rowTagName, const QString& colGroupName, const QString colTagName) const
{
    QPair<QStringList, TaggedImageInfoContainer> data;
    TaggedImageInfoContainer container;

    const TagGroup rowGroup(rowGroupName);
    container.primaryGroup = rowGroup;
    container.primaryTag = rowGroup.tag(rowTagName);

    const TagGroup colGroup(colGroupName);
    const Tag colTag(colTagName, &colGroup);

    if (rowGroup.exists() && colGroup.exists()) {
        QStringList images, primaryFolder = container.primaryTag.images();

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
                images = container.primaryTag.images();
            }
        }

        data.first = images;
        container.path = images[0];
        container.index = 0;
        container.count = images.count();

        const QString filename = QFileInfo(container.path).fileName();
        for (const TagGroup& g : allGroups()) {
            for (const Tag& t : g.tags()) {
                if (t.hasImage(filename)) {
                    container.containedGroups << t.groupName();
                    container.containedTags << t;
                }
            }
        }
    }

    data.second = container;
    return data;
}
