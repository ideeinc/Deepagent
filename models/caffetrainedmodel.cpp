#include <TreeFrogModel>
#include "caffetrainedmodel.h"
#include "caffetrainedmodelobject.h"
#include "dataset.h"
#include "neuralnetwork.h"


CaffeTrainedModel::CaffeTrainedModel()
    : TAbstractModel(), d(new CaffeTrainedModelObject())
{
    d->id = 0;
    d->epoch = 0;
    d->dataset_id = 0;
    d->img_width = 0;
    d->img_height = 0;
    d->lock_revision = 0;
}

CaffeTrainedModel::CaffeTrainedModel(const CaffeTrainedModel &other)
    : TAbstractModel(), d(new CaffeTrainedModelObject(*other.d))
{ }

CaffeTrainedModel::CaffeTrainedModel(const CaffeTrainedModelObject &object)
    : TAbstractModel(), d(new CaffeTrainedModelObject(object))
{ }

CaffeTrainedModel::~CaffeTrainedModel()
{
    // If the reference count becomes 0,
    // the shared data object 'CaffeTrainedModelObject' is deleted.
}

int CaffeTrainedModel::id() const
{
    return d->id;
}

QString CaffeTrainedModel::modelPath() const
{
    return d->model_path;
}

void CaffeTrainedModel::setModelPath(const QString &modelPath)
{
    d->model_path = modelPath;
}

QString CaffeTrainedModel::neuralNetworkName() const
{
    return d->neural_network_name;
}

void CaffeTrainedModel::setNeuralNetworkName(const QString &neuralNetworkName)
{
    d->neural_network_name = neuralNetworkName;
}

int CaffeTrainedModel::epoch() const
{
    return d->epoch;
}

void CaffeTrainedModel::setEpoch(int epoch)
{
    d->epoch = epoch;
}

int CaffeTrainedModel::datasetId() const
{
    return d->dataset_id;
}

void CaffeTrainedModel::setDatasetId(int datasetId)
{
    d->dataset_id = datasetId;
}

int CaffeTrainedModel::imgWidth() const
{
    return d->img_width;
}

void CaffeTrainedModel::setImgWidth(int imgWidth)
{
    d->img_width = imgWidth;
}

int CaffeTrainedModel::imgHeight() const
{
    return d->img_height;
}

void CaffeTrainedModel::setImgHeight(int imgHeight)
{
    d->img_height = imgHeight;
}

QDateTime CaffeTrainedModel::updatedAt() const
{
    return d->updated_at;
}

int CaffeTrainedModel::lockRevision() const
{
    return d->lock_revision;
}

CaffeTrainedModel &CaffeTrainedModel::operator=(const CaffeTrainedModel &other)
{
    d = other.d;  // increments the reference count of the data
    return *this;
}

CaffeTrainedModel CaffeTrainedModel::create(const QString &modelPath, const QString &neuralNetworkName, int epoch, int datasetId, int imgWidth, int imgHeight)
{
    CaffeTrainedModelObject obj;
    obj.model_path = modelPath;
    obj.neural_network_name = neuralNetworkName;
    obj.epoch = epoch;
    obj.dataset_id = datasetId;
    obj.img_width = imgWidth;
    obj.img_height = imgHeight;
    if (!obj.create()) {
        return CaffeTrainedModel();
    }
    return CaffeTrainedModel(obj);
}

CaffeTrainedModel CaffeTrainedModel::create(const QVariantMap &values)
{
    CaffeTrainedModel model;
    model.setProperties(values);
    if (!model.d->create()) {
        model.d->clear();
    }
    return model;
}

CaffeTrainedModel CaffeTrainedModel::get(int id)
{
    TSqlORMapper<CaffeTrainedModelObject> mapper;
    return CaffeTrainedModel(mapper.findByPrimaryKey(id));
}

CaffeTrainedModel CaffeTrainedModel::get(int id, int lockRevision)
{
    TSqlORMapper<CaffeTrainedModelObject> mapper;
    TCriteria cri;
    cri.add(CaffeTrainedModelObject::Id, id);
    cri.add(CaffeTrainedModelObject::LockRevision, lockRevision);
    return CaffeTrainedModel(mapper.findFirst(cri));
}

int CaffeTrainedModel::count()
{
    TSqlORMapper<CaffeTrainedModelObject> mapper;
    return mapper.findCount();
}

QList<CaffeTrainedModel> CaffeTrainedModel::getAll()
{
    return tfGetModelListByCriteria<CaffeTrainedModel, CaffeTrainedModelObject>(TCriteria());
}

QJsonArray CaffeTrainedModel::getAllJson()
{
    QJsonArray array;
    TSqlORMapper<CaffeTrainedModelObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<CaffeTrainedModelObject> i(mapper); i.hasNext(); ) {
            array.append(QJsonValue(QJsonObject::fromVariantMap(CaffeTrainedModel(i.next()).toVariantMap())));
        }
    }
    return array;
}

TModelObject *CaffeTrainedModel::modelData()
{
    return d.data();
}

const TModelObject *CaffeTrainedModel::modelData() const
{
    return d.data();
}

NeuralNetwork CaffeTrainedModel::getNeuralNetwork() const
{
    return NeuralNetwork::getOneByName(neuralNetworkName());
}


Dataset CaffeTrainedModel::getDataset() const
{
    return Dataset::get(datasetId());
}
