#ifndef TRAININGMODELOBJECT_H
#define TRAININGMODELOBJECT_H

#include <TSqlObject>
#include <QSharedData>


class T_MODEL_EXPORT TrainingModelObject : public TSqlObject, public QSharedData
{
public:
    QString id;
    QString name;
    QString solver_prototxt;
    QString train_prototxt;
    QString deploy_prototxt;
    QString pretrained_model;
    QString log_path;
    QString state;
    QString created_at;

    enum PropertyIndex {
        Id = 0,
        Name,
        SolverPrototxt,
        TrainPrototxt,
        DeployPrototxt,
        PretrainedModel,
        LogPath,
        State,
        CreatedAt,
    };

    int primaryKeyIndex() const override { return Id; }
    int autoValueIndex() const override { return -1; }
    QString tableName() const override { return QLatin1String("training_model"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(QString id READ getid WRITE setid)
    T_DEFINE_PROPERTY(QString, id)
    Q_PROPERTY(QString name READ getname WRITE setname)
    T_DEFINE_PROPERTY(QString, name)
    Q_PROPERTY(QString solver_prototxt READ getsolver_prototxt WRITE setsolver_prototxt)
    T_DEFINE_PROPERTY(QString, solver_prototxt)
    Q_PROPERTY(QString train_prototxt READ gettrain_prototxt WRITE settrain_prototxt)
    T_DEFINE_PROPERTY(QString, train_prototxt)
    Q_PROPERTY(QString deploy_prototxt READ getdeploy_prototxt WRITE setdeploy_prototxt)
    T_DEFINE_PROPERTY(QString, deploy_prototxt)
    Q_PROPERTY(QString pretrained_model READ getpretrained_model WRITE setpretrained_model)
    T_DEFINE_PROPERTY(QString, pretrained_model)
    Q_PROPERTY(QString log_path READ getlog_path WRITE setlog_path)
    T_DEFINE_PROPERTY(QString, log_path)
    Q_PROPERTY(QString state READ getstate WRITE setstate)
    T_DEFINE_PROPERTY(QString, state)
    Q_PROPERTY(QString created_at READ getcreated_at WRITE setcreated_at)
    T_DEFINE_PROPERTY(QString, created_at)
};

#endif // TRAININGMODELOBJECT_H
