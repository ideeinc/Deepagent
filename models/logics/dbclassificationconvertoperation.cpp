#include "logics/dbclassificationconvertoperation.h"
#include "logics/dbconverter.h"
#include <TGlobal>


void
DbClassificationConvertOperation::execute(const DbConverter& converter, const QString& sourceDir, const QString& destinationDir)
{
    const QString role(roleName());

    // save label mapping file (like 'train.txt' or 'val.txt')
    const QString labelPath = QDir(destinationDir).filePath(role + ".txt");
    QFile labelFile(labelPath);
    if (labelFile.open(QFile::WriteOnly)) {
        QTextStream stream(&labelFile);
        QDirIterator i(QDir(sourceDir).filePath(role), {"*.jpg"}, QDir::Files|QDir::Readable|QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (i.hasNext()) {
            const QString path = i.next();
            const QString label = QFileInfo(path).dir().dirName();
            const int index = converter.labelNames().indexOf(label);
            if (index > -1) {
                stream << QDir(sourceDir).relativeFilePath(path) << " " << index << "\n";
            }
        }
        labelFile.close();
    }

    const QString caffeRoot = QProcessEnvironment::systemEnvironment().value("CAFFE_ROOT");;
    const QString cmdPath = QDir(caffeRoot).filePath("build/tools/convert_imageset");

    if (! QFileInfo(cmdPath).isExecutable()) {
        tDebug() << "command: '" << cmdPath << "' is not executable.";
        return;
    }

    // run database creation tool
    QProcess dbImporter;
    dbImporter.setProcessChannelMode(QProcess::MergedChannels);
    dbImporter.setStandardOutputFile(QDir(destinationDir).filePath("create_" + role + "_db.log"));
    dbImporter.start(cmdPath, {
        sourceDir + QDir::separator(),                      // image source directory path (needs trailing slash)
        labelPath,                                          // label map file path
        QDir(destinationDir).filePath(role + "_db"),        // database output path
        (converter.needsShuffle() ? "1" : "0"),             // needs shuffle (1 to shuffle)
        "-backend", converter.backendName(),                // backend database type (lmdb or leveldb)
        QString::number(converter.imageSize().height()),    // image height
        QString::number(converter.imageSize().width()),     // image width
    });
    dbImporter.waitForFinished(-1);

    tDebug() << "CLASSIFICATION DB IMPORT-OUT (" << role << "): " << QString(dbImporter.readAllStandardOutput());
    tDebug() << "CLASSIFICATION DB IMPORT-ERR (" << role << "): " << QString(dbImporter.readAllStandardError());
}
