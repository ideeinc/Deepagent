#include "logics/dirlayoutdescriptor.h"
#include <random>
#include <algorithm>


DirLayoutDescriptor::DirLayoutDescriptor()
{
}

DirLayoutDescriptor::~DirLayoutDescriptor()
{
}

QDir
DirLayoutDescriptor::makeSourceLayout(const QMap<QString, QStringList>& sourceImages, const QString& workPath, const std::function<bool(const QString&)>& filter)
{
    const QDir archiveDir(workPath);
    const QDir sourceDir(archiveDir.absoluteFilePath("source")); archiveDir.mkpath("source");
    const QDir trainingDir(sourceDir.filePath("train")); sourceDir.mkpath("train");
    const QDir validationDir(sourceDir.filePath("val")); sourceDir.mkpath("val");

    // make source data
    for (const QString& label : sourceImages.keys()) {
        // training
        trainingDir.mkpath(label);
        const QDir trainingLabelDir(trainingDir.filePath(label));
        for (const QString& imagePath : sourceImages[ label ]) {
            if ((filter == nullptr) || filter(imagePath)) {
                const QString fileName = QFileInfo(imagePath).fileName();
                QFile::link(imagePath, trainingLabelDir.filePath(fileName));
            }
        }

        // validation
        const float rate = validationRate();
        if ((0 < rate) && (rate < 1)) {
            validationDir.mkpath(label);
            const QDir validationLabelDir(validationDir.filePath(label));

            QFileInfoList images = trainingLabelDir.entryInfoList({"*.jpg"});
            std::shuffle(images.begin(), images.end(), std::mt19937());

            const int validationCount = ((images.count() > 1) ? std::max(images.count() * rate, 1.0f) : 0);
            for (int i = 0; i < validationCount; ++i) {
                const QFileInfo& info = images.takeFirst();
                QFile::rename(info.absoluteFilePath(), validationLabelDir.absoluteFilePath(info.fileName()));
            }
        }
    }

    return sourceDir;
}

int
DirLayoutDescriptor::augmentationRate() const
{
    return _augmentationRage;
}

void
DirLayoutDescriptor::setAugmentationRate(const int& rate)
{
    _augmentationRage = rate;
}

float
DirLayoutDescriptor::validationRate() const
{
    return _validationRate;
}

void
DirLayoutDescriptor::setValidationRate(const float& rate)
{
    _validationRate = rate;
}

QSize
DirLayoutDescriptor::imageSize() const
{
    return _imageSize;
}

void
DirLayoutDescriptor::setImageSize(const QSize& size)
{
    _imageSize = size;
}

void
DirLayoutDescriptor::Statistics::saveTo(const QString& dir, const QString& filename)
{
    QFile file(QDir(dir).filePath(filename));
    if (file.open(QFile::WriteOnly)) {
        QVariantMap data{
            { "created", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") },
            { "rate", QVariantMap{
                { "validation", QString::number(static_cast<int>(validationRate * 100)) + "%" },
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
