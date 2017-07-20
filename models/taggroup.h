#ifndef TAGGROUP_H
#define TAGGROUP_H

#include <QtCore/QtCore>
#include <memory>
#include "tag.h"


class TagGroup {
public:
    TagGroup() {}; // for Q_DECLARE_METATYPE
    TagGroup(const QString& name);
    TagGroup(const TagGroup& other);
    TagGroup(const QDir&, const QString&);

    bool exists() const;
    bool hasTag(const QString& name) const;
    QString name() const;
    bool setName(const QString& newName);
    long countOfTags() const;
    QList<Tag> tags() const;
    QDir path() const;
    QDir path(const QString& tagName) const;
    Tag tag(const QString& name) const;

    Tag createTag(const QString&);
    bool destroyTag(const QString&);
    Tag findTag(const QString&) const;
    bool saveTag(Tag&);

    const QDir* directory() const;
    void setDirectory(const QDir*);
    void setDirectory(const QDir&);

    TagGroup& operator=(const TagGroup&);
private:
    std::unique_ptr<QDir> _dir;
    QString _name;
    mutable QList<Tag> _tags;
};

Q_DECLARE_METATYPE(TagGroup)

#endif // TAGGROUP_H
