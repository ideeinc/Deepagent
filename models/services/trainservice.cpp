#include <TGlobal>
#include <THttpRequest>
#include <TTemporaryFile>
#include <TBackgroundProcess>
#include <TBackgroundProcessHandler>
#include <QtCore>
#include "caffemodel.h"
#include "trainservice.h"
#include "caffeprocess.h"
#include "ssddetector.h"
#include "image.h"


TrainIndexContainer TrainService::index()
{
    TrainIndexContainer container;
    container.trainingModels = CaffeModel::getAll();
    tDebug() << "models count: " << container.trainingModels.count();
    return container;
}


static bool createFile(const QString &filePath, const QByteArray &data)
{
    QDir dir(QFileInfo(filePath).absolutePath());
    dir.mkpath(".");

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(data);
        file.close();
        return true;
    }
    return false;
}


class CleanupHandler : public TBackgroundProcessHandler
{
public:
    CleanupHandler(const QString &dirPath) : _dirPath(dirPath) {}

protected:
    void handleFinished(int exitCode, QProcess::ExitStatus exitStatus)
    {
        auto id = QFileInfo(_dirPath).fileName();
        auto model = CaffeModel::get(id);
        if (! model.isNull()) {
            model.setState("done");
            model.update();

        } else {
            tError() << "CaffeModel not found: " << id;
        }
    }

private:
    QString _dirPath;
};


static void caffeTrain(const QString &solverPath, const QString &pretrainedModel)
{
    static const auto CaffeCommandPath = Tf::app()->getConfig("settings").value("CaffeCommand").toString();
    auto *caffeCmd = new TBackgroundProcess();
    auto *handler = new CleanupHandler(QFileInfo(solverPath).absolutePath());

    auto workDir = QFileInfo(solverPath).absolutePath();
    caffeCmd->setWorkingDirectory(workDir);
    QStringList args = {"train", "-solver", solverPath, "-log_dir", workDir};
    tDebug() << "caffe command: " << CaffeCommandPath << " " << args.join(" ");
    caffeCmd->start(CaffeCommandPath, args, QProcess::ReadWrite, handler);
    caffeCmd->waitForStarted();
    caffeCmd->closeReadChannel(QProcess::StandardOutput);
    caffeCmd->closeReadChannel(QProcess::StandardError);
    caffeCmd->closeWriteChannel();
}


QString TrainService::create(THttpRequest &request)
{
    const auto CaffeCommandPath = Tf::app()->getConfig("settings").value("CaffeCommand").toString();
    auto caffeModel = request.formItems("caffeModel");
    caffeModel.insert("category", "classification");
    auto model = CaffeModel::create(caffeModel);

    if (! model.isNull()) {
        // Train model
        //caffeTrain(model.solverPrototxtPath(), "");
    }
    return model.id();
}


QString TrainService::createSsd(THttpRequest &request)
{
    const auto CaffeCommandPath = Tf::app()->getConfig("settings").value("CaffeCommand").toString();
    auto ssdParam = request.formItems("ssdParam");

    CaffeModel model;
    model.setName(ssdParam["name"].toString());
    model.setCategory("detection");

    if (! model.create()) {
        // Train model
        //caffeTrain(model.solverPrototxtPath(), "");
    }
    return model.id();
}


static void plotResultPng(const QString &caffeLogPath, const QString &keyword, int index, float unit, const QString &outPath)
{
    QProcess grep;
    QProcess awk;
    grep.setStandardOutputProcess(&awk);
    grep.start("grep", QStringList({keyword, caffeLogPath}));
    auto awkarg = QString("{ print $%1 }").arg(index);
    awk.start("awk", QStringList({awkarg}));
    grep.waitForFinished();
    awk.waitForFinished();

    auto awkOutput = awk.readAllStandardOutput();
    QByteArray output;
    int num = 1;
    for (auto &line : awkOutput.split('\n')) {
        if (!line.isEmpty()) {
            output += QByteArray::number(unit * num++) + "\t" + line + "\n";
        }
    }
    TTemporaryFile tmpfile;
    tmpfile.open();
    tmpfile.write(output);
    tmpfile.close();

    QProcess plot;
    QString plotarg = "set terminal png; set output '%1'; plot '%2' using 1:2 smooth unique;";
    plotarg = plotarg.arg(outPath).arg(tmpfile.absoluteFilePath());
    plot.start("gnuplot", QStringList({"-e", plotarg}));
    plot.waitForFinished();
}


TrainDetectContainer TrainService::detect(const QString &id, THttpRequest &request)
{
    TrainDetectContainer container;
    container.caffeModel = CaffeModel::get(id);

    switch (request.method()) {
    case Tf::Get: {
        break; }

    case Tf::Post: {
        auto &formdata = request.multipartFormData();
        auto entity = formdata.entity("imageFile");
        auto jpg = entity.uploadedFilePath();
        auto caffeModel = CaffeModel::get(id);
        auto meanValue = request.formItemValue("meanValue", "53,74,144");
        auto trainedModel = request.formItemValue("trainedModel");
        auto image = Image(jpg);

        if (! caffeModel.isNull() && ! jpg.isEmpty() && ! trainedModel.isEmpty()) {
            container.detections = SsdDetector::detect(jpg, 0.4, caffeModel.deployFilePath(), caffeModel.trainedModelFilePath(trainedModel), meanValue);
            for (const auto &c : container.detections) {
                image.drawRectangle(c[3], c[4], c[5], c[6], CV_RGB(0,200,0), 2, 8);
                tInfo() << "class:" << c[1] << " score:"  << c[2] << " w:" << (int)(c[3]) << " h:"  << (int)c[4]
                        << " w:" << (int)c[5] << " h:"  << (int)c[6];
            }
        }
        container.jpegBin = image.toEncoded(".jpg");
        break; }

    default:
        break;
    }
    return container;
}


TrainShowContainer TrainService::show(const QString &id)
{
    TrainShowContainer container;
    container.caffeModel = CaffeModel::get(id);

    QFileInfo caffeLog(container.caffeModel.caffeInfoLogPath());
    if (caffeLog.exists()) {
        QString path = caffeLog.absolutePath() + "/plot0.png";
        plotResultPng(container.caffeModel.caffeInfoLogPath(), "Train net output #0", 11, 40/2702.0, path);
        container.graphImages << path;

        path = caffeLog.absolutePath() + "/plot_t.png";
        plotResultPng(container.caffeModel.caffeInfoLogPath(), "Test net output #0", 11, 1, path);
        container.graphImages << path;
    }
    return container;
}


bool TrainService::remove(const QString &id)
{
    auto model = CaffeModel::get(id);
    return (model.isNull()) ? true : model.remove();
}


TrainCreateContainer TrainService::clone(const QString &id)
{
    TrainCreateContainer container;
    container.caffeModel = CaffeModel::get(id);
    return container;
}


QByteArray TrainService::getPrototxt(const QString &id, const QString &prototxt)
{
    static const QStringList Prototxts = {"solverPrototxt", "trainPrototxt", "deployPrototxt"};

    if (!Prototxts.contains(prototxt)) {
        tError() << "invalid params: " << prototxt;
        return QByteArray();
    }
    auto caffeModel = CaffeModel::get(id).toVariantMap();
    return caffeModel.value(prototxt).toByteArray();
}
