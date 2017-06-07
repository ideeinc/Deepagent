#ifndef CAFFEMODELOBJECT_H
#define CAFFEMODELOBJECT_H

#include <TSqlObject>
#include <QSharedData>


class T_MODEL_EXPORT CaffeModelObject : public TSqlObject, public QSharedData
{
public:
    QString dataset_id;
    QString mean_file;
    QString mean_data;
    QString solver_file;
    QString solver_prototxt;
    QString train_file;
    QString train_prototxt;
    QString deploy_file;
    QString deploy_prototxt;
    QString network_file;
    QString network_prototxt;
    QString trained_model_files;
    QString log_file;

    enum PropertyIndex {
        DatasetId = 0,
        MeanFile,
        MeanData,
        SolverFile,
        SolverPrototxt,
        TrainFile,
        TrainPrototxt,
        DeployFile,
        DeployPrototxt,
        NetworkFile,
        NetworkPrototxt,
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
    Q_PROPERTY(QString mean_file READ getmean_file WRITE setmean_file)
    T_DEFINE_PROPERTY(QString, mean_file)
    Q_PROPERTY(QString mean_data READ getmean_data WRITE setmean_data)
    T_DEFINE_PROPERTY(QString, mean_data)
    Q_PROPERTY(QString solver_file READ getsolver_file WRITE setsolver_file)
    T_DEFINE_PROPERTY(QString, solver_file)
    Q_PROPERTY(QString solver_prototxt READ getsolver_prototxt WRITE setsolver_prototxt)
    T_DEFINE_PROPERTY(QString, solver_prototxt)
    Q_PROPERTY(QString train_file READ gettrain_file WRITE settrain_file)
    T_DEFINE_PROPERTY(QString, train_file)
    Q_PROPERTY(QString train_prototxt READ gettrain_prototxt WRITE settrain_prototxt)
    T_DEFINE_PROPERTY(QString, train_prototxt)
    Q_PROPERTY(QString deploy_file READ getdeploy_file WRITE setdeploy_file)
    T_DEFINE_PROPERTY(QString, deploy_file)
    Q_PROPERTY(QString deploy_prototxt READ getdeploy_prototxt WRITE setdeploy_prototxt)
    T_DEFINE_PROPERTY(QString, deploy_prototxt)
    Q_PROPERTY(QString network_file READ getnetwork_file WRITE setnetwork_file)
    T_DEFINE_PROPERTY(QString, network_file)
    Q_PROPERTY(QString network_prototxt READ getnetwork_prototxt WRITE setnetwork_prototxt)
    T_DEFINE_PROPERTY(QString, network_prototxt)
    Q_PROPERTY(QString trained_model_files READ gettrained_model_files WRITE settrained_model_files)
    T_DEFINE_PROPERTY(QString, trained_model_files)
    Q_PROPERTY(QString log_file READ getlog_file WRITE setlog_file)
    T_DEFINE_PROPERTY(QString, log_file)
};

#endif // CAFFEMODELOBJECT_H
