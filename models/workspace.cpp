#include <TreeFrogModel>
#include "workspace.h"
#include "workspaceobject.h"

Workspace::Workspace()
    : TAbstractModel(), d(new WorkspaceObject())
{
    d->id = 0;
    d->solver_proto_id = 0;
    d->neural_net_id = 0;
    d->lock_revision = 0;
}

Workspace::Workspace(const Workspace &other)
    : TAbstractModel(), d(new WorkspaceObject(*other.d))
{ }

Workspace::Workspace(const WorkspaceObject &object)
    : TAbstractModel(), d(new WorkspaceObject(object))
{ }

Workspace::~Workspace()
{
    // If the reference count becomes 0,
    // the shared data object 'WorkspaceObject' is deleted.
}

int Workspace::id() const
{
    return d->id;
}

int Workspace::solverProtoId() const
{
    return d->solver_proto_id;
}

void Workspace::setSolverProtoId(int solverProtoId)
{
    d->solver_proto_id = solverProtoId;
}

int Workspace::neuralNetId() const
{
    return d->neural_net_id;
}

void Workspace::setNeuralNetId(int neuralNetId)
{
    d->neural_net_id = neuralNetId;
}

QString Workspace::logPath() const
{
    return d->log_path;
}

void Workspace::setLogPath(const QString &logPath)
{
    d->log_path = logPath;
}

QDateTime Workspace::createdAt() const
{
    return d->created_at;
}

QDateTime Workspace::updatedAt() const
{
    return d->updated_at;
}

int Workspace::lockRevision() const
{
    return d->lock_revision;
}

Workspace &Workspace::operator=(const Workspace &other)
{
    d = other.d;  // increments the reference count of the data
    return *this;
}

Workspace Workspace::create(int solverProtoId, int neuralNetId, const QString &logPath)
{
    WorkspaceObject obj;
    obj.solver_proto_id = solverProtoId;
    obj.neural_net_id = neuralNetId;
    obj.log_path = logPath;
    if (!obj.create()) {
        return Workspace();
    }
    return Workspace(obj);
}

Workspace Workspace::create(const QVariantMap &values)
{
    Workspace model;
    model.setProperties(values);
    if (!model.d->create()) {
        model.d->clear();
    }
    return model;
}

Workspace Workspace::get(int id)
{
    TSqlORMapper<WorkspaceObject> mapper;
    return Workspace(mapper.findByPrimaryKey(id));
}

Workspace Workspace::get(int id, int lockRevision)
{
    TSqlORMapper<WorkspaceObject> mapper;
    TCriteria cri;
    cri.add(WorkspaceObject::Id, id);
    cri.add(WorkspaceObject::LockRevision, lockRevision);
    return Workspace(mapper.findFirst(cri));
}

int Workspace::count()
{
    TSqlORMapper<WorkspaceObject> mapper;
    return mapper.findCount();
}

QList<Workspace> Workspace::getAll()
{
    return tfGetModelListByCriteria<Workspace, WorkspaceObject>(TCriteria());
}

QJsonArray Workspace::getAllJson()
{
    QJsonArray array;
    TSqlORMapper<WorkspaceObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<WorkspaceObject> i(mapper); i.hasNext(); ) {
            array.append(QJsonValue(QJsonObject::fromVariantMap(Workspace(i.next()).toVariantMap())));
        }
    }
    return array;
}

TModelObject *Workspace::modelData()
{
    return d.data();
}

const TModelObject *Workspace::modelData() const
{
    return d.data();
}
