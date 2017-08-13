#include "tag.h"
#include "taggroup.h"
#include <TDebug>

Tag::Tag()
{
}

Tag::Tag(const Tag& other)
    : _dir((other._dir != nullptr) ? new QDir(*(other._dir.get())) : nullptr), _name(other._name), _displayName(other._displayName), _descriptionPath(other._descriptionPath)
{
}

Tag::Tag(const QDir& dir, const QString& name)
    : _dir(dir.exists() ? new QDir(dir) : nullptr), _name(name), _displayName(""), _descriptionPath(dir.exists() ? QDir(dir.filePath(name)).absoluteFilePath(".description.ini") : "")
{
}

Tag::~Tag()
{
}

QString Tag::path() const
{
    return exists() ? _dir->absoluteFilePath(_name) : "";
}

bool Tag::exists() const
{
    return ((_dir != nullptr) && (! _name.isEmpty()) && _dir->exists(_name));
}

QString Tag::name() const
{
    return _name;
}

bool
Tag::setName(const QString& name)
{
    bool succeed = false;
    if (exists()) {
        if (QFile::rename(_dir->filePath(_name), _dir->filePath(name))) {
            _name = name;
            _descriptionPath = QDir(_dir->filePath(name)).absoluteFilePath(".description.ini");
            succeed = true;
        }
    }
    else {
        _name = name;
        succeed = true;
    }
    return succeed;
}

QString Tag::displayName() const
{
    if (! _displayName.isEmpty()) {
        return _displayName;
    }
    if (exists() && (! _descriptionPath.isEmpty())) {
        return (loadDisplayName() ? _displayName : _name);
    }

    return _name;
}

bool Tag::setDisplayName(const QString& displayName)
{
    _displayName = displayName;

    if (exists() && (! _descriptionPath.isEmpty()) && (! _displayName.isEmpty())) {
        return saveDisplayName();
    }
    return true;
}

bool
Tag::loadDisplayName() const
{
    bool loaded = false;
    if (exists() && (! _descriptionPath.isEmpty())) {
        QFileInfo file(_descriptionPath);
        if (file.exists()) {
            _displayName = QSettings(file.absoluteFilePath(), QSettings::IniFormat).value("displayName", _name).toString();
            loaded = (! _displayName.isEmpty());
        }
    }
    return loaded;
}

bool
Tag::saveDisplayName() const
{
    bool saved = false;
    if (exists() && (! _descriptionPath.isEmpty()) && (! _displayName.isEmpty())) {
        QSettings file(_descriptionPath, QSettings::IniFormat);
        file.setValue("displayName", _displayName);
        saved = true;
    }
    return saved;
}

QString Tag::groupName() const
{
    return QFileInfo(_dir->absolutePath()).baseName();
}

long Tag::countOfImages() const
{
    if (exists()) {
        QProcess ls, wc;
        ls.setStandardOutputProcess(&wc);

        ls.start("ls", {"-U", _dir->filePath(_name)});
        wc.start("wc", {"-l"});

        wc.setProcessChannelMode(QProcess::ForwardedChannels);

        if (! ls.waitForStarted())
            return 0;
        wc.waitForFinished();
        ls.waitForFinished();

        return QString(wc.readAll()).toLong();
    }
    return 0L;
}

QStringList Tag::images(QDir::SortFlags sort) const
{
    QStringList images;

    if (exists()) {
        const QDir dir = QDir(_dir->filePath(_name)).absolutePath();
        for (const QString& s : dir.entryList({"*.jpg", "*.jpeg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot, sort)) {
            images << dir.filePath(s);
        }
    }

    return images;
}

QStringList Tag::imageNames(QDir::SortFlags sort) const
{
    QStringList names;

    if (exists()) {
        const QDir dir = QDir(_dir->filePath(_name)).absolutePath();
        for (const QString& s : dir.entryList({"*.jpg", "*.jpeg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot, sort)) {
            names << s;
        }
    }

    return names;
}

QStringList Tag::imagePaths(QDir::SortFlags sort) const
{
    QStringList paths;

    if (exists()) {
        const QDir dir = QDir(_dir->filePath(_name)).absolutePath();
        for (const QString& s : dir.entryList({"*.jpg", "*.jpeg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot, sort)) {
            const QString path(dir.filePath(s));
            paths << (QFileInfo(path).isSymLink() ? QFile::symLinkTarget(path) : path);
        }
    }

    return paths;
}

bool Tag::hasImage(const QString& filename) const
{
    return QDir(_dir->filePath(_name)).exists(filename);
}

TagGroup Tag::tagGroup() const
{
    return exists() ? TagGroup(QDir(_dir->path() + QLatin1String("/..")), _dir->dirName()) : TagGroup();
}

void Tag::appendImage(const QString& path) const
{
    const QFileInfo file( QFileInfo(path).isSymLink() ? QFile::symLinkTarget(path) : path );
    if (file.exists() && exists()) {
        // force append
        removeImage(file.fileName());
        QFile::link(file.absoluteFilePath(), QDir(_dir->filePath(_name)).filePath(file.fileName()));
    }
}

void Tag::removeImage(const QString& name) const
{
    // force remove
    QFile::remove(QDir(_dir->filePath(_name)).filePath( QFileInfo(name).fileName() ));
}

bool
Tag::destroy()
{
    bool destroyed = false;
    if (exists()) {
        destroyed = QDir(_dir->filePath(_name)).removeRecursively();

        // clear params on success.
        if (destroyed) {
            _dir.reset();
            _name = "";
            _displayName = "";
            _descriptionPath = "";
        }
    }
    return destroyed;
}

const QDir*
Tag::directory() const
{
    return _dir.get();
}

void
Tag::setDirectory(const QDir* dir)
{
    if ((dir != nullptr) && dir->exists()) {
        _dir.reset(new QDir(*dir));
        _descriptionPath = QDir(_dir->filePath(_name)).absoluteFilePath(".description.ini");
    }
    else {
        _dir.reset(nullptr);
        _descriptionPath = "";
    }
}

void
Tag::setDirectory(const QDir& dir)
{
    setDirectory(dir.exists() ? &dir : nullptr);
}

Tag&
Tag::operator=(const Tag& other)
{
    if (this != &other) {
        _name = other._name;
        _displayName = other._displayName;
        _dir.reset((other._dir != nullptr) ? new QDir(*(other._dir.get())) : nullptr);
        _descriptionPath = other._descriptionPath;
    }
    return *this;
}

bool
Tag::operator==(const Tag& tag) const
{
    return _name == tag._name && _displayName == tag._displayName && _descriptionPath == tag._descriptionPath
           && _dir.get()->absolutePath() == tag._dir.get()->absolutePath();
}

bool
Tag::operator!=(const Tag& tag) const
{
    return !operator==(tag);
}
