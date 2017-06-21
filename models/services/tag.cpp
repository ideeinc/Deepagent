#include "tag.h"
#include "taggroup.h"


Tag::Tag(const QString& name, const TagGroup* group)
    : _baseDir(group->path()), _name(name), _descriptionPath(group->path(name).filePath(".description.ini"))
{
}

Tag::Tag(const Tag& other)
    : _baseDir(other._baseDir), _name(other._name), _descriptionPath(other._descriptionPath)
{
}

Tag::~Tag()
{
}

QString Tag::path() const
{
    return _baseDir.absoluteFilePath(_name);
}

bool Tag::exists() const
{
    return _baseDir.exists(_name);
}

QString Tag::name() const
{
    return _name;
}

QString Tag::displayName() const
{
    if (! _descriptionPath.isEmpty()) {
        QFileInfo file(_descriptionPath);
        if (file.exists()) {
            QString displayName = QSettings(file.absoluteFilePath(), QSettings::IniFormat).value("displayName", _name).toString();
            return displayName.isEmpty() ? _name : displayName;
        }
    }

    return _name;
}

void Tag::setDisplayName(const QString& displayName) const
{
    if (! _descriptionPath.isEmpty()) {
        QSettings file(_descriptionPath, QSettings::IniFormat);
        file.setValue("displayName", displayName);
    }
}

QString Tag::groupName() const
{
    return QFileInfo(_baseDir.absolutePath()).baseName();
}

bool Tag::changeGroup(const QString& groupName) const
{
    return TagGroup(groupName).take(*this);
}

long Tag::countOfImages() const
{
    QProcess ls, wc;
    ls.setStandardOutputProcess(&wc);

    ls.start("ls", {"-U", _baseDir.filePath(_name)});
    wc.start("wc", {"-l"});

    wc.setProcessChannelMode(QProcess::ForwardedChannels);

    if (! ls.waitForStarted())
        return 0;
    wc.waitForFinished(-1);

    return QString(wc.readAll()).toLong();

    // return QDir(_baseDir.filePath(_name)).entryList({"*.jpg", "*.jpeg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot).count();
}

QStringList Tag::images() const
{
    QStringList images;

    const QDir dir = QDir(_baseDir.filePath(_name)).absolutePath();
    for (const QString& s : dir.entryList({"*.jpg", "*.jpeg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot)) {
        images << dir.filePath(s);
    }

    return images;
}

QStringList Tag::imageNames() const
{
    QStringList names;

    const QDir dir = QDir(_baseDir.filePath(_name)).absolutePath();
    for (const QString& s : dir.entryList({"*.jpg", "*.jpeg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot)) {
        names << s;
    }

    return names;
}

QStringList Tag::imagePaths() const
{
    QStringList paths;

    const QDir dir = QDir(_baseDir.filePath(_name)).absolutePath();
    for (const QString& s : dir.entryList({"*.jpg", "*.jpeg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot)) {
        paths << QFile::symLinkTarget(dir.filePath(s));
    }

    return paths;
}

bool Tag::hasImage(const QString& filename) const
{
    return QDir(_baseDir.filePath(_name)).exists(filename);
}

void Tag::appendImage(const QString& path) const
{
    const QFileInfo file(path);
    if (file.exists() && exists()) {
        // force append
        removeImage(file.fileName());
        QFile::link(path, QDir(_baseDir.filePath(_name)).filePath(file.fileName()));
    }
}

void Tag::removeImage(const QString& name) const
{
    // force remove
    QFile::remove(QDir(_baseDir.filePath(_name)).filePath(name));
}
