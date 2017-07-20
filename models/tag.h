#ifndef TAG_H
#define TAG_H

#include <QtCore/QtCore>
#include <memory>


class Tag {
public:
    Tag() {}; // for Q_DECLARE_METATYPE

    Tag(const QString& name);
    Tag(const Tag&);
    Tag(const QDir&, const QString&);
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

    void appendImage(const QString& path) const;
    void removeImage(const QString& name) const;
    bool destroy();

    const QDir* directory() const;
    void setDirectory(const QDir*);
    void setDirectory(const QDir&);

    Tag& operator=(const Tag&);
private:
    std::unique_ptr<QDir> _dir;
    QString _name;
    mutable QString _displayName;
    QString _descriptionPath;
};

Q_DECLARE_METATYPE(Tag)

#endif // TAG_H
