#include "managedfileservice.h"
#include "image.h"
#include <TWebApplication>
#include <THttpRequest>


namespace {
    typedef struct OriginalFile {
    public:
        QString path;
        QString name;
        QString hash;
        QSize size;
        Image image;

        OriginalFile(const QFileInfo& file)
         : path(file.absoluteFilePath()), name(file.fileName()), hash(QString(ManagedFileService::checksum(path).toHex())), image(path) {
             size = QSize(image.width(), image.height());
        }
        void saveTo(const QString& dir, const QString& destination) const
        {
            if (! hash.isEmpty()) {
                QFile file(QDir(dir).filePath(hash + ".txt"));
                if (file.open(QFile::WriteOnly)) {
                    file.write(QJsonDocument::fromVariant(QVariantMap{
                        { "name", name },
                        { "hash", hash },
                        { "size", QVariantMap{{"width", size.width()}, {"height", size.height()}} },
                        { "path", destination },
                    }).toJson());
                    file.close();
                }
            }
        }
    } OriginalFile;
};


FileError::FileError()
{
}

FileError::FileError(const FileError& other)
: _type(other._type), _file(other._file), _reason(other._reason)
{
}

FileError::FileError(const Type type, const QString& file, const QString& reason)
    : _type(type), _file(file), _reason(reason)
{
}

FileError::~FileError()
{
}

FileError::Type FileError::type() const
{
    return _type;
}

QString FileError::typeName() const
{
    QMetaEnum metaEnum = QMetaEnum::fromType<FileError::Type>();
    return metaEnum.valueToKey(static_cast<int>(type()));
}

QString FileError::file() const
{
    return _file;
}

QString FileError::reason() const
{
    return _reason;
}


ManagedFileService::ManagedFileService()
    : _sourceDir(Tf::conf("settings").value("OriginalImagesDir").toString()), _originalDir(Tf::conf("settings").value("OriginalInformationDir").toString())
{
    QDir().mkpath(_sourceDir);
    QDir().mkpath(_originalDir);
}

std::tuple<QStringList, FileErrorList> ManagedFileService::append(const QList<TMimeEntity>& files, const bool& isCropImage)
{
    const auto basePath = QDir(_sourceDir).filePath( QDir::toNativeSeparators(QDateTime::currentDateTime().toString("yyyy/MMdd/")) + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()) );
    if (! QDir(basePath).exists()) {
        QDir(basePath).mkpath(".");
    }

    FileErrorList errors;
    QStringList success;

    const QTemporaryDir tempDir;
    const auto workPath = tempDir.path();
    for (const auto& f : files) {
        errors << extract(f, workPath);
    }

    // cache the all hashes.
    QStringList caches;
    QDirIterator i(_sourceDir, {"*.jpg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (i.hasNext()) {
        const QString path = i.next();
        caches << QFileInfo(path).completeBaseName();
    }

    QDirIterator search(workPath, {"*.jpg", "*.jpeg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (search.hasNext()) {
        const OriginalFile original(search.next());
        // 移動先の sources ディレクトリ (オリジナル画像置き場) から重複を検索
        const bool duplicated = caches.contains(original.hash);
        // 適切な場所に配置
        FileError::Type errorKey = FileError::Type::NOERR;
        if (! duplicated) {
            const QString destination = QDir(basePath).filePath(original.hash + ".jpg");

            if (isCropImage) {
                const QRect frame = original.image.getValidRect();
                const auto size = std::min(frame.width(), frame.height());
                if (original.image.cropped(frame.x(), frame.y(), size, size).save(destination)) {
                    success << QFileInfo(destination).absoluteFilePath();
                    original.saveTo(_originalDir, destination);
                } else {
                    errorKey = FileError::Type::IMAGECROP;
                }
            }
            if ((! isCropImage) || (errorKey != FileError::Type::NOERR)) {
                if (QFile::rename(original.path, destination)) {
                    success << QFileInfo(destination).absoluteFilePath();
                    original.saveTo(_originalDir, destination);
                } else {
                    errorKey = FileError::Type::UNKNOWN;
                }
            }
        }
        // 重複ファイルは警告
        else {
            errorKey = FileError::Type::DUPLICATED;
        }
        // エラーを記録
        if (errorKey != FileError::Type::NOERR) {
            errors << FileError(errorKey, QString(original.path).replace(workPath + QDir::separator(), ""));
        }
    }

    return std::make_tuple(success, errors);
}

QList<ManagedFile> ManagedFileService::find(const QString& word)
{
    QList<ManagedFile> images;

    QDirIterator search(ManagedFileService()._originalDir, {"*.txt"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (search.hasNext()) {
        const ManagedFile file = ManagedFile::fromConfig(search.next());
        if (file.name().contains(word, Qt::CaseInsensitive)) {
            images << file;
        }
    }

    return images;
}

QList<ManagedFile> ManagedFileService::findInDirectory(const QString& word, const QString& path)
{
    QList<ManagedFile> images;

    QDirIterator search(path, {"*.jpg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (search.hasNext()) {
        const QString hash = QFileInfo(search.next()).completeBaseName();
        ManagedFile file = ManagedFile::fromHash(hash);
        if (file.name().contains(word, Qt::CaseInsensitive)) {
            images << file;
        }
    }

    return images;
}

QList<ManagedFile> ManagedFileService::findInList(const QString& word, const QStringList& list)
{
    QList<ManagedFile> images;

    for (const QString& link : list) {
        const QString hash = QFileInfo(link).completeBaseName();
        ManagedFile file = ManagedFile::fromHash(hash);
        if (file.name().contains(word, Qt::CaseInsensitive)) {
            images << file;
        }
    }

    return images;
}

QStringList ManagedFileService::filterInList(const QString& word, const QStringList& list)
{
    QStringList images;

    for (const QString& link : list) {
        const QString hash = QFileInfo(link).completeBaseName();
        ManagedFile file = ManagedFile::fromHash(hash);
        if (file.name().contains(word, Qt::CaseInsensitive)) {
            images << link;
        }
    }

    return images;
}

QByteArray ManagedFileService::checksum(const QString& path, QCryptographicHash::Algorithm algorithm)
{
    QFile file(path);
    if (file.open(QFile::ReadOnly)) {
        QCryptographicHash hash(algorithm);
        if (hash.addData(&file)) {
            return hash.result();
        }
    }
    return QByteArray();
}

FileErrorList ManagedFileService::extract(const TMimeEntity& entity, const QString& workPath)
{
    FileErrorList errors;

    QProcess inflator;
    const QString originalName = entity.originalFileName();
    const QString extension = QFileInfo(originalName).completeSuffix();
    const QString contentType = entity.contentType();
    if ((extension == "zip") || (contentType == "application/zip") || (contentType == "application/x-zip-compressed")) {
        inflator.start("unzip", { entity.uploadedFilePath(), "-d", workPath });
        inflator.waitForFinished(-1);
    }
    else if ((extension == "tar") || (contentType == "application/x-tar")) {
        inflator.start("tar", { "xf", entity.uploadedFilePath(), "-C", workPath });
        inflator.waitForFinished(-1);
    }
    else if ((extension == "tar.gz") || (contentType == "application/x-gzip") || (contentType == "application/gzip")) {
        inflator.start("tar", { "xf", entity.uploadedFilePath(), "-C", workPath });
        inflator.waitForFinished(-1);
    }
    else if (contentType == "image/jpeg") {
        const_cast<TMimeEntity*>(&entity)->renameUploadedFile(QDir(workPath).filePath(originalName));
    }
    else {
        tDebug() << "Not processed: [" << contentType << "] " << entity.uploadedFilePath() << "/" << originalName;
        errors << FileError(FileError::Type::INVALIDCONTENT, QString(entity.uploadedFilePath()).replace(workPath + QDir::separator(), ""));
    }

    return errors;
}