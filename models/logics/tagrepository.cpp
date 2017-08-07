#include "logics/tagrepository.h"
#include <TWebApplication>
#include <TScheduler>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMutex>
#include <QMutexLocker>

// make symlink for `tags` to public
namespace {
    struct TagDirMakePublicLink {
        TagDirMakePublicLink() {
            const QString sourcePath = TagRepository().baseDir().absolutePath();
            const QString linkPath = QDir(Tf::app()->publicPath()).absoluteFilePath(QFileInfo(sourcePath).fileName());
            QFile::link(sourcePath, linkPath);
        };
    } TagDirMakePublicLink;
};

TagRepository::TagRepository()
    : _baseDir(Tf::conf("settings").value("TagsDir").toString())
{
}

TagRepository::TagRepository(const QDir& listDir)
    : _baseDir(listDir)
{
}

QList<TagGroup>
TagRepository::allGroups() const
{
    QList<TagGroup> groups;

    if (_baseDir.exists()) {
        for (const QString& name : _baseDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot, QDir::Name)) {
            groups << TagGroup(_baseDir, name);
        }
    }

    return groups;
}

TagGroup
TagRepository::createGroup(const QString& groupName) const
{
    TagGroup created;
    if (! groupName.isEmpty()) {
        if (_baseDir.mkpath(groupName)) {
            created = TagGroup(_baseDir, groupName);
        }
    }
    return created;
}

bool
TagRepository::destroyGroup(const QString& groupName) const
{
    bool destroyed = false;
    if (findGroup(groupName).exists()) {
        destroyed = QDir(_baseDir.filePath(groupName)).removeRecursively();
    }
    return destroyed;
}

TagGroup
TagRepository::findGroup(const QString& groupName) const
{
    TagGroup found;
    if (! groupName.isEmpty()) {
        if (_baseDir.exists(groupName)) {
            found = TagGroup(_baseDir, groupName);
        }
    }
    return found;
}

bool
TagRepository::saveGroup(TagGroup& group)
{
    bool created = false;
    const QString groupName(group.name());
    if (! groupName.isEmpty()) {
        created = _baseDir.mkpath(groupName);
        if (created) {
            group.setDirectory(_baseDir);
        }
    }
    return created;
}

QList<Tag>
TagRepository::allTags() const
{
    QList<Tag> tags;

    for (const TagGroup& group : allGroups()) {
        tags << group.tags();
    }

    return tags;
}

Tag
TagRepository::createTag(const QString& groupName, const QString& tagName, const QString& tagDisplayName)
{
    TagGroup group = findGroup(groupName);
    if (! group.exists()) {
        group = createGroup(groupName);
    }

    Tag created = group.createTag(tagName);
    if (created.exists()) {
        created.setDisplayName(tagDisplayName);

        QStringList alltags;
        for (auto& tag : TagRepository().allTags()) {
            alltags << tag.name();
        }

        // Tag name is system unique.
        if (alltags.filter(tagName).count() > 1) {
            created.destroy();
        }
    }

    return created;
}

bool
TagRepository::destroyTag(const QString& groupName, const QString& tagName)
{
    return findGroup(groupName).destroyTag(tagName);
}

bool
TagRepository::destroyTag(const QString& tagName)
{
    return findTag(tagName).destroy();
}

Tag
TagRepository::findTag(const QString& tagName) const
{
    Tag found;
    if (! tagName.isEmpty()) {
        for (const TagGroup& group : allGroups()) {
            found = group.findTag(tagName);
            if (found.exists()) break;
        }
    }
    return found;
}

bool
TagRepository::updateTag(const QString& groupName, const QString& tagName, const QVariantMap& changes)
{
    Tag sourceTag = findGroup(groupName).findTag(tagName);
    if (sourceTag.exists()) {
        return updateTag(sourceTag, changes);
    }
    return false;
}

bool
TagRepository::updateTag(const QString& tagName, const QVariantMap& changes)
{
    Tag sourceTag = findTag(tagName);
    if (sourceTag.exists()) {
        return updateTag(sourceTag, changes);
    }
    return false;
}

bool
TagRepository::updateTag(Tag& sourceTag, const QVariantMap& changes)
{
    bool succeed = false;

    if (sourceTag.exists()) {
        const QString name = changes.value("name").toString();
        if ((! name.isEmpty()) && (name != sourceTag.name())) {
            // tag name is system unique
            if (findTag(name).exists()) {
                return false;
            }
            succeed = sourceTag.setName(name);
        }

        const QString displayName = changes.value("displayName").toString();
        if ((! displayName.isEmpty()) && (displayName != sourceTag.displayName())) {
            sourceTag.setDisplayName(displayName);
            succeed = true;
        }

        const QString groupName = changes.value("groupName").toString();
        if ((! groupName.isEmpty()) && (groupName != sourceTag.groupName())) {
            TagGroup destinationGroup = findGroup(groupName);
            if (destinationGroup.exists()) {
                succeed = destinationGroup.saveTag(sourceTag);
            }
        }
    }

    return succeed;
}

void
TagRepository::appendImages(const QStringList& images, const QString& groupName, const QString& tagName)
{
    const TagGroup group = findGroup(groupName);
    if ((! group.exists()) || (! group.hasTag(tagName))) {
        createTag(groupName, tagName);
    }

    const Tag tag = group.findTag(tagName);
    for (const QString& path : images) {
        tag.appendImage(path);
        asyncGenerateTagResolution(path);
    }
}

void
TagRepository::removeImages(const QStringList& images, const QString& groupName, const QString& tagName)
{
    const TagGroup group = findGroup(groupName);
    if ((! group.exists()) || (! group.hasTag(tagName))) {
        return;
    }

    const Tag tag = group.findTag(tagName);
    for (const QString& path : images) {
        tag.removeImage(QFileInfo(path).fileName());
        asyncGenerateTagResolution(path);
    }
}

void
TagRepository::updateImages(const QStringList& images, const QString& groupName, const QString& tagName)
{
    TagGroup group = findGroup(groupName);
    Tag tag = group.findTag(tagName);

    for (const QString& path : images) {
        if (! tagName.isEmpty()) {
            tag.appendImage(path);
        }
        for (const Tag& otherTag : group.tags()) {
            if (otherTag.name() != tagName) {
                otherTag.removeImage(QFileInfo(path).fileName());
            }
        }
        asyncGenerateTagResolution(path);
    }
}

void
TagRepository::updateImages(const QStringList& images, const QVariantMap& tags)
{
    for (const QString& groupName : tags.keys()) {
        const QString tagName = tags.value(groupName).toString();
        updateImages(images, groupName, tagName);
    }
}

QDir
TagRepository::baseDir() const
{
    return _baseDir;
}

QList<Tag>
TagRepository::getTags(const QString& image, const QList<TagGroup> excludes) const
{
    static const auto TagResolutionDir = QDir(Tf::conf("settings").value("TagResolutionDir").toString());
    QList<Tag> tags;
    QFileInfo fi(image);
    QFile resolveJson(TagResolutionDir.absoluteFilePath(fi.completeBaseName() + ".json"));

    if (! resolveJson.exists()) {
        generateTagResolution(image);
    }

    if (resolveJson.open(QIODevice::ReadOnly)) {
        // Read from JSON
        auto json = resolveJson.readAll();
        resolveJson.close();

        auto tagobj = QJsonDocument::fromJson(json).object().value("tags").toObject();
        if (! json.isEmpty()) {
            bool regen = false;
            QList<Tag> excludeTags;
            for (auto &tg : excludes) {
                excludeTags << tg.tags();
            }

            for (auto it = tagobj.constBegin(); it != tagobj.constEnd(); ++it) {
                Tag t = findGroup(it.key()).findTag(it.value().toString());
                if (t.exists()) {
                    if (! excludeTags.contains(t)) {
                        tags << t;
                    }
                } else {
                    regen = true;
                }
            }

            if (regen) {
                // Regenerates
                generateTagResolution(image);
            }
            return tags;
        }
    }

    tError() << "Tag resolution error : " << fi.fileName();
    return tags;
}

static bool
generateTagResolutionFile(const QString& image, const QList<TagGroup>& allGroups)
{
    static const auto TagResolutionDir = QDir(Tf::conf("settings").value("TagResolutionDir").toString());
    const QFileInfo fi(image);
    const QString filename = fi.fileName();

    QJsonObject tagObj;
    for (const TagGroup& g : allGroups) {
        for (const Tag& t : g.tags()) {
            if (t.hasImage(filename)) {
                tagObj.insert(g.name(), t.name());
            }
        }
    }

    QJsonObject json;
    json.insert("tags", tagObj);
    auto jsondata = QJsonDocument(json).toJson(QJsonDocument::Compact);

    if (! TagResolutionDir.exists()) {
        TagResolutionDir.mkpath(".");
    }

    QFile resolveJson(TagResolutionDir.absoluteFilePath(fi.completeBaseName() + ".json"));
    if (resolveJson.exists() && resolveJson.open(QIODevice::ReadOnly)) {
        auto currentJson = resolveJson.readAll();
        resolveJson.close();

        if (currentJson == jsondata) {
            // not write
            return true;
        }
    }

    if (resolveJson.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        resolveJson.write(jsondata);
        resolveJson.close();
        tInfo() << "tag resolution json created: " << resolveJson.fileName();
    } else {
        tError() << "write error tag resolution json: " << resolveJson.fileName();
        return false;
    }
    return true;
}

bool
TagRepository::generateTagResolution(const QString& image) const
{
    return generateTagResolutionFile(image, allGroups());
}

void
TagRepository::asyncGenerateTagResolution(const QString& image)
{
    //
    // TagUpdater class declaration
    //
    class TagUpdater : public TScheduler {
    public:
        TagUpdater() : TScheduler() { setSingleShot(true); }
        void generate(const QString& image) { generate(QStringList(image)); }
        void generate(const QStringList& images)
        {
            QMutexLocker locker(&_mutex);
            _images << images;
            start(0);
        }

    private:
        void job() override
        {
            TagRepository repo;
            for (;;) {
                QMutexLocker locker(&_mutex);
                if (_images.isEmpty()) {
                    break;
                }
                auto img = _images.takeFirst();
                locker.unlock();
                if (! img.isEmpty()) {
                    repo.generateTagResolution(img);
                }
            }
        }

        QStringList _images;
        QMutex _mutex;
    };

    static TagUpdater* tagUpdater = new TagUpdater;
    tagUpdater->generate(image);
}

void
TagRepository::regenerateTagResolution() const
{
    static const auto OriginalImagesDir = Tf::conf("settings").value("OriginalImagesDir").toString();
    const auto allgroups = allGroups();

    QDirIterator it(OriginalImagesDir, {"*.jpg", "*.jpeg"}, QDir::Files | QDir::Readable, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        generateTagResolutionFile(it.next(), allgroups);
    }
}

/* JSON sample
{
    "tags": {
        "フォルダ（大腸炎）": "ulcerativecolitis_0630_001-420_mayo2_r",
        "全体病変": "ulcerativecolitis_mayo2",
        "照明": "normal",
        "部位": "e07daichorectum"
    }
}
*/
