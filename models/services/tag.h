#ifndef TAG_H
#define TAG_H

#include <QtCore/QtCore>

class TagGroup;


class Tag {
public:
    Tag() {}; // for Q_DECLARE_METATYPE

    Tag(const QString& name, const TagGroup* group);
    Tag(const Tag&);
    virtual ~Tag();

    QString path() const;
    bool exists() const;
    QString name() const;
    QString displayName() const;
    QString groupName() const;
    long countOfImages() const;
    QStringList images() const;
    QStringList imageNames() const;
    QStringList imagePaths() const;
    bool hasImage(const QString&) const;

    void setDisplayName(const QString& displayName) const;
    bool changeGroup(const QString& groupName) const;
    void appendImage(const QString& path) const;
    void removeImage(const QString& name) const;

private:
    QDir _baseDir;
    QString _name;
    QString _descriptionPath;
};

Q_DECLARE_METATYPE(Tag)

#endif // TAG_H
