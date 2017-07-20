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
#include "prediction.h"
#include "image.h"
#include "dataset.h"
#include "roccurve.h"


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


static QList<QVector<float>> ssdDetect(const SsdDetector &detector, float threshold, float drawThreshold, Image &image)
{
    const QMap<int, cv::Scalar> colorMap = {{1,CV_RGB(255,255,0)}, {2,CV_RGB(255,255,0)}, {5,CV_RGB(255,255,0)}, {8,CV_RGB(255,255,0)},
                                            {9,CV_RGB(255,255,0)}, {13,CV_RGB(0,0,200)}};

    auto detects = detector.detect(image.mat(), threshold);

    for (const auto &c : detects) {
        auto score = c[2];
        if (score > drawThreshold) {
            auto color = colorMap.value(c[1], CV_RGB(0,200,0));

            image.drawRectangle(c[3], c[4], c[5], c[6], color, 1);
            auto text = QString("C%1 : %2").arg(c[1]).arg(score, 0, 'g', 3);
            image.drawLabel(text, c[3], c[4], cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0), 1, color, 0.5); // label
            tInfo() << "class:" << c[1] << " score:"  << score << " w:" << (int)(c[3]) << " h:"  << (int)c[4]
                    << " w:" << (int)c[5] << " h:"  << (int)c[6];
        }
    }
    return detects;
}


static QFileInfoList searchRecursive(const QDir &dir, const QStringList &nameFilters, QDir::SortFlags sort=QDir::Name)
{
    auto files = dir.entryInfoList(nameFilters, QDir::Files, sort);
    auto subdirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (auto &d : subdirs) {
       files << searchRecursive(QDir(d.absoluteFilePath()), nameFilters, sort);
   }
   return files;
}


static QMap<int,QString> parseLabel(const QByteArray &data)
{
    QMap<int, QString> labels;
    QJsonParseError error;
    auto jsonDoc = QJsonDocument::fromJson(data, &error);

    if (jsonDoc.isEmpty()) {
        tWarn() << "JSON parse error : " << error.errorString();
    } else {
        auto jsonObj = jsonDoc.object();
        auto version = jsonObj.value("formatVersion").toInt();
        tDebug() << "labels.json format_version:" << version;

        switch (version) {
        case 2: {
            bool ok;
            for (auto it = jsonObj.constBegin(); it != jsonObj.constEnd(); ++it) {
                auto dispname = it.value().toObject()["displayName"].toString();
                tDebug() << "label " << it.key() << ": " << dispname;
                int num = it.key().toInt(&ok);
                if (ok) {
                    labels.insert(num, dispname);
                }
            }
            break; }

        default: {
            bool ok;
            auto jsonMap = jsonDoc.toVariant().toMap();
            for (auto it = jsonMap.constBegin(); it != jsonMap.constEnd(); ++it) {
                tDebug() << "label " << it.key() << ": " << it.value();
                int num = it.key().toInt(&ok);
                if (ok) {
                    labels.insert(num, it.value().toString());
                }
            }
            break; }
        }
    }
    return labels;
}


static float predictSum(const Prediction &predict, const QList<int> &sumIndex, const QString &filePath)
{
    float score = 0;
    auto pred = predict.predict(filePath);
    for (int i : sumIndex) {
        score += pred[i];
    }
    return score;
}


TrainClassifyContainer TrainService::classify(const QString &id, THttpRequest &request)
{
    TrainClassifyContainer container;
    container.caffeModel = CaffeModel::get(id);
    if (container.caffeModel.category().toLower() != "classification") {
        container.caffeModel.clear();
        return container;
    }
    const auto &cm = container.caffeModel;
    const auto dataset = cm.getDataset();

    switch (request.method()) {
    case Tf::Get: {
        break; }

    case Tf::Post: {
        auto &formdata = request.multipartFormData();
        auto entities = formdata.entityList("imageFile[]");
        auto trainedModel = request.formItemValue("trainedModel");
        auto imageDir = formdata.formItemValue("imageDir");
        auto aucIndexes = [](const QStringList &lst) {
            // QStringList -> QList<int>
            QList<int> ret;
            bool ok;
            for (auto &item : lst) {
                int d = item.trimmed().toInt(&ok);
                if (ok) ret << d;
            }
            return ret;
        }(formdata.formItemValue("aucIndexes").split(","));

        if (trainedModel.isEmpty()) {
            tWarn() << "empty trained model!";
            break;
        }

        if (dataset.isNull()) {
            tWarn() << "Null dataset";
        }

        Prediction prediction(cm.deployFilePath());
        prediction.init(cm.trainedModelFilePath(trainedModel), dataset.meanFilePath());

        QMap<int, QString> labels = parseLabel(dataset.labelData().toUtf8());
        QString imageDirPath = request.formItemValue("imageDir");

        if (imageDirPath.isEmpty()) {
            for (auto &ent : entities) {
                auto jpg = ent.uploadedFilePath();
                auto results = prediction.predictTop(jpg, 5);
                QList<QPair<QString, float>> predictions;

                for (auto &p : results) {
                    auto name = labels.value(p.first, QString::number(p.first));
                    predictions << QPair<QString, float>(name, p.second);
                    tInfo() << name << " : " << p.second;
                }

                container.predictionsList << predictions;
                container.jpegBinList << Image(jpg).toEncoded("jpg");
            }
        } else {
            //const QList<int> ColitisIndex = {1};  // 5分類での大腸炎
            //const QList<int> ColitisIndex = {3};  // 8分類での大腸炎
            //const QList<int> ColitisIndex = {7,8,9,10,11,12,13};  // 16分類での大腸炎

            // ディレクトリ指定
            QDir imageDir(imageDirPath);
            tDebug() << "imageDir: " << imageDirPath;

            if (imageDir.exists()) {
                auto dirPath = imageDir.absolutePath();
                QDirIterator it(dirPath, {"*.jpg","*.jpeg"}, QDir::Files, QDirIterator::Subdirectories);
                QList<QPair<float, int>> rocData;

                while (it.hasNext()) {
                    auto path = it.next();
                    tDebug() << path.mid(dirPath.length() + 1);
                    int cor = path.mid(dirPath.length() + 1).startsWith("1_") ? 1 : 0;  // correct

                    auto score = predictSum(prediction, aucIndexes, path);
                    tDebug() << "score:" << score << " [correct:" << cor << "]  name:" << QFileInfo(path).fileName().left(6);
                    if (! aucIndexes.isEmpty()) {
                        rocData << qMakePair(score, cor);
                    }

                    if (score < 0.5) {
                        tWarn() << "## score:" << score << "]  name:" << QFileInfo(path).fileName();
                    }
                }

                // AUC算出
                if (! rocData.isEmpty()) {
                    auto auc = RocCurve(rocData).getAuc();
                    tDebug() << "AUC:" << auc << "  (sample count:" << rocData.count() << ")";
                }
            }
        }
        break; }

    default:
        break;
    }
    return container;
}


TrainDetectContainer TrainService::detect(const QString &id, THttpRequest &request)
{
    const QString outDir("/home/aoyama/cancer_pics/");

    TrainDetectContainer container;
    container.caffeModel = CaffeModel::get(id);
    if (container.caffeModel.category().toLower() != "detection") {
        container.caffeModel.clear();
        return container;
    }
    const auto &cm = container.caffeModel;

    switch (request.method()) {
    case Tf::Get: {
        break; }

    case Tf::Post: {
        auto &formdata = request.multipartFormData();
        auto entities = formdata.entityList("imageFile[]");
        auto meanValue = request.formItemValue("meanValue", "104,117,123");
        auto trainedModel = request.formItemValue("trainedModel");
        auto imageDir = formdata.formItemValue("imageDir");

        if (trainedModel.isEmpty()) {
            tWarn() << "empty trained model!";
            break;
        }

        SsdDetector detector(cm.deployFilePath().toStdString(), cm.trainedModelFilePath(trainedModel).toStdString(),
                             string(), meanValue.toStdString());

        QMap<int, QString> labels = parseLabel(cm.getDataset().labelData().toUtf8());

        if (! imageDir.isEmpty()) { // ディレクトリ指定
            QDir(outDir).mkpath(".");

            const QStringList nameFilters = {"*.jpg", "*.jpeg"};
            auto images = searchRecursive(imageDir, nameFilters);

            for (auto &img : images) {
                Image jpg(img.absoluteFilePath());
                if (jpg.width() < 50 || jpg.height() < 50) {
                    tWarn() << "too small size: " << img.fileName();
                    continue;
                }
                jpg.trim();

                auto detections = ssdDetect(detector, 0.1, 0.5, jpg);
                // Sort by score
                std::sort(detections.begin(), detections.end(), [](const QVector<float> &v1, const QVector<float> &v2) -> bool {
                    return v1[2] > v2[2];
                });

                tDebug() << "image file name: " << img.fileName();
                const QString outpath = outDir % img.dir().dirName() % "/" % img.fileName();
                QDir(outDir % img.dir().dirName()).mkpath(".");
                jpg.save(outpath);
                QString logmsg;
                const QString format = "\"class:%1, score:%2, x1:%3, y1:%4, x2:%5, y2:%6\", ";
                for (auto &c : detections) {
                    auto name = labels[(int)c[1]];
                    logmsg += format.arg(name).arg(c[2],0,'g',3).arg((int)c[3]).arg((int)c[4]).arg((int)c[5]).arg((int)c[6]);
                }
                logmsg.chop(2);
                tInfo() << "#" % img.dir().dirName() % ", " % img.fileName() % ", " % logmsg;
            }

        } else if (! entities.isEmpty()) { // 画像ファイル
            for (auto &ent : entities) {
                auto jpg = ent.uploadedFilePath();
                tInfo() << jpg;
                auto image = Image(jpg);
                image.trim();
                QList<QVector<float>> detections;

                if (! cm.isNull() && ! image.isEmpty()) {
                    detections << ssdDetect(detector, 0.1, 0.5, image);
                }

                container.detectionsList << detections;
                container.jpegBinList << image.toEncoded("jpg");
            }
        } else {
            tWarn() << "empty image!";
        }
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


static bool moveFile(const QString &filePath, const QString &dstFilePath, bool overwrite = true)
{
    QFile dstFile(dstFilePath);
    QDir dstDir(QFileInfo(dstFile).absolutePath());

    if (! dstDir.exists()) {
        dstDir.mkpath(".");
    }

    if (dstFile.exists()) {
        if (overwrite) {
            dstFile.remove();
        } else {
            return false;
        }
    }
    return QFile::rename(filePath, dstFilePath);
}


static bool move(const QString &filePath, const QString &dstDirPath, bool overwrite = true)
{
    auto dstFilePath = QDir(dstDirPath).absoluteFilePath(QFileInfo(filePath).fileName());
    return moveFile(filePath, dstFilePath, overwrite);
}


void TrainService::uploadTrainedModel(THttpRequest &request)
{
    auto entity = request.multipartFormData().entity("trainedModel");
    auto uploadFile = entity.uploadedFilePath();

    if (uploadFile.isEmpty()) {
        return;
    }

    QString tempPath = Tf::app()->tmpPath() + QString::number(Tf::rand64_r());
    QDir tempDir(tempPath);
    tempDir.mkpath(".");

    QProcess tar;
    QStringList args = { "xf", uploadFile, "-C", tempPath };
    tar.start("tar", args);
    tar.waitForFinished();
    tDebug() << "tar exit code:" << tar.exitCode() << " status:" << tar.exitStatus();

    QFile jsonFile(tempDir.absoluteFilePath("info.json"));
    if (jsonFile.open(QIODevice::ReadOnly)) {
        auto infoJson = QJsonDocument::fromJson(jsonFile.readAll()).object();

        Dataset dataset;
        dataset.create();
        moveFile(tempDir.absoluteFilePath(infoJson["mean file"].toString()), dataset.meanFilePath());
        // Labels
        auto labels = Dataset::readFile(tempDir.absoluteFilePath(infoJson["labels file"].toString()));
        QTextStream ts(&labels);
        QJsonObject json;
        json["formatVersion"] = 2;

        int i = 0;
        while (! ts.atEnd()) {
            auto line = ts.readLine();
            if (! line.isEmpty()) {
                QJsonObject name;
                name["name"] = line;
                name["displayName"] = line;
                json[QString::number(i++)] = name;
            }
        }
        dataset.setLabelData(QString::fromUtf8(QJsonDocument(json).toJson()));

        auto name = infoJson["name"].toString();
        dataset.setName(name + "_Dateset");
        dataset.setState("done");
        dataset.update();

        CaffeModel caffeModel;
        caffeModel.create();
        moveFile(tempDir.absoluteFilePath(infoJson["solver file"].toString()), caffeModel.solverFilePath());
        moveFile(tempDir.absoluteFilePath(infoJson["deploy file"].toString()), caffeModel.deployFilePath());
        moveFile(tempDir.absoluteFilePath(infoJson["train_val file"].toString()), caffeModel.trainFilePath());
        moveFile(tempDir.absoluteFilePath(infoJson["model file"].toString()), caffeModel.networkFilePath());

        auto trainedModel = infoJson["snapshot file"].toString();
        move(tempDir.absoluteFilePath(trainedModel), caffeModel.dirPath());
        caffeModel.setTrainedModelFiles({trainedModel});
        caffeModel.setCategory("Classification");
        caffeModel.setName(name);
        caffeModel.setState("done");
        caffeModel.setDatasetId(dataset.id());
        caffeModel.update();
    }

    tempDir.removeRecursively();
}
