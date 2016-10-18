#ifndef SOLVERPROTO_H
#define SOLVERPROTO_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include <TAbstractModel>

class TModelObject;
class SolverProtoObject;
class QJsonArray;


class T_MODEL_EXPORT SolverProto : public TAbstractModel
{
public:
    SolverProto();
    SolverProto(const SolverProto &other);
    SolverProto(const SolverProtoObject &object);
    ~SolverProto();

    int id() const;
    QString net() const;
    void setNet(const QString &net);
    QString testIter() const;
    void setTestIter(const QString &testIter);
    QString testInterval() const;
    void setTestInterval(const QString &testInterval);
    QString baseLr() const;
    void setBaseLr(const QString &baseLr);
    QString momentum() const;
    void setMomentum(const QString &momentum);
    QString weightDecay() const;
    void setWeightDecay(const QString &weightDecay);
    QString lrPolicy() const;
    void setLrPolicy(const QString &lrPolicy);
    QString gamma() const;
    void setGamma(const QString &gamma);
    QString power() const;
    void setPower(const QString &power);
    QString display() const;
    void setDisplay(const QString &display);
    QString maxIter() const;
    void setMaxIter(const QString &maxIter);
    QString snapshot() const;
    void setSnapshot(const QString &snapshot);
    QString snapshotPrefix() const;
    void setSnapshotPrefix(const QString &snapshotPrefix);
    QString solverMode() const;
    void setSolverMode(const QString &solverMode);
    SolverProto &operator=(const SolverProto &other);

    QString toText() const;

    bool create() { return TAbstractModel::create(); }
    bool update() { return TAbstractModel::update(); }
    bool save()   { return TAbstractModel::save(); }
    bool remove() { return TAbstractModel::remove(); }

    static SolverProto create(const QVariantMap &values);
    static SolverProto get(int id);
    static int count();
    static QList<SolverProto> getAll();
    static QJsonArray getAllJson();

private:
    QSharedDataPointer<SolverProtoObject> d;

    TModelObject *modelData();
    const TModelObject *modelData() const;
};

Q_DECLARE_METATYPE(SolverProto)
Q_DECLARE_METATYPE(QList<SolverProto>)

#endif // SOLVERPROTO_H
