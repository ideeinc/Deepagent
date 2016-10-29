#include <TreeFrogModel>
#include "caffecontext.h"
#include "caffecontextobject.h"

CaffeContext::CaffeContext()
    : TAbstractModel(), d(new CaffeContextObject())
{
    d->id = 0;
    d->solver_proto_id = 0;
    d->neural_net_id = 0;
    d->caffe_pid = 0;
    d->lock_revision = 0;
}

CaffeContext::CaffeContext(const CaffeContext &other)
    : TAbstractModel(), d(new CaffeContextObject(*other.d))
{ }

CaffeContext::CaffeContext(const CaffeContextObject &object)
    : TAbstractModel(), d(new CaffeContextObject(object))
{ }

CaffeContext::~CaffeContext()
{
    // If the reference count becomes 0,
    // the shared data object 'CaffeContextObject' is deleted.
}

int CaffeContext::id() const
{
    return d->id;
}

int CaffeContext::solverProtoId() const
{
    return d->solver_proto_id;
}

void CaffeContext::setSolverProtoId(int solverProtoId)
{
    d->solver_proto_id = solverProtoId;
}

int CaffeContext::neuralNetId() const
{
    return d->neural_net_id;
}

void CaffeContext::setNeuralNetId(int neuralNetId)
{
    d->neural_net_id = neuralNetId;
}

qlonglong CaffeContext::caffePid() const
{
    return d->caffe_pid;
}

void CaffeContext::setCaffePid(const qlonglong &caffePid)
{
    d->caffe_pid = caffePid;
}

QString CaffeContext::topic() const
{
    return d->topic;
}

void CaffeContext::setTopic(const QString &topic)
{
    d->topic = topic;
}

QString CaffeContext::logPath() const
{
    return d->log_path;
}

void CaffeContext::setLogPath(const QString &logPath)
{
    d->log_path = logPath;
}

QDateTime CaffeContext::createdAt() const
{
    return d->created_at;
}

int CaffeContext::lockRevision() const
{
    return d->lock_revision;
}

CaffeContext &CaffeContext::operator=(const CaffeContext &other)
{
    d = other.d;  // increments the reference count of the data
    return *this;
}

CaffeContext CaffeContext::create(int solverProtoId, int neuralNetId, const qlonglong &caffePid, const QString &topic, const QString &logPath)
{
    CaffeContextObject obj;
    obj.solver_proto_id = solverProtoId;
    obj.neural_net_id = neuralNetId;
    obj.caffe_pid = caffePid;
    obj.topic = topic;
    obj.log_path = logPath;
    if (!obj.create()) {
        return CaffeContext();
    }
    return CaffeContext(obj);
}

CaffeContext CaffeContext::create(const QVariantMap &values)
{
    CaffeContext model;
    model.setProperties(values);
    if (!model.d->create()) {
        model.d->clear();
    }
    return model;
}

CaffeContext CaffeContext::get(int id)
{
    TSqlORMapper<CaffeContextObject> mapper;
    return CaffeContext(mapper.findByPrimaryKey(id));
}

CaffeContext CaffeContext::get(int id, int lockRevision)
{
    TSqlORMapper<CaffeContextObject> mapper;
    TCriteria cri;
    cri.add(CaffeContextObject::Id, id);
    cri.add(CaffeContextObject::LockRevision, lockRevision);
    return CaffeContext(mapper.findFirst(cri));
}

int CaffeContext::count()
{
    TSqlORMapper<CaffeContextObject> mapper;
    return mapper.findCount();
}

QList<CaffeContext> CaffeContext::getAll()
{
    return tfGetModelListByCriteria<CaffeContext, CaffeContextObject>(TCriteria());
}

QJsonArray CaffeContext::getAllJson()
{
    QJsonArray array;
    TSqlORMapper<CaffeContextObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<CaffeContextObject> i(mapper); i.hasNext(); ) {
            array.append(QJsonValue(QJsonObject::fromVariantMap(CaffeContext(i.next()).toVariantMap())));
        }
    }
    return array;
}

TModelObject *CaffeContext::modelData()
{
    return d.data();
}

const TModelObject *CaffeContext::modelData() const
{
    return d.data();
}
