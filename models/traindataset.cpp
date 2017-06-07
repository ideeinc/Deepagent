#include <TreeFrogModel>
#include "traindataset.h"
#include "traindatasetobject.h"

TrainDataset::TrainDataset()
    : TAbstractModel(), d(new TrainDatasetObject())
{
    d->id = 0;
    d->lock_revision = 0;
}

TrainDataset::TrainDataset(const TrainDataset &other)
    : TAbstractModel(), d(new TrainDatasetObject(*other.d))
{ }

TrainDataset::TrainDataset(const TrainDatasetObject &object)
    : TAbstractModel(), d(new TrainDatasetObject(object))
{ }

TrainDataset::~TrainDataset()
{
    // If the reference count becomes 0,
    // the shared data object 'TrainDatasetObject' is deleted.
}

int TrainDataset::id() const
{
    return d->id;
}

QString TrainDataset::dbPath() const
{
    return d->db_path;
}

void TrainDataset::setDbPath(const QString &dbPath)
{
    d->db_path = dbPath;
}

QString TrainDataset::dbType() const
{
    return d->db_type;
}

void TrainDataset::setDbType(const QString &dbType)
{
    d->db_type = dbType;
}

QString TrainDataset::meanPath() const
{
    return d->mean_path;
}

void TrainDataset::setMeanPath(const QString &meanPath)
{
    d->mean_path = meanPath;
}

QDateTime TrainDataset::updatedAt() const
{
    return d->updated_at;
}

int TrainDataset::lockRevision() const
{
    return d->lock_revision;
}

TrainDataset &TrainDataset::operator=(const TrainDataset &other)
{
    d = other.d;  // increments the reference count of the data
    return *this;
}

TrainDataset TrainDataset::create(const QString &dbPath, const QString &dbType, const QString &meanPath)
{
    TrainDatasetObject obj;
    obj.db_path = dbPath;
    obj.db_type = dbType;
    obj.mean_path = meanPath;
    if (!obj.create()) {
        return TrainDataset();
    }
    return TrainDataset(obj);
}

TrainDataset TrainDataset::create(const QVariantMap &values)
{
    TrainDataset model;
    model.setProperties(values);
    if (!model.d->create()) {
        model.d->clear();
    }
    return model;
}

TrainDataset TrainDataset::get(int id)
{
    TSqlORMapper<TrainDatasetObject> mapper;
    return TrainDataset(mapper.findByPrimaryKey(id));
}

TrainDataset TrainDataset::get(int id, int lockRevision)
{
    TSqlORMapper<TrainDatasetObject> mapper;
    TCriteria cri;
    cri.add(TrainDatasetObject::Id, id);
    cri.add(TrainDatasetObject::LockRevision, lockRevision);
    return TrainDataset(mapper.findFirst(cri));
}

int TrainDataset::count()
{
    TSqlORMapper<TrainDatasetObject> mapper;
    return mapper.findCount();
}

QList<TrainDataset> TrainDataset::getAll()
{
    return tfGetModelListByCriteria<TrainDataset, TrainDatasetObject>(TCriteria());
}

QJsonArray TrainDataset::getAllJson()
{
    QJsonArray array;
    TSqlORMapper<TrainDatasetObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<TrainDatasetObject> i(mapper); i.hasNext(); ) {
            array.append(QJsonValue(QJsonObject::fromVariantMap(TrainDataset(i.next()).toVariantMap())));
        }
    }
    return array;
}

TModelObject *TrainDataset::modelData()
{
    return d.data();
}

const TModelObject *TrainDataset::modelData() const
{
    return d.data();
}
