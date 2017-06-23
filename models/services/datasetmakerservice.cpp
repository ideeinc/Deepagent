#include "services/datasetmakerservice.h"
#include "services/taggroup.h"
#include "image.h"
#include <TWebApplication>
#include <THttpRequest>
#include <TSession>
#include <QtConcurrent>
#include <functional>
#include <random>
#include <algorithm>


class DatasetStatistics {
public:
    QMap<QString, QVariant> originalTotal;
    QMap<QString, QVariant> originalTraining;
    QMap<QString, QVariant> originalValidation;
    QMap<QString, QVariant> augmentedTraining;
    QMap<QString, QVariant> augmentedValidation;
    int augmentationRate{0};
    float validationRate{0};

    void saveTo(const QString& dir, const QString& filename)
    {
        QFile file(QDir(dir).filePath(filename));
        if (file.open(QFile::WriteOnly)) {
            QVariantMap data{
                { "created", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") },
                { "rate", QVariantMap{
                    { "validation", validationRate },
                    { "augmentation", augmentationRate }
                } },
                { "original",QVariantMap{
                    { "total", originalTotal },
                    { "training", originalTraining },
                    { "validation", originalValidation }
                 } },
                { "augmented", QVariantMap{
                    { "training", augmentedTraining },
                    { "validation", augmentedValidation }
                } }
            };
            file.write(QJsonDocument::fromVariant(data).toJson());
            file.close();
        }
    }
};


namespace {
    uint rand(uint min, uint max)
    {
        static std::random_device randev;
        static std::mt19937 mt(randev());
        static QMutex mutex;

        mutex.lock();
        std::uniform_int_distribution<uint> uniform(min, max);
        uint ret = uniform(mt);
        mutex.unlock();
        return ret;
    }
    float randf(float min, float max)
    {
        const float num = rand(0, UINT_MAX) / static_cast<float>(UINT_MAX); // 0.0 - 1.0
        return std::abs(max - min) * num + qMin(max, min);
    }
    void copyAugmentedImage(const QDir& sourceDir, const QDir& destinationDir, const QSize& size, const long multiple)
    {
        const float rotationRange = 180.0;
        const float zoomRange = 0.2;

        for (const QFileInfo& info : sourceDir.entryInfoList({"*.jpg"})) {
            Image original(info.absoluteFilePath());
            for (long i = 0; i < multiple; ++i) {
                Image clone;
                if (i > 0) {
                    const float degree = randf(-rotationRange, rotationRange);
                    float zoom = randf(0, zoomRange);
                    zoom = (zoom >= 0.0) ? 1.0+zoom : 1.0/(1.0+std::abs(zoom));
                    clone = original.rotated(degree, zoom);
                }
                else {
                    clone = original.clone();
                }
                clone.resize(size.width(), size.height());
                const QString destinationName = info.completeBaseName() + "_" + QString::number(i) + "." + info.suffix();
                clone.save(destinationDir.absoluteFilePath(destinationName));
            }
        }
    }
}
namespace {
    //
    QString rotateLabelFromLineBuffer(const QString &lineBuffer, const float angle, const QPointF &center)
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
    void copyRotatedImageAndLabel(const QString &sourceImagePath, const QString &destinationImagePath, const QString &sourceLabelPath, const QString &destinationLabelPath, const float angle)
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
    //
    QString scaleLabelFromLineBuffer(const QString &lineBuffer, const QPointF& scale, const QString& alternateLabel = "")
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
    void copyScaledImageAndLabel(const QString &sourceImagePath, const QString &destinationImagePath, const QString &sourceLabelPath, const QString &destinationLabelPath, const QSize& size, const QString& alternateLabel = "")
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
}
namespace {
    //
    QDir makeLayoutSource(const QString& workDir, const QMap<QString, QStringList>& labeledImages, const float validationRate)
    {
        const QDir archiveDir(workDir);
        const QDir sourceDir(archiveDir.absoluteFilePath("source")); archiveDir.mkpath("source");
        const QDir trainingDir(sourceDir.filePath("train")); sourceDir.mkpath("train");
        const QDir validationDir(sourceDir.filePath("val")); sourceDir.mkpath("val");

        // make source data
        for (const QString& label : labeledImages.keys()) {
            // training
            trainingDir.mkpath(label);
            const QDir trainingLabelDir(trainingDir.filePath(label));
            for (const QString& imagePath : labeledImages[ label ]) {
                const QString fileName = QFileInfo(imagePath).fileName();
                QFile::link(imagePath, trainingLabelDir.filePath(fileName));
            }

            // validation
            if ((0 < validationRate) && (validationRate < 1)) {
                validationDir.mkpath(label);
                const QDir validationLabelDir(validationDir.filePath(label));

                QFileInfoList images = trainingLabelDir.entryInfoList({"*.jpg"});
                std::shuffle(images.begin(), images.end(), std::mt19937());

                const int validationCount = ((images.count() > 1) ? std::max(images.count() * validationRate, 1.0f) : 0);
                for (int i = 0; i < validationCount; ++i) {
                    const QFileInfo& info = images.takeFirst();
                    QFile::rename(info.absoluteFilePath(), validationLabelDir.absoluteFilePath(info.fileName()));
                }
            }
        }

        return sourceDir;
    }
    //
    void layoutByClassification(const QString& workDir, const QMap<QString, QStringList>& labeledImages, const QSize& size, const long augmentationRate, const float validationRate)
    {
        const QDir archiveDir(workDir);
        const QDir trainDir = QDir(archiveDir.absoluteFilePath("train"));
        const QDir sourceDir = makeLayoutSource(workDir, labeledImages, validationRate);

        // record statistics
        DatasetStatistics stat;

        // determine augmetation value (with source training images)
        long maxAugmentation = 1;
        QMap<QString, float> trainingRates;
        if (augmentationRate > 0) {
            for (const QString& label : labeledImages.keys()) {
                const QDir sourceLabelDir(sourceDir.filePath("train"));
                const QDir currentLabelDir(sourceLabelDir.filePath(label));
                maxAugmentation = std::max(maxAugmentation, currentLabelDir.entryList({"*.jpg"}).count() * augmentationRate);
            }
            stat.augmentationRate = augmentationRate;
        }

        // make training data
        for (const QString& label : labeledImages.keys()) {
            trainDir.mkpath(label);
            const QDir currentSourceDir(QDir(sourceDir.filePath("train")).filePath(label));
            const int numberOfImages = currentSourceDir.entryList({"*.jpg"}).count();
            const float currentRate =(numberOfImages > 0) ? std::roundf(static_cast<float>(maxAugmentation) / static_cast<float>(numberOfImages)) : 0;
            const QDir trainTargetDir(trainDir.filePath(label));
            copyAugmentedImage(currentSourceDir, trainTargetDir, size, currentRate);

            trainingRates[label] = currentRate;
            stat.originalTotal[label] = labeledImages[label].count();
            stat.originalTraining[label] = numberOfImages;
            stat.augmentedTraining[label] = trainTargetDir.entryList({"*.jpg"}).count();
        }

        // make validation data
        if ((0 < validationRate) && (validationRate < 1)) {
            const QDir valDir(archiveDir.absoluteFilePath("val"));
            for (const QString& label : labeledImages.keys()) {
                valDir.mkpath(label);
                const QDir currentSourceDir(QDir(sourceDir.filePath("val")).filePath(label));
                const float currentRate = trainingRates[label];
                const QDir valTargetDir(valDir.filePath(label));
                copyAugmentedImage(currentSourceDir, valTargetDir, size, currentRate);

                stat.originalValidation[label] = currentSourceDir.entryList({"*.jpg"}).count();
                stat.augmentedValidation[label] = valTargetDir.entryList({"*.jpg"}).count();
            }
            stat.validationRate = validationRate;
        }

        // save statistics
        stat.saveTo(workDir, "info.json");
        // clean up
        QDir(sourceDir).removeRecursively();
    }
    void layoutByObjectDetection(const QString& workDir, const QMap<QString, QStringList>& labeledImages, const QSize& size, const long augmentationRate, const float validationRate)
    {
        const QDir archiveDir(workDir);
        archiveDir.mkpath(QString("train") + QDir::separator() + "images");
        archiveDir.mkpath(QString("train") + QDir::separator() + "labels");
        const QDir trainImageDir = QDir(archiveDir.absoluteFilePath("train") + QDir::separator() + "images");
        const QDir trainLabelDir = QDir(archiveDir.absoluteFilePath("train") + QDir::separator() + "labels");

        const QDir sourceDir = makeLayoutSource(workDir, labeledImages, validationRate);
        const QDir sourceTrainDir(sourceDir.filePath("train"));
        const QDir sourceValDir(sourceDir.filePath("val"));

        // record statistics
        DatasetStatistics stat;

        if (augmentationRate > 0) {
            stat.augmentationRate = 4;
        }

        // make training data
        const QDir labelDir(Tf::conf("settings").value("ObjectDetectionLabelDir").toString());
        for (const QString& label : labeledImages.keys()) {
            const QFileInfoList images = QDir(sourceTrainDir.filePath(label)).entryInfoList({"*.jpg"});
            for (const QFileInfo& originalImage : images) {
                const QString labelFileName = originalImage.completeBaseName() + ".txt";
                if (labelDir.exists(labelFileName)) {
                    const QFileInfo trainImage = trainImageDir.filePath(originalImage.fileName());
                    const QFileInfo originalLabel = labelDir.filePath(labelFileName);
                    const QFileInfo trainLabel = trainLabelDir.filePath(labelFileName);
                    // copy original image to destination
                    copyScaledImageAndLabel(
                        originalImage.absoluteFilePath(), trainImage.absoluteFilePath(),
                        originalLabel.absoluteFilePath(), trainLabel.absoluteFilePath(),
                        size, label
                    );
                    stat.originalTraining[label] = (stat.originalTraining[label].toInt() + 1);
                    stat.originalTotal[label] = (stat.originalTotal[label].toInt() + 1);

                    // generate augmented images and labels
                    if (augmentationRate > 0) {
                        for (const QString& angle : QStringList({"90", "180", "270"})) {
                            const QString dstImagePath = trainImageDir.filePath(trainImage.completeBaseName() + "_" + angle + ".jpg");
                            const QString dstLabelPath = trainLabelDir.filePath(trainLabel.completeBaseName() + "_" + angle + ".txt");
                            copyRotatedImageAndLabel(
                                trainImage.absoluteFilePath(), dstImagePath,
                                trainLabel.absoluteFilePath(), dstLabelPath,
                                angle.toFloat()
                            );
                        }
                        stat.augmentedTraining[label] = (stat.augmentedTraining[label].toInt() + 4);
                    }
                }
            }
        }

        // make validation data
        if ((0 < validationRate) && (validationRate < 1)) {
            archiveDir.mkpath(QString("val") + QDir::separator() + "images");
            archiveDir.mkpath(QString("val") + QDir::separator() + "labels");
            const QDir valImageDir = QDir(archiveDir.absoluteFilePath("val") + QDir::separator() + "images");
            const QDir valLabelDir = QDir(archiveDir.absoluteFilePath("val") + QDir::separator() + "labels");

            for (const QString& label : labeledImages.keys()) {
                const QFileInfoList images = QDir(sourceValDir.filePath(label)).entryInfoList({"*.jpg"});
                for (const QFileInfo& originalImage : images) {
                    const QString labelFileName = originalImage.completeBaseName() + ".txt";
                    if (labelDir.exists(labelFileName)) {
                        const QFileInfo valImage = valImageDir.filePath(originalImage.fileName());
                        const QFileInfo originalLabel = labelDir.filePath(labelFileName);
                        const QFileInfo valLabel = valLabelDir.filePath(labelFileName);
                        // copy original image to destination
                        copyScaledImageAndLabel(
                            originalImage.absoluteFilePath(), valImage.absoluteFilePath(),
                            originalLabel.absoluteFilePath(), valLabel.absoluteFilePath(),
                            size, label
                        );
                        stat.originalValidation[label] = (stat.originalValidation[label].toInt() + 1);
                        stat.originalTotal[label] = (stat.originalTotal[label].toInt() + 1);

                        // generate augmented images and labels
                        if (augmentationRate > 0) {
                            for (const QString& angle : QStringList({"90", "180", "270"})) {
                                const QString dstImagePath = valImageDir.filePath(valImage.completeBaseName() + "_" + angle + ".jpg");
                                const QString dstLabelPath = valLabelDir.filePath(valLabel.completeBaseName() + "_" + angle + ".txt");
                                copyRotatedImageAndLabel(
                                    valImage.absoluteFilePath(), dstImagePath,
                                    valLabel.absoluteFilePath(), dstLabelPath,
                                    angle.toFloat()
                                );
                            }
                            stat.augmentedValidation[label] = (stat.augmentedValidation[label].toInt() + 4);
                        }
                    }
                }
            }
            stat.validationRate = validationRate;
        }

        // save statistics
        stat.saveTo(workDir, "info.json");
        // clean up
        QDir(sourceDir).removeRecursively();
    }
    //
    QString archiveByTar(const QString& sourceDir, const QString& destinationDir, const QString& destinationName)
    {
        QDir().mkpath(destinationDir);
        const QString destinationPath = QDir(destinationDir).absoluteFilePath(destinationName + ".tar.gz");

        QProcess tar;
        tar.start("tar", {"czfh", destinationPath, "-C", QFileInfo(sourceDir).dir().absolutePath(), destinationName});
        tar.waitForFinished(-1);

        return destinationPath;
    }
    QString archiveByLMDB(const QString& sourceDir, const QString& destinationDir, const QString& destinationName)
    {
        QDir().mkpath(destinationDir);
        const QString destinationPath = QDir(destinationDir).absoluteFilePath(destinationName);

        /* TODO: currently just a simple move operation */
        QFile::rename(sourceDir, destinationPath);

        return destinationPath;
    }
}
namespace {
    void combination_internal(const long level, const QList<TagGroup>& sourceGroups, const QList<Tag>& tags, const std::function<void(const QList<Tag>)>& callback)
    {
        const TagGroup currentGroup = sourceGroups[ level ];
        for (const Tag& target : currentGroup.tags()) {
            QList<Tag> currentTags(tags);
            currentTags << target;
            const long nextLevel = (level + 1);
            if (nextLevel < sourceGroups.count()) {
                combination_internal(nextLevel, sourceGroups, currentTags, callback);
            }
            else {
                callback(currentTags);
            }
        }
    }
    void combination(const QList<TagGroup>& sourceGroups, const std::function<void(const QList<Tag>)>& callback)
    {
        if (sourceGroups.count() > 0) {
            QList<Tag> empty;
            combination_internal(0, sourceGroups, empty, callback);
        }
    }
}


DatasetMakerIndexContainer DatasetMakerService::index()
{
    DatasetMakerIndexContainer container;

    const QString dirPath(QDir(Tf::app()->configPath()).filePath("datasetmaker"));
    QFile file( QDir(dirPath).absoluteFilePath("default.json") );
    if (file.open(QFile::ReadOnly)) {
        QJsonObject setting = QJsonDocument::fromJson(file.readAll()).object();
        for (const auto& v : setting.value("excludes").toArray()) {
            QJsonObject entry = v.toObject();
            for (const auto& k : entry.keys()) {
                container.excludes << TagGroup(k).tag(entry.value(k).toString());
            }
        }
        file.close();
    }

    const QDir archiveDir(Tf::conf("settings").value("DatasetArchivesDir").toString());
    for (const QString& archive : archiveDir.entryList({"*.tar.gz"})) {
        container.archives << archive;
    }

    return container;
}

DatasetMakerPreviewContainer DatasetMakerService::preview(THttpRequest& request)
{
    DatasetMakerPreviewContainer container;

    QMap<QString, QSet<QString>> cache;
    QList<TagGroup> required;
    for (const auto& group : request.formItemList("groups")) {
        const TagGroup g(group);
        required << g;
        for (const Tag& t : g.tags()) {
            cache[ g.name() + "_" + t.name() ] = t.imagePaths().toSet();
        }
    }

    QSet<QString> excludes;
    for (const auto& info : request.formItemList("excludes")) {
        const QVariantMap m = QJsonDocument::fromJson(info.toUtf8()).object().toVariantMap();
        for (const auto& k : m.keys()) {
            excludes += TagGroup(k).tag(m.value(k).toString()).imagePaths().toSet();
        }
    }

    combination(required, [&](const QList<Tag> list){
        QString name, displayName;
        QSet<QString> images;
        for (long i = 0; i < list.count(); ++i) {
            const Tag& t = list[i];
            const QString cacheIdentifier = t.groupName() + "_" + t.name();
            if (i == 0)
                images = cache[ cacheIdentifier ];
            else
                images &= cache[ cacheIdentifier ];

            if (! name.isEmpty())
                name += "_";
            name += t.name();
            if (! displayName.isEmpty())
                displayName += " x ";
            displayName += t.displayName();
        }
        if (! excludes.isEmpty()) {
            images -= excludes;
        }
        if (! images.isEmpty()) {
            container.list.append(QVariantMap{{ "images", QVariant(images.toList()) }, { "name", name }, { "displayName", displayName }});
        }
    });

    return container;
}

DatasetMakerResultContainer DatasetMakerService::make(THttpRequest& request, const QVariantList& originalList, const QString& archiveName)
{
    DatasetMakerResultContainer container;
    QMap<QString, QStringList> sources;

    const QVariantMap labelNames = request.formItems("labelNames");
    const QVariantMap includes = request.formItems("includes");
    for (const QVariant& k : includes.values()) {
        const QString identifier = k.toString();
        const QString labelName = labelNames[ identifier ].toString();
        for (const QVariant& l : originalList) {
            const QVariantMap m = l.toMap();
            if (m["name"] == identifier) {
                sources[ labelName ] << m["images"].toStringList();
                break;
            }
        }
    }

    const QTemporaryDir workDir;
    const QString datasetName = (archiveName.isEmpty() ? request.formItemValue("datasetName") : archiveName);
    const QString workPath = QDir(workDir.path()).filePath(datasetName); QDir().mkpath(workPath);

    const long augmentationRate = request.formItemValue("augmentationRate").toLong();
    const float validationRate = request.formItemValue("validationRate").toFloat() / 100.0;
    const QSize size = request.hasFormItem("imageSize") ? QSize(request.formItems("imageSize")["w"].toInt(), request.formItems("imageSize")["h"].toInt()) : QSize(448, 448);

    typedef std::function<void(const QString&, const QMap<QString, QStringList>&, const QSize&, const long, const float)> LayoutDescriptor;
    const QMap<QString, LayoutDescriptor> layoutMaps = {{"classification", layoutByClassification}, {"detection", layoutByObjectDetection}};
    const QString layoutType = request.formItemValue("layoutType");
    LayoutDescriptor layoutor = layoutMaps[ layoutType ];
    if (layoutor != nullptr) {
        layoutor(workPath, sources, size, augmentationRate, validationRate);
    }

    typedef std::function<QString(const QString&, const QString&, const QString&)> ArchiveDescriptor;
    const QMap<QString, ArchiveDescriptor> archiveMaps = {{"tar", archiveByTar}, {"lmdb", archiveByLMDB}};
    const QString archiveType = request.formItemValue("archiveType");
    ArchiveDescriptor archiver = archiveMaps[ archiveType ];
    if (archiver != nullptr) {
        const QString archiveDir(Tf::conf("settings").value("DatasetArchivesDir").toString());
        container.path = archiver(workPath, archiveDir, datasetName);
    }

    return container;
}

QVariantMap DatasetMakerService::check(TSession& session, const QString& action)
{
    QVariantMap response;

    const QString path = QDir(Tf::app()->tmpPath()).filePath(session.value(action + "Result").toString());
    response["status"] = QFileInfo(path).exists();
    response["file"] = path;

    return response;
}

void DatasetMakerService::previewAsync(THttpRequest& request, TSession& session)
{
    const QString oldPreviewName = session.value("previewResult").toString();
    if (QFileInfo( QDir(Tf::app()->tmpPath()).filePath(oldPreviewName) ).exists()) {
        QFile::remove( QDir(Tf::app()->tmpPath()).filePath(oldPreviewName) );
    }

    session.remove("previewResult");
    const QString previewName("datasetpreview_" + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));

    QtConcurrent::run([=]{
        THttpRequest r(request);

        const DatasetMakerPreviewContainer container = preview(r);
        const QJsonDocument json = QJsonDocument::fromVariant(container.list);

        QFile file( QDir(Tf::app()->tmpPath()).filePath(previewName) );
        if (file.open(QFile::WriteOnly)) {
            file.write(json.toJson());
            file.close();
        }
    });

    session.insert("previewResult", previewName);
}

DatasetMakerPreviewContainer DatasetMakerService::previewResult(TSession& session)
{
    DatasetMakerPreviewContainer container;

    const QString path = QDir(Tf::app()->tmpPath()).filePath(session.value("previewResult").toString());
    QFile file(path);
    if (file.open(QFile::ReadOnly)) {
        container.list = QJsonDocument::fromJson(file.readAll()).array().toVariantList();
        file.close();

        // session.remove("previewResult");
        // file.remove();
    }

    return container;
}

void DatasetMakerService::makeAsync(THttpRequest& request, TSession& session)
{
    session.remove("makeResult");
    const QString archiveName = (request.hasFormItem("datasetName") ?  request.formItemValue("datasetName") : "dataset_" + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));

    const DatasetMakerPreviewContainer result = previewResult(session);
    QtConcurrent::run([=]{
        THttpRequest r(request);
        const DatasetMakerResultContainer container = make(r, result.list, archiveName);

        QFile file(QDir(Tf::app()->tmpPath()).filePath(archiveName));
        if (file.open(QFile::WriteOnly)) {
            file.write(container.path.toUtf8());
            file.close();
        }
    });

    session.insert("makeResult", archiveName);
}

DatasetMakerResultContainer DatasetMakerService::makeResult(TSession& session)
{
    DatasetMakerResultContainer container;

    const QString path = QDir(Tf::app()->tmpPath()).filePath(session.value("makeResult").toString());
    QFile file(path);
    if (file.open(QFile::ReadOnly)) {
        container.path = file.readAll();
        file.close();

        file.remove();
        session.remove("makeResult");
    }

    return container;
}

QString DatasetMakerService::archivePath(const QString& archiveName)
{
    QString path;

    const QDir archiveDir(Tf::conf("settings").value("DatasetArchivesDir").toString());
    if (archiveDir.exists(archiveName)) {
        path = archiveDir.filePath(archiveName);
    }

    return path;
}

bool DatasetMakerService::removeArchive(const QString& archiveName)
{
    bool success = false;

    const QDir archiveDir(Tf::conf("settings").value("DatasetArchivesDir").toString());
    if (archiveDir.exists(archiveName)) {
        success = QDir(archiveDir).remove(archiveName);
    }

    return success;
}
