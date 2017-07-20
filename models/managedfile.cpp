#include "managedfile.h"
#include "logics/managedfilecontext.h"
#include <TWebApplication>


ManagedFile ManagedFile::fromJson(const QByteArray& json)
{
    const QJsonObject m = QJsonDocument::fromJson(json).object();
    return ManagedFile(m);
}

ManagedFile ManagedFile::fromConfig(const QString& configPath)
{
    QFile config(configPath);
    if (config.open(QFile::ReadOnly)) {
        return ManagedFile::fromJson(config.readAll());
    }
}

ManagedFile ManagedFile::fromHash(const QString& hash)
{
    const QDir dir(Tf::conf("settings").value("OriginalInformationDir").toString());
    if (dir.exists(hash + ".txt")) {
        ManagedFile file = ManagedFile::fromConfig(dir.filePath(hash + ".txt"));
        if (! file._path.isEmpty()) {
            if (file._name.isEmpty()) {
                file._name = QFileInfo(file._path).fileName();
            }
            if (file._hash.isEmpty()) {
                file._hash = ManagedFileContext::checksum(file._path);
            }
        }
        return file;
    }
    return ManagedFile();
}

ManagedFile ManagedFile::fromLink(const QString& link)
{
    const QFileInfo info(link);
    if (info.exists()) {
        ManagedFile file = ManagedFile::fromHash(info.completeBaseName());
        if (file._path.isEmpty()) {
            file._path = (info.isSymLink() ? info.symLinkTarget() : link);
        }
        if (file._name.isEmpty()) {
            file._name = QFileInfo(file._path).fileName();
        }
        if (file._hash.isEmpty()) {
            file._hash = ManagedFileContext::checksum(file._path);
        }
        return file;
    }
    return ManagedFile();
}

ManagedFile::ManagedFile()
{
}

ManagedFile::ManagedFile(const ManagedFile& other)
    : _name(other._name), _hash(other._hash), _path(other._path), _size(other._size)
{
}

ManagedFile::ManagedFile(const QJsonObject& config)
: _name(config["name"].toString()), _hash(config["hash"].toString()), _path(config["path"].toString()),
  _size(config["size"].toObject()["width"].toInt(), config["size"].toObject()["height"].toInt())
{
}

QString ManagedFile::name() const
{
    return _name;
}

QString ManagedFile::hash() const
{
    return _hash;
}

QString ManagedFile::path() const
{
    return _path;
}

bool ManagedFile::isEmpty() const
{
    return (_name.isEmpty() || _hash.isEmpty() || _path.isEmpty());
}

bool ManagedFile::isEqual(const ManagedFile& other) const
{
    return (_path == other._path);
}
