#include "logics/tagrepository.h"
#include <TWebApplication>


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

    Tag created = group.findTag(tagName);
    if (! created.exists()) {
        created = group.createTag(tagName);
    }
    created.setDisplayName(tagDisplayName);

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
    }
}

void
TagRepository::updateImages(const QStringList& images, const QString& groupName, const QString& tagName)
{
    if (! tagName.isEmpty()) {
        appendImages(images, groupName, tagName);
    }

    for (const Tag& tag : findGroup(groupName).tags()) {
        if (tag.name() != tagName) {
            removeImages(images, groupName, tag.name());
        }
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
