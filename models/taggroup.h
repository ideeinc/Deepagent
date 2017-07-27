#ifndef TAGGROUP_H
#define TAGGROUP_H

#include <QtCore/QtCore>
#include <memory>
#include "tag.h"


class TagGroup {
    friend class TagRepository;
    friend class Tag;
public:
    TagGroup();
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

    Tag createTag(const QString&);
    bool destroyTag(const QString&);
    Tag findTag(const QString&) const;
    bool saveTag(Tag&);

    TagGroup& operator=(const TagGroup&);
private:
    TagGroup(const QDir&, const QString&);

    const QDir* directory() const;
    void setDirectory(const QDir*);
    void setDirectory(const QDir&);

    std::unique_ptr<QDir> _dir;
    QString _name;
    mutable QList<Tag> _tags;
};

Q_DECLARE_METATYPE(TagGroup)

#endif // TAGGROUP_H
