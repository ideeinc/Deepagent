#include "managedfilecontext.h"
#include "image.h"
#include "logics/sourcereference.h"
#include <TWebApplication>
#include <THttpRequest>

QSet<QString> ManagedFileContext::hashes;

// make symlink for `sources` to public
namespace {
    struct SourceDirMakePublicLink {
        SourceDirMakePublicLink() {
            const QString sourcePath = ManagedFileContext().sourceDir().absolutePath();
            const QString linkPath = QDir(Tf::app()->publicPath()).absoluteFilePath(QFileInfo(sourcePath).fileName());
            QFile::link(sourcePath, linkPath);
        };
    } SourceDirMakePublicLink;
};

namespace {
    typedef struct OriginalFile {
    public:
        QString path;
        QString name;
        QString hash;
        QSize size;
        Image image;

        OriginalFile(const QFileInfo& file)
         : path(file.absoluteFilePath()), name(file.fileName()), hash(QString(ManagedFileContext::checksum(path).toHex())), image(path) {
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
        FileError::Type saveTrimmingImage(const TrimmingMode& mode, const QString& destination) const
        {
            FileError::Type err = FileError::Type::NOERR;
            switch (mode) {
                case TrimmingMode::NoTrimming: {
                    if (! QFile::rename(path, destination)) {
                        err = FileError::Type::UNKNOWN;
                    }
                }
                    break;
                case TrimmingMode::Default: {
                    if (! image.trimmed().save(destination)) {
                        err = FileError::Type::IMAGECROP;
                    }
                }
                    break;
                case TrimmingMode::Square: {
                    const QRect frame = image.getValidRect();
                    const auto size = std::min(frame.width(), frame.height());
                    const float xMargin = (static_cast<float>(frame.width()) - size) / 2.0;
                    const float yMargin = (static_cast<float>(frame.height()) - size) / 2.0;
                    if (! image.cropped(frame.x() + xMargin, frame.y() + yMargin, size, size).save(destination)) {
                        err = FileError::Type::IMAGECROP;
                    }
                }
                    break;
            }
            return err;
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


ManagedFileContext::ManagedFileContext()
    : _sourceDir(Tf::conf("settings").value("OriginalImagesDir").toString()), _originalDir(Tf::conf("settings").value("OriginalInformationDir").toString())
{
    QDir().mkpath(_sourceDir);
    QDir().mkpath(_originalDir);
}

std::tuple<QStringList, FileErrorList> ManagedFileContext::append(const QList<TMimeEntity>& files, const AppendingOption& option)
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

    QDirIterator search(workPath, {"*.jpg", "*.jpeg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (search.hasNext()) {
        const OriginalFile original(search.next());
        // 移動先の sources ディレクトリ (オリジナル画像置き場) から重複を検索
        const bool duplicated = ManagedFileContext::hashes.contains(original.hash);
        // 適切な場所に配置
        FileError::Type errorKey = FileError::Type::NOERR;
        if (! duplicated) {
            const QString destination = QDir(basePath).filePath(original.hash + ".jpg");
            errorKey = original.saveTrimmingImage(option.trimmingMode, destination);
            if (errorKey == FileError::Type::NOERR) {
                const QString absoluteSourcePath = QFileInfo(destination).absoluteFilePath();
                success << absoluteSourcePath;
                MakeReferenceFromSourcePath(absoluteSourcePath);
                original.saveTo(_originalDir, destination);
            }
        }
        // 重複ファイルは警告
        else {
            if (option.duplicationMode == DuplicationMode::IncludeFiles) {
                success << GetAbsolutePathFromSourceName(original.hash + ".jpg");
            } else {
                errorKey = FileError::Type::DUPLICATED;
            }
        }
        // エラーを記録
        if (errorKey != FileError::Type::NOERR) {
            errors << FileError(errorKey, QString(original.path).replace(workPath + QDir::separator(), ""));
        } else {
            ManagedFileContext::hashes << original.hash;
        }
    }

    return std::make_tuple(success, errors);
}

QList<ManagedFile> ManagedFileContext::find(const QString& word)
{
    QList<ManagedFile> images;

    QDirIterator search(ManagedFileContext()._sourceDir, {"*.jpg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (search.hasNext()) {
        const ManagedFile file = ManagedFile::fromLink(search.next());
        if (file.name().contains(word, Qt::CaseInsensitive)) {
            images << file;
        }
    }

    return images;
}

QList<ManagedFile> ManagedFileContext::findInDirectory(const QString& word, const QString& path)
{
    QList<ManagedFile> images;

    QDirIterator search(path, {"*.jpg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (search.hasNext()) {
        ManagedFile file = ManagedFile::fromLink(search.next());
        if (file.name().contains(word, Qt::CaseInsensitive)) {
            images << file;
        }
    }

    return images;
}

QList<ManagedFile> ManagedFileContext::findInList(const QString& word, const QStringList& list)
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

QStringList ManagedFileContext::filterInList(const QString& word, const QStringList& list)
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

void ManagedFileContext::load()
{
    tDebug() << "ManagedFileContext::load";
    // cache the all hashes.
    QDirIterator i(ManagedFileContext()._sourceDir, {"*.jpg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (i.hasNext()) {
        const QString path = i.next();
        ManagedFileContext::hashes << QFileInfo(path).completeBaseName();
    }
}

QByteArray ManagedFileContext::checksum(const QString& path, QCryptographicHash::Algorithm algorithm)
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

FileErrorList ManagedFileContext::extract(const TMimeEntity& entity, const QString& workPath)
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

QDir ManagedFileContext::sourceDir() const
{
    return QDir(_sourceDir);
}

QDir ManagedFileContext::originalDir() const
{
    return QDir(_originalDir);
}
