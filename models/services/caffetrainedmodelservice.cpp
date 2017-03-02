#include "caffetrainedmodelservice.h"
#include "caffetrainedmodel.h"
#include "neuralnetwork.h"
#include "dataset.h"
#include "classlabel.h"
#include "services/prediction.h"
#include <THttpRequest>
#include <QtCore/QtCore>


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

    NeuralNetwork net;
    net.setName("GoogLeNet");
    net.setType("deploy");
    net.setAbsFilePath(deployPath);
    if (!net.create()) {
        tError() << "Create Error: NeuralNetwork.create";
        return 0;
    }

    Dataset dset;
    dset.setMeanPath(meanPath);
    if (!dset.create()) {
        tError() << "Create Error: Dataset.create";
        return 0;
    }

    QFile labelFile(labelPath);
    if (labelFile.open(QIODevice::ReadOnly)) {
        QTextStream tsLabel(&labelFile);
        tsLabel.setAutoDetectUnicode(true);
        while (!tsLabel.atEnd()) {
            auto line = tsLabel.readLine();
            auto col = line.split(" ", QString::SkipEmptyParts);
            ClassLabel::create(dset.id(), col.value(0).toInt(), col.value(1), "");
        }
    }

    auto model = CaffeTrainedModel::create(caffeTrainedModel);
    model.setModelPath(modelPath);
    model.setDatasetId(dset.id());
    model.update();
    return (model.isNull()) ? 0 : model.id();
}



void CaffeTrainedModelService::predict(int modelId, THttpRequest &request)
{
    auto &formdata = request.multipartFormData();
    auto entity = formdata.entity("imageFile");
    auto jpg = entity.uploadedFilePath();

    auto trainedModel = CaffeTrainedModel::get(modelId);
    if (trainedModel.isNull()) {
        return;
    }

    Prediction prediction(trainedModel);
    prediction.init();
    prediction.predict(jpg);

//    formdata.renameUploadedFile("imageFile", deployPath);
}
