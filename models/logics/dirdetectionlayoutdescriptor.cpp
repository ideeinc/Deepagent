#include "logics/dirdetectionlayoutdescriptor.h"
#include "logics/formatconverter.h"
#include "image.h"
#include <TWebApplication>


void
DirDetectionLayoutDescriptor::layout(const QMap<QString, QStringList>& sourceImages, const QString& workPath)
{
    const QDir archiveDir(workPath);
    archiveDir.mkpath(QString("train") + QDir::separator() + "images");
    archiveDir.mkpath(QString("train") + QDir::separator() + "labels");
    archiveDir.mkpath(QString("train") + QDir::separator() + "xml");
    const QDir trainImageDir = QDir(archiveDir.absoluteFilePath("train") + QDir::separator() + "images");
    const QDir trainLabelDir = QDir(archiveDir.absoluteFilePath("train") + QDir::separator() + "labels");
    const QDir trainXmlDir = QDir(archiveDir.absoluteFilePath("train") + QDir::separator() + "xml");

    const QDir labelDir(Tf::conf("settings").value("ObjectDetectionLabelDir").toString());

    const QDir sourceDir = makeSourceLayout(sourceImages, workPath, [&labelDir](const QString& path) {
        const QString labelFileName = QFileInfo(path).completeBaseName() + ".txt";
        return labelDir.exists(labelFileName);
    });
    const QDir sourceTrainDir(sourceDir.filePath("train"));
    const QDir sourceValDir(sourceDir.filePath("val"));

    // record statistics
    DirLayoutDescriptor::Statistics stat;

    if (augmentationRate() > 0) {
        stat.augmentationRate = 4;
    }

    // make training data
    for (const QString& label : sourceImages.keys()) {
        const QFileInfoList images = QDir(sourceTrainDir.filePath(label)).entryInfoList({"*.jpg"});
        for (const QFileInfo& originalImage : images) {
            const QString labelFileName = originalImage.completeBaseName() + ".txt";
            if (labelDir.exists(labelFileName)) {
                const QFileInfo trainImage = trainImageDir.filePath(originalImage.fileName());
                const QFileInfo originalLabel = labelDir.filePath(labelFileName);
                const QFileInfo trainLabel = trainLabelDir.filePath(labelFileName);
                const QFileInfo trainXml = trainXmlDir.filePath(originalImage.completeBaseName() + ".xml");
                // copy original image to destination
                copyScaledImageAndLabel(
                    originalImage.absoluteFilePath(), trainImage.absoluteFilePath(),
                    originalLabel.absoluteFilePath(), trainLabel.absoluteFilePath(),
                    imageSize(), label
                );
                FormatConverter(
                    trainLabel.absoluteFilePath(), FormatConverter::Type::KITTI, trainImage.absoluteFilePath()
                ).convertTo(FormatConverter::Type::PASCAL_VOC, trainXml.absoluteFilePath());
                stat.originalTraining[label] = (stat.originalTraining[label].toInt() + 1);
                stat.originalTotal[label] = (stat.originalTotal[label].toInt() + 1);

                // generate augmented images and labels
                if (augmentationRate() > 0) {
                    for (const QString& angle : QStringList({"90", "180", "270"})) {
                        const QString dstImagePath = trainImageDir.filePath(trainImage.completeBaseName() + "_" + angle + ".jpg");
                        const QString dstLabelPath = trainLabelDir.filePath(trainLabel.completeBaseName() + "_" + angle + ".txt");
                        const QString dstXmlPath = trainXmlDir.filePath(trainXml.completeBaseName() + "_" + angle + ".xml");
                        copyRotatedImageAndLabel(
                            trainImage.absoluteFilePath(), dstImagePath,
                            trainLabel.absoluteFilePath(), dstLabelPath,
                            angle.toFloat()
                        );
                        FormatConverter(
                            dstLabelPath, FormatConverter::Type::KITTI, dstImagePath
                        ).convertTo(FormatConverter::Type::PASCAL_VOC, dstXmlPath);
                    }
                    stat.augmentedTraining[label] = (stat.augmentedTraining[label].toInt() + 4);
                }
            }
        }
    }

    // make validation data
    if ((0 < validationRate()) && (validationRate() < 1)) {
        archiveDir.mkpath(QString("val") + QDir::separator() + "images");
        archiveDir.mkpath(QString("val") + QDir::separator() + "labels");
        archiveDir.mkpath(QString("val") + QDir::separator() + "xml");
        const QDir valImageDir = QDir(archiveDir.absoluteFilePath("val") + QDir::separator() + "images");
        const QDir valLabelDir = QDir(archiveDir.absoluteFilePath("val") + QDir::separator() + "labels");
        const QDir valXmlDir = QDir(archiveDir.absoluteFilePath("val") + QDir::separator() + "xml");

        for (const QString& label : sourceImages.keys()) {
            const QFileInfoList images = QDir(sourceValDir.filePath(label)).entryInfoList({"*.jpg"});
            for (const QFileInfo& originalImage : images) {
                const QString labelFileName = originalImage.completeBaseName() + ".txt";
                if (labelDir.exists(labelFileName)) {
                    const QFileInfo valImage = valImageDir.filePath(originalImage.fileName());
                    const QFileInfo originalLabel = labelDir.filePath(labelFileName);
                    const QFileInfo valLabel = valLabelDir.filePath(labelFileName);
                    const QFileInfo valXml = valXmlDir.filePath(originalImage.completeBaseName() + ".xml");
                    // copy original image to destination
                    copyScaledImageAndLabel(
                        originalImage.absoluteFilePath(), valImage.absoluteFilePath(),
                        originalLabel.absoluteFilePath(), valLabel.absoluteFilePath(),
                        imageSize(), label
                    );
                    FormatConverter(
                        valLabel.absoluteFilePath(), FormatConverter::Type::KITTI, valImage.absoluteFilePath()
                    ).convertTo(FormatConverter::Type::PASCAL_VOC, valXml.absoluteFilePath());
                    stat.originalValidation[label] = (stat.originalValidation[label].toInt() + 1);
                    stat.originalTotal[label] = (stat.originalTotal[label].toInt() + 1);

                    // generate augmented images and labels
                    if (augmentationRate() > 0) {
                        for (const QString& angle : QStringList({"90", "180", "270"})) {
                            const QString dstImagePath = valImageDir.filePath(valImage.completeBaseName() + "_" + angle + ".jpg");
                            const QString dstLabelPath = valLabelDir.filePath(valLabel.completeBaseName() + "_" + angle + ".txt");
                            const QString dstXmlPath = valXmlDir.filePath(valXml.completeBaseName() + "_" + angle + ".xml");
                            copyRotatedImageAndLabel(
                                valImage.absoluteFilePath(), dstImagePath,
                                valLabel.absoluteFilePath(), dstLabelPath,
                                angle.toFloat()
                            );
                            FormatConverter(
                                dstLabelPath, FormatConverter::Type::KITTI, dstImagePath
                            ).convertTo(FormatConverter::Type::PASCAL_VOC, dstXmlPath);
                        }
                        stat.augmentedValidation[label] = (stat.augmentedValidation[label].toInt() + 4);
                    }
                }
            }
        }
        stat.validationRate = validationRate();
    }

    // save statistics
    stat.saveTo(workPath, "info.json");
    // clean up
    QDir(sourceDir).removeRecursively();
}

void
DirDetectionLayoutDescriptor::copyScaledImageAndLabel(const QString &sourceImagePath, const QString &destinationImagePath, const QString &sourceLabelPath, const QString &destinationLabelPath, const QSize& size, const QString& alternateLabel)
{
    Image srcImg(sourceImagePath);
    Image dstImg = srcImg.resized(size.width(), size.height());
    if (! dstImg.save(destinationImagePath)) {
        return;
    }
    const QPointF scale(
        static_cast<float>(size.width()) / static_cast<float>(srcImg.width()),
        static_cast<float>(size.height()) / static_cast<float>(srcImg.height())
    );

    QFile srcLabel(sourceLabelPath), dstLabel(destinationLabelPath);
    srcLabel.open(QIODevice::ReadOnly); dstLabel.open(QIODevice::WriteOnly);

    QTextStream reader(&srcLabel), writer(&dstLabel);
    while (! reader.atEnd()) {
        const QString line = reader.readLine();
        writer << scaleLabelFromLineBuffer(line, scale, alternateLabel) + "\n";
    }

    srcLabel.close(); dstLabel.close();
}

QString
DirDetectionLayoutDescriptor::scaleLabelFromLineBuffer(const QString &lineBuffer, const QPointF& scale, const QString& alternateLabel)
{
    const QStringList buffer = lineBuffer.split(' ');

    const QPointF srcTL(buffer[4].toFloat(), buffer[5].toFloat());
    const QPointF srcBR(buffer[6].toFloat(), buffer[7].toFloat());

    const QPointF dstTL( srcTL.x() * scale.x(), srcTL.y() * scale.y() );
    const QPointF dstBR( srcBR.x() * scale.x(), srcBR.y() * scale.y() );

    const float L = std::min({dstTL.x(), dstBR.x()});
    const float T = std::min({dstTL.y(), dstBR.y()});
    const float R = std::max({dstTL.x(), dstBR.x()});
    const float B = std::max({dstTL.y(), dstBR.y()});

    const QString label = (alternateLabel.isEmpty() ? buffer[0] : alternateLabel);
    QStringList output({
        label, buffer[1], buffer[2], buffer[3],
        QString::number(L, 'f', 2), QString::number(T, 'f', 2), QString::number(R, 'f', 2), QString::number(B, 'f', 2),
        buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14]
    });
    if (buffer.count() > 15) {
        output.append(buffer[15]);
    }
    return output.join(" ");
}

void
DirDetectionLayoutDescriptor::copyRotatedImageAndLabel(const QString &sourceImagePath, const QString &destinationImagePath, const QString &sourceLabelPath, const QString &destinationLabelPath, const float angle)
{
    Image srcImg(sourceImagePath);
    Image dstImg = srcImg.rotated(-angle, 1);
    if (! dstImg.save(destinationImagePath)) {
        return;
    }
    const QPointF center(srcImg.width() / 2.0, srcImg.height() / 2.0);

    QFile srcLabel(sourceLabelPath), dstLabel(destinationLabelPath);
    srcLabel.open(QIODevice::ReadOnly); dstLabel.open(QIODevice::WriteOnly);

    QTextStream reader(&srcLabel), writer(&dstLabel);
    while (! reader.atEnd()) {
        const QString line = reader.readLine();
        writer << rotateLabelFromLineBuffer(line, angle, center) + "\n";
    }

    srcLabel.close(); dstLabel.close();
}

QString
DirDetectionLayoutDescriptor::rotateLabelFromLineBuffer(const QString &lineBuffer, const float angle, const QPointF &center)
{
    const QStringList buffer = lineBuffer.split(' ');

    const QPointF srcTL(buffer[4].toFloat(), buffer[5].toFloat());
    const QPointF srcBR(buffer[6].toFloat(), buffer[7].toFloat());
    const float radian = (angle * M_PI) / 180.0;

    const QPointF dstTL(
        ((srcTL.x() - center.x()) * cos(radian)) - ((srcTL.y() - center.y()) * sin(radian)) + center.x(),
        ((srcTL.x() - center.x()) * sin(radian)) + ((srcTL.y() - center.y()) * cos(radian)) + center.y()
    );
    const QPointF dstBR(
        ((srcBR.x() - center.x()) * cos(radian)) - ((srcBR.y() - center.y()) * sin(radian)) + center.x(),
        ((srcBR.x() - center.x()) * sin(radian)) + ((srcBR.y() - center.y()) * cos(radian)) + center.y()
    );

    const float L = std::min({dstTL.x(), dstBR.x()});
    const float T = std::min({dstTL.y(), dstBR.y()});
    const float R = std::max({dstTL.x(), dstBR.x()});
    const float B = std::max({dstTL.y(), dstBR.y()});

    QStringList output({
        buffer[0], buffer[1], buffer[2], buffer[3],
        QString::number(L, 'f', 2), QString::number(T, 'f', 2), QString::number(R, 'f', 2), QString::number(B, 'f', 2),
        buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14]
    });
    if (buffer.count() > 15) {
        output.append(buffer[15]);
    }
    return output.join(" ");
}

REGISTER_LAYOUT_DESCRIPTOR(Detection, DirDetectionLayoutDescriptor);
