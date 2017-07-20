#include "taggroup.h"


TagGroup::TagGroup(const QString& name)
    : _dir(nullptr), _name(name)
{
}

TagGroup::TagGroup(const TagGroup& other)
    : _dir((other._dir != nullptr) ? new QDir(*(other._dir.get())) : nullptr), _name(other._name)
{
}

TagGroup::TagGroup(const QDir& dir, const QString& name)
    : _dir(dir.exists() ? new QDir(dir) : nullptr), _name(name)
{
}

bool TagGroup::exists() const
{
    return ((_dir != nullptr) && (! _name.isEmpty()) && (_dir->exists(_name)));
}

bool TagGroup::hasTag(const QString& name) const
{
    return (exists() && QDir(_dir->filePath(_name)).exists(name));
}

QString TagGroup::name() const
{
    return _name;
}

bool TagGroup::setName(const QString &newName)
{
    bool success = false;
    if (_dir != nullptr) {
        if ((! newName.isEmpty()) && (_name != newName)) {
            if ((success = QDir(*_dir).rename(_name, newName))) {
                _name = newName;
            }
        }
    }
    else {
        _name = newName;
        success = true;
    }
    return success;
}

long TagGroup::countOfTags() const
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

QList<Tag> TagGroup::tags() const
{
    if (_tags.isEmpty()) {
        if (exists()) {
            const QDir dir(*_dir);
            for (const QString& subdirName : QDir(dir.filePath(_name)).entryList(QDir::Dirs|QDir::NoDotAndDotDot, QDir::Name)) {
                _tags << Tag(QDir(dir.filePath(_name)), subdirName);
            }
        }

        QCollator collator;
        collator.setNumericMode(true);
        qSort(_tags.begin(), _tags.end(), [&collator](const Tag& t1, const Tag& t2) -> bool {
            return (collator.compare(t1.displayName(), t2.displayName()) < 0);
        });
    }
    return _tags;
}

QDir TagGroup::path() const
{
    return exists() ? QDir(_dir->filePath(_name)) : QDir();
}

QDir TagGroup::path(const QString& tagName) const
{
    return QDir(path().filePath(tagName));
}

Tag TagGroup::tag(const QString& name) const
{
    if (exists()) {
        const QList<Tag> contents = tags();
        const auto i = std::find_if(contents.begin(), contents.end(), [=](const Tag& t){
            return t.name() == name;
        });
        return (i != contents.end()) ? *i : Tag(QDir(*_dir), name);
    }
    return Tag();
}

Tag
TagGroup::createTag(const QString& tagName)
{
    Tag created;
    if (exists() && (! tagName.isEmpty())) {
        if (QDir(_dir->filePath(_name)).mkpath(tagName)) {
            created = Tag(_dir->filePath(_name), tagName);
        }
    }
    return created;
}

bool
TagGroup::destroyTag(const QString& tagName)
{
    bool destroyed = false;
    if (exists()) {
        destroyed = findTag(tagName).destroy();
    }
    return destroyed;
}

Tag
TagGroup::findTag(const QString& tagName) const
{
    Tag found;
    for (const Tag& tag : tags()) {
        if (tag.name() == tagName) {
            found = tag;
            break;
        }
    }
    return found;
}

bool
TagGroup::saveTag(Tag& tag)
{
    bool saved = false;
    if (exists()) {
        if (tag.exists()) {
            saved = QFile::rename(tag.path(), QDir(_dir->filePath(_name)).absoluteFilePath(tag.name()));
        }
        else if (! tag.name().isEmpty()) {
            saved = QDir(_dir->filePath(_name)).mkpath(tag.name());
            if (saved) {
                tag.setDirectory(QDir(_dir->filePath(_name)));
                tag.saveDisplayName();
            }
        }
    }
    return saved;
}

const QDir*
TagGroup::directory() const
{
    return _dir.get();
}

void
TagGroup::setDirectory(const QDir* dir)
{
    _dir.reset(((dir != nullptr) && dir->exists()) ? new QDir(*dir) : nullptr);
}

void
TagGroup::setDirectory(const QDir& dir)
{
    setDirectory(dir.exists() ? &dir : nullptr);
}

TagGroup&
TagGroup::operator=(const TagGroup& other)
{
    if (this != &other) {
        _name = other._name;
        _dir.reset((other._dir != nullptr) ? new QDir(*(other._dir.get())) : nullptr);
    }
    return *this;
}

