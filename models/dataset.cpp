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
    // For detectoin
    d->num_classes = 0;
    d->num_train_image = 0;
    d->num_val_image = 0;
    d->name_size_file = "name_size.txt";
    d->label_map_file = "labelmap.txt";
    CaffeData::setDataType(className<Dataset>());
}

Dataset::Dataset(const Dataset &other)
    : CaffeData(*(CaffeData*)&other), d(new DatasetObject(*other.d))
{ }

Dataset::~Dataset()
{
    // If the reference count becomes 0,
    // the shared data object 'DatasetObject' is deleted.
}

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

int Dataset::numClasses() const
{
    return d->num_classes;
}

void Dataset::setNumClasses(int numClasses)
{
    d->num_classes = numClasses;
}

void Dataset::setNumTrainImage(int numTrainImage)
{
   d->num_train_image = numTrainImage;
}

int Dataset::numValImage() const
{
    return d->num_val_image;
}

void Dataset::setNumValImage(int numValImage)
{
    d->num_val_image = numValImage;
}

QString Dataset::nameSizeFile() const
{
    return d->name_size_file;
}

void Dataset::setNameSizeFile(const QString &nameSizeFile)
{
    d->name_size_file = nameSizeFile;
}

QString Dataset::labelMapFile() const
{
    return d->label_map_file;
}

void Dataset::setLabelMapFile(const QString &labelMapFile)
{
    d->label_map_file = labelMapFile;
}

Dataset &Dataset::operator=(const Dataset &other)
{
    CaffeData::operator=(other);
    d = other.d;  // increments the reference count of the data
    return *this;
}

bool Dataset::create()
{
    setId(QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));
    return update();
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

bool Dataset::update()
{
    if (id().isEmpty()) {
        return false;
    }

    // json
    auto values = toVariantMap();
    values.unite(CaffeData::toVariantMap());
    CaffeData::setDataType(className<Dataset>());  // data type name
    writeJson(jsonFilePath(), values);
    return true;
}

bool Dataset::save()
{
    return (id().isEmpty()) ? create() : update();
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

// QJsonArray Dataset::getAllJson()
// {
//     QJsonArray array;
//     TSqlORMapper<DatasetObject> mapper;

//     if (mapper.find() > 0) {
//         for (TSqlORMapperIterator<DatasetObject> i(mapper); i.hasNext(); ) {
//             array.append(QJsonValue(QJsonObject::fromVariantMap(Dataset(i.next()).toVariantMap())));
//         }
//     }
//     return array;
// }

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
