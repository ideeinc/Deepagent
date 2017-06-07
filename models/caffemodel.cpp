#include <TreeFrogModel>
#include "caffemodel.h"
#include "caffemodelobject.h"


CaffeModel::CaffeModel()
    : CaffeData(), d(new CaffeModelObject())
{
    CaffeData::setDataType("caffemodel");
    d->solver_file = "solver.prototxt";
    d->train_file = "train.prototxt";
    d->deploy_file = "deploy.prototxt";
    d->log_file = "caffe.log";
}

CaffeModel::CaffeModel(const CaffeModel &other)
    : CaffeData(*(CaffeData*)&other), d(new CaffeModelObject(*other.d))
{ }

// CaffeModel::CaffeModel(const CaffeModelObject &object)
//     : CaffeData(), d(new CaffeModelObject(object))
// { }

CaffeModel::~CaffeModel()
{
    // If the reference count becomes 0,
    // the shared data object 'CaffeModelObject' is deleted.
}

// QString CaffeModel::id() const
// {
//     return d->id;
// }

// void CaffeModel::setId(const QString &id)
// {
//     d->id = id;
// }

QString CaffeModel::datasetId() const
{
    return d->dataset_id;
}

void CaffeModel::setDatasetId(const QString &datasetId)
{
    d->dataset_id = datasetId;
}

QString CaffeModel::meanFile() const
{
    return d->mean_file;
}

void CaffeModel::setMeanFile(const QString &meanFile)
{
    d->mean_file = meanFile;
}

QString CaffeModel::meanData() const
{
    return d->mean_data;
}

void CaffeModel::setMeanData(const QString &meanData)
{
    d->mean_data = meanData;
}

QString CaffeModel::solverFile() const
{
    return d->solver_file;
}

void CaffeModel::setSolverFile(const QString &solverFile)
{
    d->solver_file = solverFile;
}

QString CaffeModel::solverPrototxt() const
{
    return d->solver_prototxt;
}

void CaffeModel::setSolverPrototxt(const QString &solverPrototxt)
{
    d->solver_prototxt = solverPrototxt;
}

QString CaffeModel::trainFile() const
{
    return d->train_file;
}

void CaffeModel::setTrainFile(const QString &trainFile)
{
    d->train_file = trainFile;
}

QString CaffeModel::trainPrototxt() const
{
    return d->train_prototxt;
}

void CaffeModel::setTrainPrototxt(const QString &trainPrototxt)
{
    d->train_prototxt = trainPrototxt;
}

QString CaffeModel::deployFile() const
{
    return d->deploy_file;
}

void CaffeModel::setDeployFile(const QString &deployFile)
{
    d->deploy_file = deployFile;
}

QString CaffeModel::deployPrototxt() const
{
    return d->deploy_prototxt;
}

void CaffeModel::setDeployPrototxt(const QString &deployPrototxt)
{
    d->deploy_prototxt = deployPrototxt;
}

QString CaffeModel::networkFile() const
{
    return d->network_file;
}

void CaffeModel::setNetworkFile(const QString &networkFile)
{
    d->network_file = networkFile;
}

QString CaffeModel::networkPrototxt() const
{
    return d->network_prototxt;
}

void CaffeModel::setNetworkPrototxt(const QString &networkPrototxt)
{
    d->network_prototxt = networkPrototxt;
}

QString CaffeModel::trainedModelFiles() const
{
    return d->trained_model_files;
}

void CaffeModel::setTrainedModelFiles(const QString &trainedModelFiles)
{
    d->trained_model_files = trainedModelFiles;
}

QString CaffeModel::logFile() const
{
    return d->log_file;
}

void CaffeModel::setLogFile(const QString &logFile)
{
    d->log_file = logFile;
}

CaffeModel &CaffeModel::operator=(const CaffeModel &other)
{
    CaffeData::operator=(other);
    d = other.d;  // increments the reference count of the data
    return *this;
}

// CaffeModel CaffeModel::create(const QString &id, const QString &datasetId, const QString &meanFile, const QString &solverFile, const QString &trainFile, const QString &deployFile, const QString &networkFile, const QString &trainedModelFiles, const QString &logFile)
// {
//     CaffeModelObject obj;
//     obj.id = id;
//     obj.dataset_id = datasetId;
//     obj.mean_file = meanFile;
//     obj.solver_file = solverFile;
//     obj.train_file = trainFile;
//     obj.deploy_file = deployFile;
//     obj.network_file = networkFile;
//     obj.trained_model_files = trainedModelFiles;
//     obj.log_file = logFile;
//     if (!obj.create()) {
//         return CaffeModel();
//     }
//     return CaffeModel(obj);
// }

// QString CaffeModel::solverPrototxt() const
// {
//     return QString::fromUtf8(readFile(dirPath() % solverFile()));
// }

// void CaffeModel::setSolverPrototxt(const QString &prototxt)
// {
//     writeFile(dirPath() % solverFile(), prototxt.toUtf8());
// }

// QString CaffeModel::trainPrototxt() const
// {
//     return QString::fromUtf8(readFile(dirPath() % trainFile()));
// }

// void CaffeModel::setTrainPrototxt(const QString &prototxt)
// {
//     writeFile(dirPath() % trainFile(), prototxt.toUtf8());
// }

// QString CaffeModel::deployPrototxt() const
// {
//     return QString::fromUtf8(readFile(dirPath() % deployFile()));
// }

// void CaffeModel::setDeployPrototxt(const QString &prototxt)
// {
//     writeFile(dirPath() % deployFile(), prototxt.toUtf8());
// }

// QString CaffeModel::networkPrototxt() const
// {
//     return QString::fromUtf8(readFile(dirPath() % networkFile()));
// }

// void CaffeModel::setNetworkPrototxt(const QString &prototxt)
// {
//     writeFile(dirPath() % networkFile(), prototxt.toUtf8());
// }

QString CaffeModel::solverPrototxtPath() const
{
    return dirPath() % solverFile();
}

QString CaffeModel::caffeInfoLogPath() const
{
    return dirPath() % "caffe.INFO";
}

bool CaffeModel::create()
{
    setId(QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));
    return update();
}

CaffeModel CaffeModel::create(const QVariantMap &values)
{
    CaffeModel model;
    model.setProperties(values);
    if (! model.create()) {
        model.d->clear();
    }
    return model;
}

bool CaffeModel::update()
{
    if (id().isEmpty()) {
        return false;
    }
    // json
    auto values = toVariantMap();
    values.remove("meanData");
    values.remove("solverPrototxt");
    values.remove("trainPrototxt");
    values.remove("deployPrototxt");
    values.remove("networkPrototxt");
    writeJson(jsonFilePath(), values, true);

    // prototxt files
    writeFile(dirPath() + solverFile(), solverPrototxt().toUtf8(), true);
    writeFile(dirPath() + trainFile(), trainPrototxt().toUtf8(), true);
    writeFile(dirPath() + deployFile(), deployPrototxt().toUtf8(), true);
    writeFile(dirPath() + networkFile(), networkPrototxt().toUtf8(), true);
    return true;
}

bool CaffeModel::save()
{
    return (id().isEmpty()) ? create() : update();
}

void CaffeModel::clear()
{
    CaffeData::clear();
    d->clear();
}

CaffeModel CaffeModel::get(const QString &id)
{
    CaffeModel model = CaffeData::get<CaffeModel>(id);
    if (! model.isNull()) {
        model.setSolverPrototxt(model.dirPath() + model.solverFile());
        model.setTrainPrototxt(model.dirPath() + model.trainFile());
        model.setDeployPrototxt(model.dirPath() + model.deployFile());
        model.setNetworkPrototxt(model.dirPath() + model.networkFile());
    }
    return model;
}

// int CaffeModel::count()
// {
//     // TSqlORMapper<CaffeModelObject> mapper;
//     // return mapper.findCount();
// }

QList<CaffeModel> CaffeModel::getAll()
{
    return CaffeData::getAll<CaffeModel>();
}

// QJsonArray CaffeModel::getAllJson()
// {
//     QJsonArray array;
//     TSqlORMapper<CaffeModelObject> mapper;

//     if (mapper.find() > 0) {
//         for (TSqlORMapperIterator<CaffeModelObject> i(mapper); i.hasNext(); ) {
//             array.append(QJsonValue(QJsonObject::fromVariantMap(CaffeModel(i.next()).toVariantMap())));
//         }
//     }
//     return array;
// }

void CaffeModel::setProperties(const QVariantMap &properties)
{
    CaffeData::setProperties(properties);
    TAbstractModel::setProperties(properties);
}

QVariantMap CaffeModel::toVariantMap() const
{
    auto ret = CaffeData::toVariantMap().unite(TAbstractModel::toVariantMap());
    return ret;
}

TModelObject *CaffeModel::modelData()
{
    return d.data();
}

const TModelObject *CaffeModel::modelData() const
{
    return d.data();
}

QDataStream &operator<<(QDataStream &ds, const CaffeModel &model)
{
    auto varmap = model.toVariantMap();
    ds << varmap;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, CaffeModel &model)
{
    QVariantMap varmap;
    ds >> varmap;
    model.setProperties(varmap);
    return ds;
}

// Don't remove below this line
T_REGISTER_STREAM_OPERATORS(CaffeModel)
