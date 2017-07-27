#ifndef MANAGEDFILE_H
#define MANAGEDFILE_H

#include <QtCore/QtCore>


class ManagedFile {
public:
    static ManagedFile fromJson(const QByteArray&);
    static ManagedFile fromConfig(const QString&);
    static ManagedFile fromHash(const QString&);
    static ManagedFile fromLink(const QString&);
    static ManagedFile fromFileName(const QString&);
    static QString fileNameToPublicPath(const QString&);

    ManagedFile();
    ManagedFile(const ManagedFile&);
    ManagedFile(const QJsonObject&);

    QString name() const;
    QString hash() const;
    QString path() const;

    bool isEmpty() const;
    bool isEqual(const ManagedFile&) const;
private:
    QString _name;
    QString _hash;
    QString _path;
    QSize _size;
};

#endif // MANAGEDFILE_H

