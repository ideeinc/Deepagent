#include "caffetrainedmodelservice.h"
#include "caffetrainedmodel.h"
#include "neuralnetwork.h"
#include "dataset.h"
#include "classlabel.h"
#include "prediction.h"
#include "roccurve.h"
#include <cmath>
#include <limits>
#include <THttpRequest>
#include <TTemporaryFile>
#include <QtCore/QtCore>

const QStringList imageFilters = { "*.jpg", "*.jpeg", "*png" };


static int createTrainedModel(const QString &deployFilePath, const QString &meanPath, const QString &labelsPath,
                              const QString &trainedModelPath)
{
    NeuralNetwork net;
    QString name = "GoogLeNet" + QString::number(QDateTime::currentDateTime().toTime_t());
    net.setName(name);
    net.setType("deploy");
    net.setAbsFilePath(deployFilePath);
    if (!net.create()) {
        tError() << "Create Error: NeuralNetwork.create";
        return 0;
    }

    Dataset dset;
    // dset.setMeanPath(meanPath);
    if (!dset.create()) {
        tError() << "Create Error: TrainDataset.create";
        return 0;
    }

    QFile labelFile(labelsPath);
    if (labelFile.open(QIODevice::ReadOnly)) {
        QTextStream tsLabel(&labelFile);
        tsLabel.setAutoDetectUnicode(true);
        int idx = 0;
        while (!tsLabel.atEnd()) {
            auto line = tsLabel.readLine().trimmed();
            ClassLabel::create(dset.id().toInt(), idx++, line, "");
        }
    }

    CaffeTrainedModel model;
    model.setNeuralNetworkName(name);
    model.setModelPath(trainedModelPath);
    model.setDatasetId(dset.id().toInt());
    model.create();
    return (model.isNull()) ? 0 : model.id();
}


int CaffeTrainedModelService::create(THttpRequest &request)
{
    auto basedir = Tf::app()->webRootPath() + "caffemodel/";
    if (!QDir(basedir).exists()) {
        QDir(basedir).mkpath(".");
    }

    auto basepath = basedir + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());
    auto caffeTrainedModel = request.formItems("caffeTrainedModel");

    auto &formdata = request.multipartFormData();
    QString deployPath = basepath + ".deploy";
    QString meanPath = basepath + ".mean";
    QString labelPath = basepath + ".label";
    QString modelPath = basepath + ".caffemodel";

    formdata.renameUploadedFile("deployNetFile", deployPath);
    formdata.renameUploadedFile("meanFile", meanPath);
    formdata.renameUploadedFile("labelFile", labelPath);
    formdata.renameUploadedFile("trainedModelFile", modelPath);

    int modelId = createTrainedModel(deployPath, meanPath, labelPath, modelPath);
    return modelId;
}


void CaffeTrainedModelService::uploadTrainedModel(THttpRequest &request)
{
    auto entity = request.multipartFormData().entity("trainedModel");
    auto uploadFile = entity.uploadedFilePath();
    auto basedir = Tf::app()->webRootPath() + "caffemodel/";
    auto dirPath = basedir + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());
    QDir modelDir(dirPath);
    modelDir.mkpath(".");

    QProcess tar;
    QStringList args = { "xf", uploadFile, "-C", dirPath };
    tar.start("tar", args);
    tar.waitForFinished();
    tDebug() << "tar exit code:" << tar.exitCode() << " status:" << tar.exitStatus();

    QFile jsonFile(modelDir.absoluteFilePath("info.json"));
    if (jsonFile.open(QIODevice::ReadOnly)) {
        auto infoJson = QJsonDocument::fromJson(jsonFile.readAll()).object();
        auto deployFile = modelDir.absoluteFilePath(infoJson["deploy file"].toString());
        auto meanFile = modelDir.absoluteFilePath(infoJson["mean file"].toString());
        auto labelsFile = modelDir.absoluteFilePath(infoJson["labels file"].toString());
        auto snapshotFile = modelDir.absoluteFilePath(infoJson["snapshot file"].toString());
        createTrainedModel(deployFile, meanFile, labelsFile, snapshotFile);
    }
}


CaffeTrainedModelPredictContainer CaffeTrainedModelService::predict(int modelId, THttpRequest &request)
{
    CaffeTrainedModelPredictContainer container;
    auto &formdata = request.multipartFormData();
    auto entity = formdata.entity("imageFile");
    auto jpg = entity.uploadedFilePath();

    auto trainedModel = CaffeTrainedModel::get(modelId);
    if (trainedModel.isNull()) {
        tError() << "Empty trained model";
        return container;
    } else {
         container.jpegFile.setFile(jpg);
    }

    Prediction prediction(trainedModel.getNeuralNetwork().absFilePath());
    prediction.init(trainedModel.modelPath(), trainedModel.getDataset().meanFilePath());

    if (!jpg.isEmpty()) {
        // 1イメージ推論
        QList<QPair<QString, float>> rank;
        auto results = prediction.predictTop(jpg, 5);
        const QMap<int, ClassLabel> labels = ClassLabel::getMapByDatasetId(trainedModel.datasetId());

        for (auto &p : results) {
            int idx = p.first;
            QString lbl = labels.value(idx).name();
            if (lbl.isEmpty()) {
                lbl = QString::number(idx);
            }
            rank.append(qMakePair(lbl, p.second));
        }

        int r = 1;
        for (auto &p : rank) {
            auto res = QString("#%1 %2%  %3").arg(r++).arg(p.second * 100.0, 5, 'f', 1).arg(p.first);
            container.results << res;
            tDebug() << res;
        }

    } else {
        // TODO
    }
    prediction.release();
    return container;
}
