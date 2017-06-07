#include <TreeFrogModel>
#include "dataset.h"
#include "datasetobject.h"
#include <typeinfo>

Dataset::Dataset()
    : CaffeData(), d(new DatasetObject())
{
    d->image_width = 0;
    d->image_height = 0;
    d->mean_file = "mean.binaryproto";
    d->label_file = "label.txt";
    d->train_db_path = "train_db";
    d->val_db_path = "val_db";
    d->log_file = "create_db.log";
}

Dataset::Dataset(const Dataset &other)
    : CaffeData(*(CaffeData*)&other), d(new DatasetObject(*other.d))
{ }

// Dataset::Dataset(const DatasetObject &object)
//     : CaffeData(), d(new DatasetObject(object))
// { }

Dataset::~Dataset()
{
    // If the reference count becomes 0,
    // the shared data object 'DatasetObject' is deleted.
}

// QString Dataset::id() const
// {
//     return d->id;
// }

// void Dataset::setId(const QString &id)
// {
//     d->id = id;
// }

int Dataset::imageWidth() const
{
    return d->image_width;
}

void Dataset::setImageWidth(int imageWidth)
{
    d->image_width = imageWidth;
}

int Dataset::imageHeight() const
{
    return d->image_height;
}

void Dataset::setImageHeight(int imageHeight)
{
    d->image_height = imageHeight;
}

QString Dataset::meanFile() const
{
    return d->mean_file;
}

void Dataset::setMeanFile(const QString &meanFile)
{
    d->mean_file = meanFile;
}

QString Dataset::labelFile() const
{
    return d->label_file;
}

void Dataset::setLabelFile(const QString &labelFile)
{
    d->label_file = labelFile;
}

QString Dataset::trainDbPath() const
{
    return d->train_db_path;
}

void Dataset::setTrainDbPath(const QString &trainDbPath)
{
    d->train_db_path = trainDbPath;
}

QString Dataset::valDbPath() const
{
    return d->val_db_path;
}

void Dataset::setValDbPath(const QString &valDbPath)
{
    d->val_db_path = valDbPath;
}

QString Dataset::logFile() const
{
    return d->log_file;
}

void Dataset::setLogFile(const QString &logFile)
{
    d->log_file = logFile;
}

Dataset &Dataset::operator=(const Dataset &other)
{
    CaffeData::operator=(other);
    d = other.d;  // increments the reference count of the data
    return *this;
}

// Dataset Dataset::create(const QString &id, int imageWidth, int imageHeight, const QString &meanFile, const QString &labelFile, const QString &valDbPath, const QString &logFile)
// {
//     DatasetObject obj;
//     obj.id = id;
//     obj.image_width = imageWidth;
//     obj.image_height = imageHeight;
//     obj.mean_file = meanFile;
//     obj.label_file = labelFile;
//     obj.val_db_path = valDbPath;
//     obj.log_file = logFile;
//     if (!obj.create()) {
//         return Dataset();
//     }
//     return Dataset(obj);
// }

bool Dataset::create()
{
    setId(QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));
    // json
    auto values = toVariantMap();
    values.unite(CaffeData::toVariantMap());
    writeJson(jsonFilePath(), values);
    return true;
}

Dataset Dataset::create(const QVariantMap &values)
{
    Dataset model;
    model.setProperties(values);
    if (! model.create()) {
        model.d->clear();
    }
    return model;
}

void Dataset::clear()
{
    CaffeData::clear();
    d->clear();
}

Dataset Dataset::get(const QString &id)
{
    return CaffeData::get<Dataset>(id);
}

QList<Dataset> Dataset::getAll()
{
    return CaffeData::getAll<Dataset>();
}

QJsonArray Dataset::getAllJson()
{
    // QJsonArray array;
    // TSqlORMapper<DatasetObject> mapper;

    // if (mapper.find() > 0) {
    //     for (TSqlORMapperIterator<DatasetObject> i(mapper); i.hasNext(); ) {
    //         array.append(QJsonValue(QJsonObject::fromVariantMap(Dataset(i.next()).toVariantMap())));
    //     }
    // }
    // return array;
}

void Dataset::setProperties(const QVariantMap &properties)
{
    CaffeData::setProperties(properties);
    TAbstractModel::setProperties(properties);
}


QVariantMap Dataset::toVariantMap() const
{
    auto ret = CaffeData::toVariantMap().unite(TAbstractModel::toVariantMap());
    return ret;
}

TModelObject *Dataset::modelData()
{
    return d.data();
}

const TModelObject *Dataset::modelData() const
{
    return d.data();
}

QDataStream &operator<<(QDataStream &ds, const Dataset &model)
{
    auto varmap = model.toVariantMap();
    ds << varmap;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, Dataset &model)
{
    QVariantMap varmap;
    ds >> varmap;
    model.setProperties(varmap);
    return ds;
}

// Don't remove below this line
T_REGISTER_STREAM_OPERATORS(Dataset)
