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
    return QDir(_baseDir.filePath(_name)).entryList({"*.jpg", "*.jpeg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot).count();
}

QStringList Tag::images() const
{
    QStringList images;

    const QDir dir = QDir(_baseDir.filePath(_name)).absolutePath();
    for (const QFileInfo& p : dir.entryList({"*.jpg", "*.jpeg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot)) {
        images << dir.filePath(p.fileName()); // ??? p.absoluteFilePath() is something wrong.
    }

    return images;
}

QStringList Tag::imageNames() const
{
    QStringList names;

    const QDir dir = QDir(_baseDir.filePath(_name)).absolutePath();
    for (const QFileInfo& p : dir.entryList({"*.jpg", "*.jpeg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot)) {
        names << p.fileName();
    }
    
    return names;
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
