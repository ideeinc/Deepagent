#include <TreeFrogModel>
#include "neuralnetwork.h"
#include "neuralnetworkobject.h"

NeuralNetwork::NeuralNetwork()
    : TAbstractModel(), d(new NeuralNetworkObject())
{
    d->id = 0;
    d->lock_revision = 0;
}

NeuralNetwork::NeuralNetwork(const NeuralNetwork &other)
    : TAbstractModel(), d(new NeuralNetworkObject(*other.d))
{ }

NeuralNetwork::NeuralNetwork(const NeuralNetworkObject &object)
    : TAbstractModel(), d(new NeuralNetworkObject(object))
{ }

NeuralNetwork::~NeuralNetwork()
{
    // If the reference count becomes 0,
    // the shared data object 'NeuralNetworkObject' is deleted.
}

int NeuralNetwork::id() const
{
    return d->id;
}

QString NeuralNetwork::name() const
{
    return d->name;
}

void NeuralNetwork::setName(const QString &name)
{
    d->name = name;
}

QString NeuralNetwork::type() const
{
    return d->type;
}

void NeuralNetwork::setType(const QString &type)
{
    d->type = type;
}

QString NeuralNetwork::absFilePath() const
{
    return d->abs_file_path;
}

void NeuralNetwork::setAbsFilePath(const QString &absFilePath)
{
    d->abs_file_path = absFilePath;
}

QDateTime NeuralNetwork::updatedAt() const
{
    return d->updated_at;
}

int NeuralNetwork::lockRevision() const
{
    return d->lock_revision;
}

NeuralNetwork &NeuralNetwork::operator=(const NeuralNetwork &other)
{
    d = other.d;  // increments the reference count of the data
    return *this;
}

NeuralNetwork NeuralNetwork::create(const QString &name, const QString &type, const QString &absFilePath)
{
    NeuralNetworkObject obj;
    obj.name = name;
    obj.type = type;
    obj.abs_file_path = absFilePath;
    if (!obj.create()) {
        return NeuralNetwork();
    }
    return NeuralNetwork(obj);
}

NeuralNetwork NeuralNetwork::create(const QVariantMap &values)
{
    NeuralNetwork model;
    model.setProperties(values);
    if (!model.d->create()) {
        model.d->clear();
    }
    return model;
}

NeuralNetwork NeuralNetwork::get(int id)
{
    TSqlORMapper<NeuralNetworkObject> mapper;
    return NeuralNetwork(mapper.findByPrimaryKey(id));
}

NeuralNetwork NeuralNetwork::get(int id, int lockRevision)
{
    TSqlORMapper<NeuralNetworkObject> mapper;
    TCriteria cri;
    cri.add(NeuralNetworkObject::Id, id);
    cri.add(NeuralNetworkObject::LockRevision, lockRevision);
    return NeuralNetwork(mapper.findFirst(cri));
}

NeuralNetwork NeuralNetwork::getOneByName(const QString &name)
{
    TSqlORMapper<NeuralNetworkObject> mapper;
    TCriteria cri;
    cri.add(NeuralNetworkObject::Name, name);
    return NeuralNetwork(mapper.findFirst(cri));
}

int NeuralNetwork::count()
{
    TSqlORMapper<NeuralNetworkObject> mapper;
    return mapper.findCount();
}

QList<NeuralNetwork> NeuralNetwork::getAll()
{
    return tfGetModelListByCriteria<NeuralNetwork, NeuralNetworkObject>(TCriteria());
}

QJsonArray NeuralNetwork::getAllJson()
{
    QJsonArray array;
    TSqlORMapper<NeuralNetworkObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<NeuralNetworkObject> i(mapper); i.hasNext(); ) {
            array.append(QJsonValue(QJsonObject::fromVariantMap(NeuralNetwork(i.next()).toVariantMap())));
        }
    }
    return array;
}

TModelObject *NeuralNetwork::modelData()
{
    return d.data();
}

const TModelObject *NeuralNetwork::modelData() const
{
    return d.data();
}
