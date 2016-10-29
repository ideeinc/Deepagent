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

QString NeuralNetwork::layers() const
{
    return d->layers;
}

void NeuralNetwork::setLayers(const QString &layers)
{
    d->layers = layers;
}

QDateTime NeuralNetwork::createdAt() const
{
    return d->created_at;
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

NeuralNetwork NeuralNetwork::create(const QString &layers)
{
    NeuralNetworkObject obj;
    obj.layers = layers;
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
