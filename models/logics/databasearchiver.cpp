#include "logics/databasearchiver.h"
#include "logics/dbconverter.h"
#include "dataset.h"


QString
DatabaseArchiver::archive(const QString& sourceDir, const QString& destinationDir)
{
    Dataset ds;
    ds.setImageWidth(imageSize().width());
    ds.setImageHeight(imageSize().height());
    ds.create();

    std::unique_ptr<DbConverter> db(DbConverter::create(layoutType(), sourceDir));
    db->setName(name());
    db->setImageSize(imageSize());
    db->setLabelNames(labelNames());
    db->convertTo(destinationDir);

    ds.setCompletedAt(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"));
    ds.update();

    QDirIterator dir(QDir(destinationDir).filePath(name()), QDir::Files|QDir::Dirs|QDir::Readable|QDir::NoDotAndDotDot);
    while (dir.hasNext()) {
        const QString src = dir.next();
        const QString dst = QDir(ds.dirPath()).filePath(QFileInfo(src).fileName());
        QFile::rename(src, dst);
    }
    QDir(destinationDir).rmdir(name());

    return QDir(destinationDir).absoluteFilePath(name());
}

REGISTER_ARCHIVER(Database, DatabaseArchiver);
