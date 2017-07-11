#include "dirclassificationlayoutdescriptor.h"
#include "image.h"
#include <algorithm>
#include <functional>
#include <random>


void
DirClassificationLayoutDescriptor::layout(const QMap<QString, QStringList>& sourceImages, const QString& workPath)
{
    const QDir archiveDir(workPath);
    const QDir trainDir = QDir(archiveDir.absoluteFilePath("train"));
    const QDir sourceDir = makeSourceLayout(sourceImages, workPath);

    // record statistics
    DirLayoutDescriptor::Statistics stat;

    // determine augmetation value (with source training images)
    int maxAugmentation = 1;
    QMap<QString, float> trainingRates;
    if (augmentationRate() > 0) {
        for (const QString& label : sourceImages.keys()) {
            const QDir sourceLabelDir(sourceDir.filePath("train"));
            const QDir currentLabelDir(sourceLabelDir.filePath(label));
            maxAugmentation = std::max(maxAugmentation, currentLabelDir.entryList({"*.jpg"}).count() * augmentationRate());
        }
        stat.augmentationRate = augmentationRate();
    }

    // make training data
    for (const QString& label : sourceImages.keys()) {
        trainDir.mkpath(label);
        const QDir currentSourceDir(QDir(sourceDir.filePath("train")).filePath(label));
        const int numberOfImages = currentSourceDir.entryList({"*.jpg"}).count();
        const float currentRate =(numberOfImages > 0) ? std::roundf(static_cast<float>(maxAugmentation) / static_cast<float>(numberOfImages)) : 0;
        const QDir trainTargetDir(trainDir.filePath(label));
        copyAugmentedImage(currentSourceDir, trainTargetDir, imageSize(), currentRate);

        trainingRates[label] = currentRate;
        stat.originalTotal[label] = sourceImages[label].count();
        stat.originalTraining[label] = numberOfImages;
        stat.augmentedTraining[label] = trainTargetDir.entryList({"*.jpg"}).count();
    }

    // make validation data
    if ((0 < validationRate()) && (validationRate() < 1)) {
        const QDir valDir(archiveDir.absoluteFilePath("val"));
        for (const QString& label : sourceImages.keys()) {
            valDir.mkpath(label);
            const QDir currentSourceDir(QDir(sourceDir.filePath("val")).filePath(label));
            const float currentRate = trainingRates[label];
            const QDir valTargetDir(valDir.filePath(label));
            copyAugmentedImage(currentSourceDir, valTargetDir, imageSize(), currentRate);

            stat.originalValidation[label] = currentSourceDir.entryList({"*.jpg"}).count();
            stat.augmentedValidation[label] = valTargetDir.entryList({"*.jpg"}).count();
        }
        stat.validationRate = validationRate();
    }

    // save statistics.
    stat.saveTo(workPath, "info.json");
    // clean up.
    QDir(sourceDir).removeRecursively();
}

void
DirClassificationLayoutDescriptor::copyAugmentedImage(const QDir& sourceDir, const QDir& destinationDir, const QSize& size, const long multiple)
{
    const std::function<uint(const uint, const uint)> rand = [](const uint min, const uint max){
        static std::random_device randev;
        static std::mt19937 mt(randev());
        static QMutex mutex;

        mutex.lock();
        std::uniform_int_distribution<uint> uniform(min, max);
        uint ret = uniform(mt);
        mutex.unlock();
        return ret;
    };
    const std::function<float(const float, const float)> randf = [&rand](const float min, const float max) {
        const float num = static_cast<float>(rand(0, UINT_MAX)) / static_cast<float>(UINT_MAX);
        return std::abs(max - min) * num + qMin(max, min);
    };

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

REGISTER_LAYOUT_DESCRIPTOR(Classification, DirClassificationLayoutDescriptor);
