#include <TreeFrogModel>
#include "pretrainedmodel.h"
#include "pretrainedmodelobject.h"


PretrainedModel::PretrainedModel()
    : CaffeData(), d(new PretrainedModelObject())
{ }

PretrainedModel::PretrainedModel(const PretrainedModel &other)
    : CaffeData(other), d(new PretrainedModelObject(*other.d))
{ }

// PretrainedModel::PretrainedModel(const PretrainedModelObject &object)
//     : CaffeData(), d(new PretrainedModelObject(object))
// { }

PretrainedModel::~PretrainedModel()
{
    // If the reference count becomes 0,
    // the shared data object 'PretrainedModelObject' is deleted.
}

// QString PretrainedModel::id() const
// {
//     return d->id;
// }

// void PretrainedModel::setId(const QString &id)
// {
//     d->id = id;
// }

QString PretrainedModel::caffeModelId() const
{
    return d->caffe_model_id;
}

void PretrainedModel::setCaffeModelId(const QString &caffeModelId)
{
    d->caffe_model_id = caffeModelId;
}

QString PretrainedModel::pretrainedModelFile() const
{
    return d->pretrained_model_file;
}

void PretrainedModel::setPretrainedModelFile(const QString &pretrainedModelFile)
{
    d->pretrained_model_file = pretrainedModelFile;
}

PretrainedModel &PretrainedModel::operator=(const PretrainedModel &other)
{
    CaffeData::operator=(other);
    d = other.d;  // increments the reference count of the data
    return *this;
}

// PretrainedModel PretrainedModel::create(const QString &id, const QString &caffeModelId, const QString &pretrainedModelFile)
// {
//     PretrainedModelObject obj;
//     obj.id = id;
//     obj.caffe_model_id = caffeModelId;
//     obj.pretrained_model_file = pretrainedModelFile;
//     if (!obj.create()) {
//         return PretrainedModel();
//     }
//     return PretrainedModel(obj);
// }

bool PretrainedModel::create()
{
    setId(QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));
    // json
    auto values = toVariantMap();
    values.unite(CaffeData::toVariantMap());
    writeJson(jsonFilePath(), values);
    return true;
}


PretrainedModel PretrainedModel::create(const QVariantMap &values)
{
    PretrainedModel model;
    model.setProperties(values);
    if (! model.create()) {
        model.d->clear();
    }
    return model;
}

void PretrainedModel::clear()
{
    CaffeData::clear();
    d->clear();
}

PretrainedModel PretrainedModel::get(const QString &id)
{
    return CaffeData::get<PretrainedModel>(id);
}

QList<PretrainedModel> PretrainedModel::getAll()
{
    return CaffeData::getAll<PretrainedModel>();
}

QJsonArray PretrainedModel::getAllJson()
{
    // QJsonArray array;
    // TSqlORMapper<PretrainedModelObject> mapper;

    // if (mapper.find() > 0) {
    //     for (TSqlORMapperIterator<PretrainedModelObject> i(mapper); i.hasNext(); ) {
    //         array.append(QJsonValue(QJsonObject::fromVariantMap(PretrainedModel(i.next()).toVariantMap())));
    //     }
    // }
    // return array;
}

void PretrainedModel::setProperties(const QVariantMap &properties)
{
    CaffeData::setProperties(properties);
    TAbstractModel::setProperties(properties);
}

QVariantMap PretrainedModel::toVariantMap() const
{
    auto ret = CaffeData::toVariantMap().unite(TAbstractModel::toVariantMap());
    return ret;
}

TModelObject *PretrainedModel::modelData()
{
    return d.data();
}

const TModelObject *PretrainedModel::modelData() const
{
    return d.data();
}

QDataStream &operator<<(QDataStream &ds, const PretrainedModel &model)
{
    auto varmap = model.toVariantMap();
    ds << varmap;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, PretrainedModel &model)
{
    QVariantMap varmap;
    ds >> varmap;
    model.setProperties(varmap);
    return ds;
}

// Don't remove below this line
T_REGISTER_STREAM_OPERATORS(PretrainedModel)
