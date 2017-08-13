#ifndef MANAGEDFILE_H
#define MANAGEDFILE_H

#include <QtCore/QtCore>

class Tag;
class TagGroup;


class ManagedFile {
public:
    static ManagedFile fromJson(const QByteArray&);
    static ManagedFile fromConfig(const QString&);
    static ManagedFile fromHash(const QString&);
    static ManagedFile fromLink(const QString&);
    static ManagedFile fromFileName(const QString&);

    ManagedFile();
    ManagedFile(const ManagedFile&);
    ManagedFile(const QJsonObject&);

    QString name() const;
    QString hash() const;
    QString path() const;

    bool isEmpty() const;
    ManagedFile& operator=(const ManagedFile& other);
    bool operator==(const ManagedFile& other) const;
    bool operator!=(const ManagedFile& other) const;

    // Tag
    QList<Tag> getTags() const;
    QList<Tag> getTags(const QList<TagGroup> excludes) const;
    bool addTag(const Tag& tag, bool force = true);
    int addTags(const QList<Tag>& tags, bool force = true);
    bool updateTag(const QList<Tag>& addtags, const QList<TagGroup>& rmgroups);
    bool removeTag(const Tag& tag);
    int removeTag(const TagGroup& group);
    int removeTags(const QList<Tag>& tags);
    int removeTags(const QList<TagGroup>& groups);
    bool generateTagResolution() const;
    void asyncGenerateTagResolution();
    static void regenerateAllTagResolution();
    static bool updateTag(const QString& image, const QString& groupName, const QString& tagName);
    static QString fileNameToPublicPath(const QString&);

private:
    bool addTagInternal(const Tag& tag, bool force, bool refreshResolution);
    int addTagsInternal(const QList<Tag>& tags, bool force, bool refreshResolution);
    bool removeTagInternal(const Tag& tag, bool refreshResolution);
    int removeTagInternal(const TagGroup& group, bool refreshResolution);
    int removeTagsInternal(const QList<TagGroup>& groups, bool refreshResolution);

    QString _name;
    QString _hash;
    QString _path;
    QSize _size;
};

#endif // MANAGEDFILE_H
