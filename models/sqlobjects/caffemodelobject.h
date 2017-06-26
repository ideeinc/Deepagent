#ifndef CAFFEMODELOBJECT_H
#define CAFFEMODELOBJECT_H

#include <TSqlObject>
#include <QSharedData>


class T_MODEL_EXPORT CaffeModelObject : public TSqlObject, public QSharedData
{
public:
    QString dataset_id;
    QString solver_file;
    QString train_file;
    QString deploy_file;
    QString network_file;
    QStringList trained_model_files;
    QString log_file;

    enum PropertyIndex {
        DatasetId = 0,
        SolverFile,
        TrainFile,
        DeployFile,
        NetworkFile,
        TrainedModelFiles,
        LogFile,
    };

    int primaryKeyIndex() const override { return -1; }
    int autoValueIndex() const override { return -1; }
    QString tableName() const override { return QLatin1String("caffe_model"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(QString dataset_id READ getdataset_id WRITE setdataset_id)
    T_DEFINE_PROPERTY(QString, dataset_id)
    Q_PROPERTY(QString solver_file READ getsolver_file WRITE setsolver_file)
    T_DEFINE_PROPERTY(QString, solver_file)
    Q_PROPERTY(QString train_file READ gettrain_file WRITE settrain_file)
    T_DEFINE_PROPERTY(QString, train_file)
    Q_PROPERTY(QString deploy_file READ getdeploy_file WRITE setdeploy_file)
    T_DEFINE_PROPERTY(QString, deploy_file)
    Q_PROPERTY(QString network_file READ getnetwork_file WRITE setnetwork_file)
    T_DEFINE_PROPERTY(QString, network_file)
    Q_PROPERTY(QStringList trained_model_files READ gettrained_model_files WRITE settrained_model_files)
    T_DEFINE_PROPERTY(QStringList, trained_model_files)
    Q_PROPERTY(QString log_file READ getlog_file WRITE setlog_file)
    T_DEFINE_PROPERTY(QString, log_file)
};

#endif // CAFFEMODELOBJECT_H
