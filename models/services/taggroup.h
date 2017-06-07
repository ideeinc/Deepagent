#ifndef TAGGROUP_H
#define TAGGROUP_H

#include <QtCore/QtCore>
#include "services/tag.h"


class TagGroup {
public:
    TagGroup() {}; // for Q_DECLARE_METATYPE
    TagGroup(const QString& name);
    TagGroup(const TagGroup& other);

    bool exists() const;
    bool hasTag(const QString& name) const;
    QString name() const;
    bool setName(const QString& newName);
    long countOfTags() const;
    QList<Tag> tags() const;
    QDir path() const;
    QDir path(const QString& tagName) const;
    Tag tag(const QString& name) const;
    bool take(const Tag& tag) const;

private:
    QDir _dir;
    QString _name;
    mutable QList<Tag> _tags;
};

Q_DECLARE_METATYPE(TagGroup)

#endif // TAGGROUP_H
