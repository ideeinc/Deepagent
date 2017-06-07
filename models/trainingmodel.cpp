#include <TreeFrogModel>
#include "trainingmodel.h"
#include "trainingmodelobject.h"

#define STATUS_JSON      "status.json"
#define SOLVER_PROTOTXT  "solver.prototxt"
#define TRAIN_PROTOTXT   "train.prototxt"
#define DEPLOY_PROTOTXT  "deploy.prototxt"


TrainingModel::TrainingModel()
    : TAbstractModel(), d(new TrainingModelObject())
{ }

TrainingModel::TrainingModel(const TrainingModel &other)
    : TAbstractModel(), d(new TrainingModelObject(*other.d))
{ }

// TrainingModel::TrainingModel(const TrainingModelObject &object)
//     : d(new TrainingModelObject(object))
// { }

TrainingModel::~TrainingModel()
{
    // If the reference count becomes 0,
    // the shared data object 'TrainingModelObject' is deleted.
}

QString TrainingModel::id() const
{
    return d->id;
}

QString TrainingModel::name() const
{
    return d->name;
}

void TrainingModel::setName(const QString &name)
{
    d->name = name;
}

QString TrainingModel::solverPrototxt() const
{
    return d->solver_prototxt;
}

void TrainingModel::setSolverPrototxt(const QString &solverPrototxt)
{
    d->solver_prototxt = solverPrototxt;
}

QString TrainingModel::solverPrototxtPath() const
{
    return (isNull()) ? QString() : Tf::app()->webRootPath() + "caffemodel/" + d->id + "/" + SOLVER_PROTOTXT;
}

QString TrainingModel::trainPrototxt() const
{
    return d->train_prototxt;
}

void TrainingModel::setTrainPrototxt(const QString &trainPrototxt)
{
    d->train_prototxt = trainPrototxt;
}

QString TrainingModel::trainPrototxtPath() const
{
    return (isNull()) ? QString() : Tf::app()->webRootPath() + "caffemodel/" + d->id + "/" + TRAIN_PROTOTXT;
}

QString TrainingModel::deployPrototxt() const
{
    return d->deploy_prototxt;
}

void TrainingModel::setDeployPrototxt(const QString &deployPrototxt)
{
    d->deploy_prototxt = deployPrototxt;
}

QString TrainingModel::deployPrototxtPath() const
{
    return (isNull()) ? QString() : Tf::app()->webRootPath() + "caffemodel/" + d->id + "/" + DEPLOY_PROTOTXT;
}

QString TrainingModel::pretrainedModel() const
{
    return d->pretrained_model;
}

void TrainingModel::setPretrainedModel(const QString &pretrainedModel)
{
    d->pretrained_model = pretrainedModel;
}

QString TrainingModel::logPath() const
{
    return d->log_path;
}

void TrainingModel::setLogPath(const QString &logPath)
{
    d->log_path = logPath;
}

QString TrainingModel::state() const
{
    return d->state;
}

void TrainingModel::setState(const QString &state)
{
    d->state = state;
}

QString TrainingModel::createdAt() const
{
    return d->created_at;
}

TrainingModel &TrainingModel::operator=(const TrainingModel &other)
{
    d = other.d;  // increments the reference count of the data
    return *this;
}

static bool writeFile(const QString &filePath, const QByteArray &data)
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

static QByteArray readFile(const QString &filePath)
{
    QFile file(filePath);
    return (file.open(QIODevice::ReadOnly)) ? file.readAll() : QByteArray();
}

bool TrainingModel::create()
{
    d->id = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());
    auto basedir = Tf::app()->webRootPath() + "caffemodel/" + d->id + "/";
    QDir dir(basedir);
    if (dir.exists()) {
        tError() << "exists already. " << d->id;
        return false;
    }

    if (name().isEmpty()) {
        setName(id());
    }

    d->created_at = QDateTime::currentDateTime().toString(Qt::ISODate);
    setState("Running");

    writeFile(basedir + SOLVER_PROTOTXT, solverPrototxt().toUtf8());
    writeFile(basedir + TRAIN_PROTOTXT, trainPrototxt().toUtf8());
    writeFile(basedir + DEPLOY_PROTOTXT, deployPrototxt().toUtf8());

    // write status.json
    auto varmap = toVariantMap();
    varmap.remove("id");
    varmap.remove("solverPrototxt");
    varmap.remove("trainPrototxt");
    varmap.remove("deployPrototxt");
    writeFile(basedir + STATUS_JSON, QJsonDocument(QJsonObject::fromVariantMap(varmap)).toJson());
    return true;
}

bool TrainingModel::update()
{
    // write status.json
    auto basedir = Tf::app()->webRootPath() + "caffemodel/" + id() + "/";
    auto varmap = toVariantMap();
    varmap.remove("id");
    varmap.remove("solverPrototxt");
    varmap.remove("trainPrototxt");
    varmap.remove("deployPrototxt");
    writeFile(basedir + STATUS_JSON, QJsonDocument(QJsonObject::fromVariantMap(varmap)).toJson());
    return true;
}

bool TrainingModel::save()
{
    return false;
}

bool TrainingModel::remove()
{
    if (isNull()) {
        return true;
    }

    QDir subdir(Tf::app()->webRootPath() + "caffemodel/" + id());
    QFile statusFile(subdir.filePath(STATUS_JSON));

    if (statusFile.exists() && statusFile.open(QIODevice::ReadOnly)) {
        auto statusJson = QJsonDocument::fromJson(statusFile.readAll()).object();
        auto state = statusJson.value("state").toString();
        if (state.toLower() != "done") {
            tWarn() << "Remove running model";
        }
        auto files = subdir.entryInfoList(QDir::Files);
        for (auto f : files) {
            QFile(f.absoluteFilePath()).remove();
        }
        subdir.rmpath(".");
        tDebug() << "removed " << id();
    }
    clear();
    return true;
}

TrainingModel TrainingModel::create(const QVariantMap &values)
{
    TrainingModel model;
    model.setProperties(values);
    if (!model.create()) {
        model.clear();
    }
    return model;
}

TrainingModel TrainingModel::get(const QString &id)
{
    TrainingModel model;
    auto dirpath = Tf::app()->webRootPath() + "caffemodel/" + id + "/";
    QDir dir(dirpath);

    if (dir.exists()) {
        QFile statusFile(dir.filePath(STATUS_JSON));
        if (!statusFile.exists() || !statusFile.open(QIODevice::ReadOnly)) {
            model.clear();
            return model;
        }

        auto statusJson = QJsonDocument::fromJson(statusFile.readAll()).object();
        model.setProperties(statusJson.toVariantMap());
        model.d->id = id;
        model.setSolverPrototxt(QString::fromUtf8(readFile(dirpath + SOLVER_PROTOTXT)));
        model.setTrainPrototxt(QString::fromUtf8(readFile(dirpath + TRAIN_PROTOTXT)));
        model.setDeployPrototxt(QString::fromUtf8(readFile(dirpath + DEPLOY_PROTOTXT)));
    }
    return model;
}

int TrainingModel::count()
{
    int cnt = 0;
    QDir baseDir(Tf::app()->webRootPath() + "caffemodel/");
    const auto dirs = baseDir.entryInfoList(QDir::AllDirs);

    for (auto &d : dirs) {
        QFileInfo statusFile(QDir(d.absoluteFilePath()).filePath(STATUS_JSON));
        //tDebug() << QFileInfo(statusFile).absoluteFilePath();
        if (statusFile.exists()) {
            cnt++;
        }
    }
    return cnt;
}

QList<TrainingModel> TrainingModel::getAll()
{
    QList<TrainingModel> modelList;
    QDir baseDir(Tf::app()->webRootPath() + "caffemodel/");
    const auto dirs = baseDir.entryInfoList(QDir::AllDirs);

    for (auto &d : dirs) {
        auto model = TrainingModel::get(d.fileName());
        if (!model.isNull()) {
            modelList << model;
        }
    }
    return modelList;
}

/*
QJsonArray TrainingModel::getAllJson()
{
    QJsonArray array;
    TSqlORMapper<TrainingModelObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<TrainingModelObject> i(mapper); i.hasNext(); ) {
            array.append(QJsonValue(QJsonObject::fromVariantMap(TrainingModel(i.next()).toVariantMap())));
        }
    }
    return array;
}
*/

void TrainingModel::clear()
{
    d->id.clear();
}

bool TrainingModel::isNull() const
{
    return d->id.isEmpty();
}

TModelObject *TrainingModel::modelData()
{
    return d.data();
}

const TModelObject *TrainingModel::modelData() const
{
    return d.data();
}

QString TrainingModel::caffeInfoLogPath() const
{
    QDir baseDir(Tf::app()->webRootPath() + "caffemodel/" + id());
    return (isNull()) ? QString() : baseDir.filePath("caffe.INFO");
}

QDataStream &operator<<(QDataStream &stream, const TrainingModel &model)
{
    auto varmap = model.toVariantMap();
    stream << varmap;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, TrainingModel &model)
{
    QVariantMap varmap;
    stream >> varmap;
    model.setProperties(varmap);
    return stream;
}

T_REGISTER_STREAM_OPERATORS(TrainingModel)
