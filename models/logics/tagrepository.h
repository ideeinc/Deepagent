#ifndef TAGREPOSITORY_H
#define TAGREPOSITORY_H

#include <QtCore/QtCore>
#include "taggroup.h"
#include "tag.h"

class TagRepository
{
public:
    TagRepository();
    TagRepository(const QDir&);

    // Groups
    QList<TagGroup> allGroups() const;
    TagGroup createGroup(const QString&) const;
    bool destroyGroup(const QString&) const;
    TagGroup findGroup(const QString&) const;
    bool saveGroup(TagGroup&);

    // Tags
    QList<Tag> allTags() const;
    Tag createTag(const QString&, const QString&, const QString& = "");
    bool destroyTag(const QString&, const QString&);
    bool destroyTag(const QString&);
    Tag findTag(const QString&) const;
    bool updateTag(const QString&, const QString&, const QVariantMap&);
    bool updateTag(const QString&, const QVariantMap&);
    bool updateTag(Tag&, const QVariantMap&);

    // Images
    void appendImages(const QStringList&, const QString&, const QString&);
    void removeImages(const QStringList&, const QString&, const QString&);
    void updateImages(const QStringList&, const QString&, const QString&);
    void updateImages(const QStringList&, const QVariantMap&);

    // Miscs
    QDir baseDir() const;
private:
    QDir _baseDir;
};

#endif // TAGREPOSITORY_H
