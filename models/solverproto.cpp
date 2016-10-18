#include <TreeFrogModel>
#include "solverproto.h"
#include "solverprotoobject.h"

SolverProto::SolverProto()
    : TAbstractModel(), d(new SolverProtoObject())
{ }

SolverProto::SolverProto(const SolverProto &other)
    : TAbstractModel(), d(new SolverProtoObject(*other.d))
{ }

SolverProto::SolverProto(const SolverProtoObject &object)
    : TAbstractModel(), d(new SolverProtoObject(object))
{ }

SolverProto::~SolverProto()
{
    // If the reference count becomes 0,
    // the shared data object 'SolverProtoObject' is deleted.
}

int SolverProto::id() const
{
    return d->id;
}

QString SolverProto::net() const
{
    return d->net;
}

void SolverProto::setNet(const QString &net)
{
    d->net = net;
}

QString SolverProto::testIter() const
{
    return d->test_iter;
}

void SolverProto::setTestIter(const QString &testIter)
{
    d->test_iter = testIter;
}

QString SolverProto::testInterval() const
{
    return d->test_interval;
}

void SolverProto::setTestInterval(const QString &testInterval)
{
    d->test_interval = testInterval;
}

QString SolverProto::baseLr() const
{
    return d->base_lr;
}

void SolverProto::setBaseLr(const QString &baseLr)
{
    d->base_lr = baseLr;
}

QString SolverProto::momentum() const
{
    return d->momentum;
}

void SolverProto::setMomentum(const QString &momentum)
{
    d->momentum = momentum;
}

QString SolverProto::weightDecay() const
{
    return d->weight_decay;
}

void SolverProto::setWeightDecay(const QString &weightDecay)
{
    d->weight_decay = weightDecay;
}

QString SolverProto::lrPolicy() const
{
    return d->lr_policy;
}

void SolverProto::setLrPolicy(const QString &lrPolicy)
{
    d->lr_policy = lrPolicy;
}

QString SolverProto::gamma() const
{
    return d->gamma;
}

void SolverProto::setGamma(const QString &gamma)
{
    d->gamma = gamma;
}

QString SolverProto::power() const
{
    return d->power;
}

void SolverProto::setPower(const QString &power)
{
    d->power = power;
}

QString SolverProto::display() const
{
    return d->display;
}

void SolverProto::setDisplay(const QString &display)
{
    d->display = display;
}

QString SolverProto::maxIter() const
{
    return d->max_iter;
}

void SolverProto::setMaxIter(const QString &maxIter)
{
    d->max_iter = maxIter;
}

QString SolverProto::snapshot() const
{
    return d->snapshot;
}

void SolverProto::setSnapshot(const QString &snapshot)
{
    d->snapshot = snapshot;
}

QString SolverProto::snapshotPrefix() const
{
    return d->snapshot_prefix;
}

void SolverProto::setSnapshotPrefix(const QString &snapshotPrefix)
{
    d->snapshot_prefix = snapshotPrefix;
}

QString SolverProto::solverMode() const
{
    return d->solver_mode;
}

void SolverProto::setSolverMode(const QString &solverMode)
{
    d->solver_mode = solverMode;
}

SolverProto &SolverProto::operator=(const SolverProto &other)
{
    d = other.d;  // increments the reference count of the data
    return *this;
}

SolverProto SolverProto::create(const QVariantMap &values)
{
    SolverProto model;
    model.setProperties(values);
    if (!model.d->create()) {
        model.d->clear();
    }
    return model;
}

SolverProto SolverProto::get(int id)
{
    TSqlORMapper<SolverProtoObject> mapper;
    return SolverProto(mapper.findByPrimaryKey(id));
}

int SolverProto::count()
{
    TSqlORMapper<SolverProtoObject> mapper;
    return mapper.findCount();
}

QList<SolverProto> SolverProto::getAll()
{
    return tfGetModelListByCriteria<SolverProto, SolverProtoObject>(TCriteria());
}

QJsonArray SolverProto::getAllJson()
{
    QJsonArray array;
    TSqlORMapper<SolverProtoObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<SolverProtoObject> i(mapper); i.hasNext(); ) {
            array.append(QJsonValue(QJsonObject::fromVariantMap(SolverProto(i.next()).toVariantMap())));
        }
    }
    return array;
}

TModelObject *SolverProto::modelData()
{
    return d.data();
}

const TModelObject *SolverProto::modelData() const
{
    return d.data();
}


QString SolverProto::toText() const
{
    QString ret;
    ret += "net: \"" + net() + "\"\n";
    ret += "test_iter: " + testIter() + '\n';
    ret += "test_interval: " + testInterval() + '\n';
    ret += "base_lr: " + baseLr() + '\n';
    ret += "momentum: " + momentum() + '\n';
    ret += "weight_decay: " + weightDecay() + '\n';
    ret += "lr_policy: \"" + lrPolicy() + "\"\n";
    ret += "gamma: " + gamma() + '\n';
    ret += "power: " + power() + '\n';
    ret += "display: " + display() + '\n';
    ret += "max_iter: " + maxIter() + '\n';
    ret += "snapshot: " + snapshot() + '\n';
    ret += "snapshot_prefix: \"" + snapshotPrefix() + "\"\n";
    ret += "solver_mode: " + solverMode() + '\n';
    return ret;
}
