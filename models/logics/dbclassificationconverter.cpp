#include "logics/dbclassificationconverter.h"
#include "logics/dbclassificationconvertoperation.h"

REGISTER_DBCONVERTER(Classification, DbClassificationConverter);


DbClassificationConverter::DbClassificationConverter(const CaffeData::Category& layout, const QString& sourceDir)
    : DbConverter(layout, sourceDir)
{
}

void
DbClassificationConverter::convertTo(const QString& destinationDir)
{
    // environment prepare
    QDir(destinationDir).mkpath(name());
    const QString destinationNamedDir = QDir(destinationDir).filePath(name());

    // save labels information (labels.txt)
    saveLabelNamesTo(QDir(destinationNamedDir).filePath("labels.txt"));

    // execute convert operation
    for (const auto role : { DbConvertOperation::Role::Train, DbConvertOperation::Role::Val }) {
        DbClassificationConvertOperation operation(role);
        operation.execute(*this, sourceDir(), destinationNamedDir);
    }

    // generate image mean file if needed.
    saveImageMean(destinationNamedDir);
}

void
DbClassificationConverter::saveLabelNamesTo(const QString& path) const
{
    if (labelNames().count() > 0) {
        QFile info(path);
        if (info.open(QFile::WriteOnly)) {
            QTextStream stream(&info);
            for (const auto label : labelNames()) {
                stream << label << "\n";
            }
            info.close();
        }
    }
}

void
DbClassificationConverter::saveImageMean(const QString& path)
{
    if (isGenerateImageMean()) {
        const QString caffeRoot = QProcessEnvironment::systemEnvironment().value("CAFFE_ROOT");;
        const QString cmdPath = QDir(caffeRoot).filePath("build/tools/compute_image_mean");
        if (QFileInfo(cmdPath).isExecutable()) {
            const QString dbPath = QDir(path).filePath(DbConvertOperation::roleName(DbConvertOperation::Role::Train) + "_db");
            const QString meanPath = QDir(path).filePath("mean.binaryproto");

            QProcess generator;
            generator.start(cmdPath, { "-backend", backendName(), dbPath, meanPath });
            generator.waitForFinished(-1);
        }
    }
}
