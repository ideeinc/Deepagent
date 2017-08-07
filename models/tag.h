#ifndef TAG_H
#define TAG_H

#include <QtCore/QtCore>
#include <memory>

class TagGroup;


class Tag {
    friend class TagGroup;
public:
    Tag();
    Tag(const Tag&);
    virtual ~Tag();

    QString path() const;
    bool exists() const;
    QString name() const;
    bool setName(const QString&);
    QString displayName() const;
    bool setDisplayName(const QString& displayName);
    bool loadDisplayName() const;
    bool saveDisplayName() const;
    QString groupName() const;
    long countOfImages() const;
    QStringList images() const;
    QStringList imageNames() const;
    QStringList imagePaths() const;
    bool hasImage(const QString&) const;
    TagGroup tagGroup() const;

    void appendImage(const QString& path) const;
    void removeImage(const QString& name) const;
    bool destroy();

    Tag& operator=(const Tag&);
    bool operator==(const Tag& tag) const;
    bool operator!=(const Tag& tag) const;

private:
    Tag(const QDir&, const QString&);

    const QDir* directory() const;
    void setDirectory(const QDir*);
    void setDirectory(const QDir&);

    std::unique_ptr<QDir> _dir;
    QString _name;
    mutable QString _displayName;
    QString _descriptionPath;
};

Q_DECLARE_METATYPE(Tag)

#endif // TAG_H
