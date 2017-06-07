#ifndef TAGSERVICE_H
#define TAGSERVICE_H

#include <QtCore>
#include "containers/taginfocontainer.h"
#include "containers/taggedimageinfocontainer.h"
#include "services/taggroup.h"

class TMimeEntity;


class TagService
{
public:
    TagService();

    QList<TagGroup> allGroups() const;
    QList<Tag> allTags() const;
    QStringList imagesWithTag(const QString& tagName, const QString& inGroupName) const;
    bool createGroup(const QString& groupName) const;
    bool createTag(const QString& inGroupName, const QString& tagName, const QString& displayName) const;
    bool destroyGroup(const QString& groupName) const;
    bool destroyTag(const QString& inGroupName, const QString& tagName) const;
    bool exists(const QString& tagName) const;
    bool updateTag(const QString& inGroupName, const QString& srcName, const QVariantMap& changes) const;
    void appendImages(const QString& groupName, const QString& tagName, const QStringList& imagePaths) const;
    void removeImages(const QString& groupName, const QString& tagName, const QStringList& imageNames) const;
    void updateImages(const QStringList& images, const QVariantMap& tags) const;
    QMap<QString, QStringList> extractImages(const QList<TMimeEntity>& files, const QString& groupName, const QString& tagName, const bool cropImage = true);

    TagInfoContainer info(const QString& groupName, const QString& tagName, const long& page = 0, const long& limit = 200) const;
    TaggedImageInfoContainer image(const QString& groupName, const QString& primaryTag, const QStringList& images, const long& index) const;

private:
    const QDir _listDir;
};

#endif // TAGSERVICE_H
