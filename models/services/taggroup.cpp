#include "taggroup.h"
#include <TWebApplication>


TagGroup::TagGroup(const QString& name)
    : _dir(Tf::conf("settings").value("TagsDir").toString()), _name(name)
{
}

TagGroup::TagGroup(const TagGroup& other)
    : _dir(other._dir), _name(other._name)
{
}

bool TagGroup::exists() const
{
    return ((! _name.isEmpty()) && _dir.exists(_name));
}

bool TagGroup::hasTag(const QString& name) const
{
    return ((! name.isEmpty()) && QDir(_dir.filePath(_name)).exists(name));
}

QString TagGroup::name() const
{
    return _name;
}

bool TagGroup::setName(const QString &newName)
{
    bool success = false;
    if ((! newName.isEmpty()) && (_name != newName)) {
        if (_dir.rename(_name, newName)) {
            _name = newName;
            success = true;
        }
    }
    return success;
}

long TagGroup::countOfTags() const
{
    return tags().count();
}

QList<Tag> TagGroup::tags() const
{
    if (_tags.isEmpty()) {
        if (_dir.exists()) {
            for (const QString& subdirName : QDir(_dir.filePath(_name)).entryList(QDir::Dirs|QDir::NoDotAndDotDot, QDir::Name)) {
                _tags << Tag(subdirName, this);
            }
        }

        qSort(_tags.begin(), _tags.end(), [](const Tag& t1, const Tag& t2) -> bool {
            return t1.displayName() < t2.displayName();
        });
    }
    return _tags;
}

QDir TagGroup::path() const
{
    return QDir(_dir.filePath(_name));
}

QDir TagGroup::path(const QString& tagName) const
{
    return QDir(path().filePath(tagName));
}

Tag TagGroup::tag(const QString& name) const
{
    const QList<Tag> contents = tags();
    const auto i = std::find_if(contents.begin(), contents.end(), [=](const Tag& t){
        return t.name() == name;
    });
    return (i != contents.end()) ? *i : Tag(name, this);;
}

bool TagGroup::take(const Tag& tag) const
{
    return QFile::rename(tag.path(), QDir(_dir.filePath(_name)).absoluteFilePath(tag.name()));
}
