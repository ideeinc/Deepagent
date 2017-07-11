#include "logics/compressarchiver.h"


QString
CompressArchiver::archive(const QString& sourceDir, const QString& destinationDir)
{
    QDir().mkpath(destinationDir);
    const QString destinationPath = QDir(destinationDir).absoluteFilePath(name() + ".tar.gz");

    QProcess tar;
    tar.start("tar", {"czfh", destinationPath, "-C", QFileInfo(sourceDir).dir().absolutePath(), name()});
    tar.waitForFinished(-1);

    return destinationPath;
}

REGISTER_ARCHIVER(Compress, CompressArchiver);
