#include "managedfile.h"
#include "tagrepository.h"
#include "tag.h"
#include "taggroup.h"
#include "logics/managedfilecontext.h"
#include <TWebApplication>
#include <TScheduler>

#define SETTING(key) (Tf::conf("settings").value(#key).toString())


class PathMap : public QList<QPair<QString, QString>>
{
public:
    PathMap()
    {
        append(qMakePair(SETTING(OriginalImagesDir), SETTING(OriginalImagesPublicPath)));
        append(qMakePair(SETTING(TagsDir), SETTING(TagsPublicPath)));
    }
};
Q_GLOBAL_STATIC(PathMap, pathMap)


ManagedFile ManagedFile::fromJson(const QByteArray& json)
{
    const QJsonObject m = QJsonDocument::fromJson(json).object();
    return ManagedFile(m);
}

ManagedFile ManagedFile::fromConfig(const QString& configPath)
{
    QFile config(configPath);
    if (config.open(QFile::ReadOnly)) {
        return ManagedFile::fromJson(config.readAll());
    }
    return ManagedFile();
}

ManagedFile ManagedFile::fromHash(const QString& hash)
{
    static const QDir dir(Tf::conf("settings").value("OriginalInformationDir").toString());
    if (dir.exists(hash + ".txt")) {
        ManagedFile file = ManagedFile::fromConfig(dir.filePath(hash + ".txt"));
        if (! file._path.isEmpty()) {
            if (file._name.isEmpty()) {
                file._name = QFileInfo(file._path).fileName();
            }
            if (file._hash.isEmpty()) {
                file._hash = ManagedFileContext::checksum(file._path);
            }
        }
        return file;
    }
    return ManagedFile();
}

ManagedFile ManagedFile::fromLink(const QString& link)
{
    const QFileInfo info(link);
    if (info.exists()) {
        ManagedFile file = ManagedFile::fromHash(info.completeBaseName());
        if (file._path.isEmpty()) {
            file._path = (info.isSymLink() ? info.symLinkTarget() : link);
        }
        if (file._name.isEmpty()) {
            file._name = QFileInfo(file._path).fileName();
        }
        if (file._hash.isEmpty()) {
            file._hash = ManagedFileContext::checksum(file._path);
        }
        return file;
    }
    return ManagedFile();
}

ManagedFile ManagedFile::fromFileName(const QString& image)
{
    return fromHash(QFileInfo(image).completeBaseName());
}

QString ManagedFile::fileNameToPublicPath(const QString& image)
{
    QString path = fromFileName(image).path();

    for (auto &p : *pathMap()) {
        if (path.startsWith(p.first)) {
            path.remove(0, p.first.length());
            path.prepend(p.second);
            break;
        }
    }
    return path;
}

ManagedFile::ManagedFile()
{
}

ManagedFile::ManagedFile(const ManagedFile& other)
    : _name(other._name), _hash(other._hash), _path(other._path), _size(other._size)
{
}

ManagedFile::ManagedFile(const QJsonObject& config)
: _name(config["name"].toString()), _hash(config["hash"].toString()), _path(config["path"].toString()),
  _size(config["size"].toObject()["width"].toInt(), config["size"].toObject()["height"].toInt())
{
}

QString ManagedFile::name() const
{
    return _name;
}

QString ManagedFile::hash() const
{
    return _hash;
}

QString ManagedFile::path() const
{
    return _path;
}

bool ManagedFile::isEmpty() const
{
    return (_name.isEmpty() || _hash.isEmpty() || _path.isEmpty());
}

ManagedFile& ManagedFile::operator=(const ManagedFile& other)
{
    _name = other._name;
    _hash = other._hash;
    _path = other._path;
    _size = other._size;
    return *this;
}

bool ManagedFile::operator==(const ManagedFile& other) const
{
    return _path == other._path && _hash == other._hash && _path == other._path && _size == other._size;
}

bool ManagedFile::operator!=(const ManagedFile& other) const
{
    return !operator==(other);
}

QList<Tag> ManagedFile::getTags() const
{
    return getTags(QList<TagGroup>());
}

QList<Tag> ManagedFile::getTags(const QList<TagGroup> excludes) const
{
    static const auto TagResolutionDir = QDir(Tf::conf("settings").value("TagResolutionDir").toString());
    QList<Tag> tags;
    QFileInfo fi(path());
    QFile resolveJson(TagResolutionDir.absoluteFilePath(fi.completeBaseName() + ".json"));
    TagRepository repo;

    if (isEmpty()) {
        return tags;
    }

    if (! resolveJson.exists()) {
        generateTagResolution();
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
                Tag t = repo.findGroup(it.key()).findTag(it.value().toString());
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
                generateTagResolution();
            }
            return tags;
        }
    }

    tError() << "Tag resolution error : " << fi.fileName();
    return tags;
}


template <class T>
QList<T> intersect(const QList<T>& a, const QList<T>& b)
{
    QList<T> ret = a;
    const QList<T> &x = b;

    for (int i = ret.count() - 1; i >= 0; i--) {
        if (! x.contains(ret[i])) {
            ret.removeAt(i);
        }
    }
    return ret;
}

bool ManagedFile::addTagInternal(const Tag& tag, bool force, bool refreshResolution)
{
    const QFileInfo file(path());
    if (isEmpty() || ! tag.exists()) {
        return false;
    }

    auto ptags = tag.tagGroup().tags();
    auto intersects = intersect<Tag>(ptags, getTags());
    if (! intersects.isEmpty()) {
        if (force) {
            removeTags(intersects);
        } else {
            return false;
        }
    }

    tDebug() << "addTag: " << QDir(tag.path()).absoluteFilePath(file.fileName());
    QFile::link(file.absoluteFilePath(), QDir(tag.path()).filePath(file.fileName()));

    if (refreshResolution) {
        generateTagResolution();
    }
    return true;
}

bool ManagedFile::addTag(const Tag& tag, bool force)
{
    return addTagInternal(tag, force, true);
}

int ManagedFile::addTagsInternal(const QList<Tag>& tags, bool force, bool refreshResolution)
{
    int cnt = 0;
    if (! tags.isEmpty()) {
        for (auto &t : tags) {
            if (addTagInternal(t, force, false)) {
                cnt++;
            }
        }

        if (refreshResolution) {
            generateTagResolution();
        }
    }
    return cnt;
}

int ManagedFile::addTags(const QList<Tag>& tags, bool force)
{
    return addTagsInternal(tags, force, true);
}

bool ManagedFile::updateTag(const QList<Tag>& addtags, const QList<TagGroup>& rmgroups)
{
    if (isEmpty()) {
        return false;
    }

    int cnt = removeTagsInternal(rmgroups, false);
    cnt += addTagsInternal(addtags, true, false);
    if (cnt > 0) {
        generateTagResolution();
    }
    return true;
}

bool ManagedFile::removeTagInternal(const Tag& tag, bool refreshResolution)
{
    if (isEmpty() || ! tag.exists()) {
        return false;
    }

    QFile link(QDir(tag.path()).filePath(QFileInfo(path()).fileName()));
    if (! link.exists()) {
        return false;
    }

    tDebug() << "removeTag: " << QFileInfo(link).absoluteFilePath();
    link.remove();

    if (refreshResolution) {
        generateTagResolution();
    }
    return true;
}

bool ManagedFile::removeTag(const Tag& tag)
{
    return removeTagInternal(tag, true);
}

int ManagedFile::removeTagInternal(const TagGroup& group, bool refreshResolution)
{
    int cnt = 0;
    if (! group.exists()) {
        return cnt;
    }

    for (auto &t : group.tags()) {
        if (removeTagInternal(t, false)) {
            cnt++;
        }
    }

    if (refreshResolution) {
        generateTagResolution();
    }
    return cnt;
}

int ManagedFile::removeTag(const TagGroup& group)
{
    return removeTagInternal(group, true);
}

int ManagedFile::removeTags(const QList<Tag>& tags)
{
    int cnt = 0;
    for (auto &t : tags) {
        if (removeTagInternal(t, false)) {
            cnt++;
        }
    }
    generateTagResolution();
    return cnt;
}

int ManagedFile::removeTagsInternal(const QList<TagGroup>& groups, bool refreshResolution)
{
    int cnt = 0;
    for (auto &g : groups) {
        cnt += removeTagInternal(g, false);
    }

    if (refreshResolution) {
       generateTagResolution();
    }
    return cnt;
}

int ManagedFile::removeTags(const QList<TagGroup>& groups)
{
    return removeTagsInternal(groups, true);
}

// Removes the tag group from the image if the tagName is empty; otherwise adds the tag.
bool ManagedFile::updateTag(const QString& image, const QString& groupName, const QString& tagName)
{
    auto mngf = ManagedFile::fromFileName(image);

    if (mngf.isEmpty()) {
        return false;
    }

    TagRepository repo;
    if (tagName.isEmpty()) {
        auto grp = repo.findGroup(groupName);
        return mngf.removeTag(grp);
    } else {
        auto tag = repo.findTag(tagName);
        return mngf.addTag(tag, true);
    }
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
            tDebug() << "no change tag resolution: " << filename;
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

bool ManagedFile::generateTagResolution() const
{
    return (! isEmpty()) ? generateTagResolutionFile(path(), TagRepository().allGroups()) : false;
}

void ManagedFile::asyncGenerateTagResolution()
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
            _images = _images.toSet().unite(images.toSet()).toList();
            start(0);
        }

    private:
        void job() override
        {
            for (;;) {
                QMutexLocker locker(&_mutex);
                if (_images.isEmpty()) {
                    break;
                }
                auto img = _images.takeFirst();
                locker.unlock();
                if (! img.isEmpty()) {
                    ManagedFile::fromFileName(img).generateTagResolution();
                }
            }
        }

        QStringList _images;
        QMutex _mutex;
    };

    static TagUpdater* tagUpdater = new TagUpdater;
    tagUpdater->generate(path());
}

void ManagedFile::regenerateAllTagResolution()
{
    static const auto OriginalImagesDir = Tf::conf("settings").value("OriginalImagesDir").toString();
    const auto allgroups = TagRepository().allGroups();

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
