#include "logics/sourcereference.h"
#include <TWebApplication>
#include <QtConcurrent>

/**
 */
namespace {
    /**
     */
    static const QDir kSourceReferenceDir(Tf::conf("settings").value("SourceReferencesDir").toString());

    /**
     */
    struct SourceReferenceInitializer {
        SourceReferenceInitializer() {
            if (! kSourceReferenceDir.exists()) {
                kSourceReferenceDir.mkpath(".");
                QtConcurrent::run([=]{
                    QDirIterator i(Tf::conf("settings").value("OriginalImagesDir").toString(), {"*.jpg", "*.jpeg"}, QDir::Files | QDir::Readable, QDirIterator::Subdirectories);
                    while (i.hasNext()) {
                        const QString name = QFileInfo(i.next()).fileName();
                        if (! kSourceReferenceDir.exists(name)) {
                            QFile::link(i.fileInfo().absoluteFilePath(), kSourceReferenceDir.absoluteFilePath(name));
                        }
                    }
                });
            }

            const QString from = kSourceReferenceDir.absolutePath();
            const QString to = QDir(Tf::app()->publicPath()).absoluteFilePath(QFileInfo(from).fileName());
            QFile::link(from, to);
        }
    } SourceReferenceInitializer;
}

/**
 */
bool
MakeReferenceFromSourcePath(const QString& sourcePath)
{
    const QFileInfo file(sourcePath);
    if (file.exists()) {
        return QFile::link(file.absoluteFilePath(), kSourceReferenceDir.absoluteFilePath(file.fileName()));
    }
    return false;
}

/**
 */
QString
GetAbsolutePathFromSourceName(const QString& fileName)
{
    if (kSourceReferenceDir.exists(fileName)) {
        return QDir(kSourceReferenceDir.filePath(fileName)).canonicalPath();
    }
    return "";
}

/**
 */
QString
GetPublicPathFromSourceName(const QString& fileName)
{
    if (kSourceReferenceDir.exists(fileName)) {
        return QString("/%1/%2").arg(kSourceReferenceDir.dirName()).arg(fileName);
    }
    return "";
}
