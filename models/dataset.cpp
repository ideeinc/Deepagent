#include <TreeFrogModel>
#include "dataset.h"
#include "datasetobject.h"

Dataset::Dataset()
    : TAbstractModel(), d(new DatasetObject())
{
    d->id = 0;
    d->lock_revision = 0;
}

Dataset::Dataset(const Dataset &other)
    : TAbstractModel(), d(new DatasetObject(*other.d))
{ }

Dataset::Dataset(const DatasetObject &object)
    : TAbstractModel(), d(new DatasetObject(object))
{ }

Dataset::~Dataset()
{
    // If the reference count becomes 0,
    // the shared data object 'DatasetObject' is deleted.
}

int Dataset::id() const
{
    return d->id;
}

QString Dataset::dbPath() const
{
    return d->db_path;
}

void Dataset::setDbPath(const QString &dbPath)
{
    d->db_path = dbPath;
}

QString Dataset::dbType() const
{
    return d->db_type;
}

void Dataset::setDbType(const QString &dbType)
{
    d->db_type = dbType;
}

QString Dataset::meanPath() const
{
    return d->mean_path;
}

void Dataset::setMeanPath(const QString &meanPath)
{
    d->mean_path = meanPath;
}

QDateTime Dataset::updatedAt() const
{
    return d->updated_at;
}

int Dataset::lockRevision() const
{
    return d->lock_revision;
}

Dataset &Dataset::operator=(const Dataset &other)
{
    d = other.d;  // increments the reference count of the data
    return *this;
}

Dataset Dataset::create(const QString &dbPath, const QString &dbType, const QString &meanPath)
{
    DatasetObject obj;
    obj.db_path = dbPath;
    obj.db_type = dbType;
    obj.mean_path = meanPath;
    if (!obj.create()) {
        return Dataset();
    }
    return Dataset(obj);
}

Dataset Dataset::create(const QVariantMap &values)
{
    Dataset model;
    model.setProperties(values);
    if (!model.d->create()) {
        model.d->clear();
    }
    return model;
}

Dataset Dataset::get(int id)
{
    TSqlORMapper<DatasetObject> mapper;
    return Dataset(mapper.findByPrimaryKey(id));
}

Dataset Dataset::get(int id, int lockRevision)
{
    TSqlORMapper<DatasetObject> mapper;
    TCriteria cri;
    cri.add(DatasetObject::Id, id);
    cri.add(DatasetObject::LockRevision, lockRevision);
    return Dataset(mapper.findFirst(cri));
}

int Dataset::count()
{
    TSqlORMapper<DatasetObject> mapper;
    return mapper.findCount();
}

QList<Dataset> Dataset::getAll()
{
    return tfGetModelListByCriteria<Dataset, DatasetObject>(TCriteria());
}

QJsonArray Dataset::getAllJson()
{
    QJsonArray array;
    TSqlORMapper<DatasetObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<DatasetObject> i(mapper); i.hasNext(); ) {
            array.append(QJsonValue(QJsonObject::fromVariantMap(Dataset(i.next()).toVariantMap())));
        }
    }
    return array;
}

TModelObject *Dataset::modelData()
{
    return d.data();
}

const TModelObject *Dataset::modelData() const
{
    return d.data();
}
