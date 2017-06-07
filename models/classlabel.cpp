#include <TreeFrogModel>
#include "classlabel.h"
#include "classlabelobject.h"

ClassLabel::ClassLabel()
    : TAbstractModel(), d(new ClassLabelObject())
{
    d->id = 0;
    d->dataset_id = 0;
    d->idx = 0;
    d->lock_revision = 0;
}

ClassLabel::ClassLabel(const ClassLabel &other)
    : TAbstractModel(), d(new ClassLabelObject(*other.d))
{ }

ClassLabel::ClassLabel(const ClassLabelObject &object)
    : TAbstractModel(), d(new ClassLabelObject(object))
{ }

ClassLabel::~ClassLabel()
{
    // If the reference count becomes 0,
    // the shared data object 'ClassLabelObject' is deleted.
}

int ClassLabel::id() const
{
    return d->id;
}

int ClassLabel::datasetId() const
{
    return d->dataset_id;
}

void ClassLabel::setDatasetId(int datasetId)
{
    d->dataset_id = datasetId;
}

int ClassLabel::idx() const
{
    return d->idx;
}

void ClassLabel::setIdx(int idx)
{
    d->idx = idx;
}

QString ClassLabel::name() const
{
    return d->name;
}

void ClassLabel::setName(const QString &name)
{
    d->name = name;
}

QString ClassLabel::metaInfo() const
{
    return d->meta_info;
}

void ClassLabel::setMetaInfo(const QString &metaInfo)
{
    d->meta_info = metaInfo;
}

QDateTime ClassLabel::updatedAt() const
{
    return d->updated_at;
}

int ClassLabel::lockRevision() const
{
    return d->lock_revision;
}

ClassLabel &ClassLabel::operator=(const ClassLabel &other)
{
    d = other.d;  // increments the reference count of the data
    return *this;
}

ClassLabel ClassLabel::create(int datasetId, int idx, const QString &name, const QString &metaInfo)
{
    ClassLabelObject obj;
    obj.dataset_id = datasetId;
    obj.idx = idx;
    obj.name = name;
    obj.meta_info = metaInfo;
    if (!obj.create()) {
        return ClassLabel();
    }
    return ClassLabel(obj);
}

ClassLabel ClassLabel::create(const QVariantMap &values)
{
    ClassLabel model;
    model.setProperties(values);
    if (!model.d->create()) {
        model.d->clear();
    }
    return model;
}

ClassLabel ClassLabel::get(int id)
{
    TSqlORMapper<ClassLabelObject> mapper;
    return ClassLabel(mapper.findByPrimaryKey(id));
}

ClassLabel ClassLabel::get(int id, int lockRevision)
{
    TSqlORMapper<ClassLabelObject> mapper;
    TCriteria cri;
    cri.add(ClassLabelObject::Id, id);
    cri.add(ClassLabelObject::LockRevision, lockRevision);
    return ClassLabel(mapper.findFirst(cri));
}

int ClassLabel::count()
{
    TSqlORMapper<ClassLabelObject> mapper;
    return mapper.findCount();
}

QList<ClassLabel> ClassLabel::getAll()
{
    return tfGetModelListByCriteria<ClassLabel, ClassLabelObject>(TCriteria());
}

QList<ClassLabel> ClassLabel::getListByDatasetId(int id)
{
    TCriteria cri;
    cri.add(ClassLabelObject::DatasetId, id);
    return tfGetModelListByCriteria<ClassLabel, ClassLabelObject>(cri);
}

QMap<int, ClassLabel> ClassLabel::getMapByDatasetId(int id)
{
    QMap<int, ClassLabel> ret;
    const auto labelList = ClassLabel::getListByDatasetId(id);
    for (auto &label : labelList) {
        ret.insert(label.idx(), label);
    }
    return ret;
}

QJsonArray ClassLabel::getAllJson()
{
    QJsonArray array;
    TSqlORMapper<ClassLabelObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<ClassLabelObject> i(mapper); i.hasNext(); ) {
            array.append(QJsonValue(QJsonObject::fromVariantMap(ClassLabel(i.next()).toVariantMap())));
        }
    }
    return array;
}

TModelObject *ClassLabel::modelData()
{
    return d.data();
}

const TModelObject *ClassLabel::modelData() const
{
    return d.data();
}
