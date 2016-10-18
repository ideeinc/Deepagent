#ifndef SOLVERPROTOOBJECT_H
#define SOLVERPROTOOBJECT_H

#include <TSqlObject>
#include <QSharedData>


class T_MODEL_EXPORT SolverProtoObject : public TSqlObject, public QSharedData
{
public:
    int id {0};
    QString net;
    QString test_iter;
    QString test_interval;
    QString base_lr;
    QString momentum;
    QString weight_decay;
    QString lr_policy;
    QString gamma;
    QString power;
    QString display;
    QString max_iter;
    QString snapshot;
    QString snapshot_prefix;
    QString solver_mode;

    enum PropertyIndex {
        Id = 0,
        Net,
        TestIter,
        TestInterval,
        BaseLr,
        Momentum,
        WeightDecay,
        LrPolicy,
        Gamma,
        Power,
        Display,
        MaxIter,
        Snapshot,
        SnapshotPrefix,
        SolverMode,
    };

    int primaryKeyIndex() const override { return Id; }
    int autoValueIndex() const override { return Id; }
    QString tableName() const override { return QLatin1String("solver_proto"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(int id READ getid WRITE setid)
    T_DEFINE_PROPERTY(int, id)
    Q_PROPERTY(QString net READ getnet WRITE setnet)
    T_DEFINE_PROPERTY(QString, net)
    Q_PROPERTY(QString test_iter READ gettest_iter WRITE settest_iter)
    T_DEFINE_PROPERTY(QString, test_iter)
    Q_PROPERTY(QString test_interval READ gettest_interval WRITE settest_interval)
    T_DEFINE_PROPERTY(QString, test_interval)
    Q_PROPERTY(QString base_lr READ getbase_lr WRITE setbase_lr)
    T_DEFINE_PROPERTY(QString, base_lr)
    Q_PROPERTY(QString momentum READ getmomentum WRITE setmomentum)
    T_DEFINE_PROPERTY(QString, momentum)
    Q_PROPERTY(QString weight_decay READ getweight_decay WRITE setweight_decay)
    T_DEFINE_PROPERTY(QString, weight_decay)
    Q_PROPERTY(QString lr_policy READ getlr_policy WRITE setlr_policy)
    T_DEFINE_PROPERTY(QString, lr_policy)
    Q_PROPERTY(QString gamma READ getgamma WRITE setgamma)
    T_DEFINE_PROPERTY(QString, gamma)
    Q_PROPERTY(QString power READ getpower WRITE setpower)
    T_DEFINE_PROPERTY(QString, power)
    Q_PROPERTY(QString display READ getdisplay WRITE setdisplay)
    T_DEFINE_PROPERTY(QString, display)
    Q_PROPERTY(QString max_iter READ getmax_iter WRITE setmax_iter)
    T_DEFINE_PROPERTY(QString, max_iter)
    Q_PROPERTY(QString snapshot READ getsnapshot WRITE setsnapshot)
    T_DEFINE_PROPERTY(QString, snapshot)
    Q_PROPERTY(QString snapshot_prefix READ getsnapshot_prefix WRITE setsnapshot_prefix)
    T_DEFINE_PROPERTY(QString, snapshot_prefix)
    Q_PROPERTY(QString solver_mode READ getsolver_mode WRITE setsolver_mode)
    T_DEFINE_PROPERTY(QString, solver_mode)
};

#endif // SOLVERPROTOOBJECT_H
