#include "tagservice.h"
#include "image.h"
#include <QtCore>
#include <TWebApplication>
#include <THttpRequest>
#include <functional>

namespace {
    QByteArray checksum(const QString& path, QCryptographicHash::Algorithm algorithm = QCryptographicHash::Md5)
    {
        QFile file(path);
        if (file.open(QFile::ReadOnly)) {
            QCryptographicHash hash(algorithm);
            if (hash.addData(&file)) {
                return hash.result();
            }
        }
        return QByteArray();
    }

    void find(const QString& path, const QString& filename, const std::function<bool(const QString&)> action)
    {
        const QString needle = QFileInfo(filename).completeBaseName();
        QDirIterator i(path, {needle + ".*"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (i.hasNext()) {
            const QString name = i.next();
            const QFileInfo info(name);
            if (info.completeBaseName() == needle) {
                if (! action(info.absoluteFilePath())) {
                    break;
                }
            }
        }
    }
}


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

QMap<QString, QStringList> TagService::extractImages(const QList<TMimeEntity>& files, const QString& groupName, const QString& tagName, const bool cropImage)
{
    const auto baseDir = Tf::conf("settings").value("OriginalImagesDir").toString();
    const auto basePath = QDir(baseDir).filePath( QDir::toNativeSeparators(QDateTime::currentDateTime().toString("yyyy/MMdd/")) + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()) );
    if (! QDir(basePath).exists()) {
        QDir(basePath).mkpath(".");
    }

    const QTemporaryDir tempDir;
    const auto workPath = tempDir.path();
    for (const auto& f : files) {
        QProcess inflator;
        const QString originalName = f.originalFileName();
        const QString extension = QFileInfo(originalName).completeSuffix();
        const QString contentType = f.contentType();
        if ((extension == "zip") || (contentType == "application/zip") || (contentType == "application/x-zip-compressed")) {
            inflator.start("unzip", { f.uploadedFilePath(), "-d", workPath });
            inflator.waitForFinished();
        }
        else if ((extension == "tar") || (contentType == "application/x-tar")) {
            inflator.start("tar", { "xf", f.uploadedFilePath(), "-C", workPath });
            inflator.waitForFinished();
        }
        else if ((extension == "tar.gz") || (contentType == "application/x-gzip") || (contentType == "application/gzip")) {
            inflator.start("tar", { "xf", f.uploadedFilePath(), "-C", workPath });
            inflator.waitForFinished();
        }
        else if (contentType == "image/jpeg") {
            const_cast<TMimeEntity*>(&f)->renameUploadedFile(QDir(workPath).filePath(originalName));
        }
        else {
            tDebug() << "Not processed: [" << contentType << "] " << f.uploadedFilePath() << "/" << originalName;
        }
    }

    // cache the all hashes.
    QStringList caches;
    QDirIterator i(baseDir, {"*.jpg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (i.hasNext()) {
        const QString path = i.next();
        caches << QFileInfo(path).completeBaseName();
    }

    QMap<QString, QStringList> errors;
    QStringList images;
    QDirIterator search(workPath, {"*.jpg", "*.jpeg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (search.hasNext()) {
        const QString original = search.next();
        // 重複を検出するため checksum をファイル名する (重複していた場合、新しいファイルは処理しない)
        const QString hash = QString(checksum(original).toHex());
        // 移動先の sources ディレクトリ (オリジナル画像置き場) から重複を検索
        bool duplicated = false;
        if (caches.contains(hash)) {
            duplicated = true;
        }
        // 適切な場所に配置
        QString errorKey = "";
        if (! duplicated) {
            const QString destination = QDir(basePath).filePath(hash + ".jpg");
            if (cropImage) {
                Image originalImage(original);
                const QRect frame = originalImage.getValidRect();
                const auto size = std::min(frame.width(), frame.height());
                if (originalImage.cropped(frame.x(), frame.y(), size, size).save(destination)) {
                    images << QFileInfo(destination).absoluteFilePath();
                }
                else {
                    errorKey = "cropImage";
                }
            }
            if ((! cropImage) || (! errorKey.isEmpty())) {
                if (QFile::rename(original, destination)) {
                    images << QFileInfo(destination).absoluteFilePath();
                }
                else {
                    errorKey = "unknown";
                }
            }
        }
        // 重複ファイルは警告
        else {
            errorKey = "duplicated";
        }
        // エラーを記録
        if (! errorKey.isEmpty()) {
            errors[ errorKey ] << QString(original).replace(workPath + QDir::separator(), "");
        }
    }
    if (images.count() > 0) {
        // タグを更新
        updateImages(images, {{groupName, tagName}});
    }

    return errors;
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
