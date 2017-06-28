#ifndef MANAGEDFILESERVICE_H
#define MANAGEDFILESERVICE_H

#include <QtCore/QtCore>
#include "managedfile.h"

/*
 */
class TMimeEntity;

/*
 */
enum class TrimmingMode : int {
    NoTrimming = 0,
    Default = 1,
    Square = 2,
};

/*
 */
class FileError {
    Q_GADGET
public:
    enum class Type {
        NOERR,
        INVALIDCONTENT,
        IMAGECROP,
        DUPLICATED,
        UNKNOWN,
    };
    Q_ENUM(Type)

    FileError();
    FileError(const Type type, const QString& file, const QString& reason = "");
    FileError(const FileError& other);
    virtual ~FileError();

    Type type() const;
    QString typeName() const;
    QString file() const;
    QString reason() const;

private:
    Type _type { Type::UNKNOWN };
    QString _file;
    QString _reason;
};

/*
 */
typedef QList<FileError> FileErrorList;


/*
 */
class ManagedFileService {
public:
    ManagedFileService();

    std::tuple<QStringList, FileErrorList> append(const QList<TMimeEntity>&, const TrimmingMode& = TrimmingMode::Square);

    static QList<ManagedFile> find(const QString&);
    static QList<ManagedFile> findInDirectory(const QString&, const QString&);
    static QList<ManagedFile> findInList(const QString&, const QStringList&);

    static QStringList filterInList(const QString& word, const QStringList& list);

    static QByteArray checksum(const QString& path, QCryptographicHash::Algorithm algorithm = QCryptographicHash::Md5);
protected:

private:
    const QString _sourceDir;
    const QString _originalDir;

    static FileErrorList extract(const TMimeEntity&, const QString&);
};

#endif // MANAGEDFILESERVICE_H
