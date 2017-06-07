#ifndef CAFFETRAINEDMODELOBJECT_H
#define CAFFETRAINEDMODELOBJECT_H

#include <TSqlObject>
#include <QSharedData>


class T_MODEL_EXPORT CaffeTrainedModelObject : public TSqlObject, public QSharedData
{
public:
    int id {0};
    QString model_path;
    QString neural_network_name;
    int epoch {0};
    int dataset_id {0};
    int img_width {0};
    int img_height {0};
    QString note;
    QDateTime updated_at;
    int lock_revision {0};

    enum PropertyIndex {
        Id = 0,
        ModelPath,
        NeuralNetworkName,
        Epoch,
        DatasetId,
        ImgWidth,
        ImgHeight,
        Note,
        UpdatedAt,
        LockRevision,
    };

    int primaryKeyIndex() const override { return Id; }
    int autoValueIndex() const override { return Id; }
    QString tableName() const override { return QLatin1String("caffe_trained_model"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(int id READ getid WRITE setid)
    T_DEFINE_PROPERTY(int, id)
    Q_PROPERTY(QString model_path READ getmodel_path WRITE setmodel_path)
    T_DEFINE_PROPERTY(QString, model_path)
    Q_PROPERTY(QString neural_network_name READ getneural_network_name WRITE setneural_network_name)
    T_DEFINE_PROPERTY(QString, neural_network_name)
    Q_PROPERTY(int epoch READ getepoch WRITE setepoch)
    T_DEFINE_PROPERTY(int, epoch)
    Q_PROPERTY(int dataset_id READ getdataset_id WRITE setdataset_id)
    T_DEFINE_PROPERTY(int, dataset_id)
    Q_PROPERTY(int img_width READ getimg_width WRITE setimg_width)
    T_DEFINE_PROPERTY(int, img_width)
    Q_PROPERTY(int img_height READ getimg_height WRITE setimg_height)
    T_DEFINE_PROPERTY(int, img_height)
    Q_PROPERTY(QString note READ getnote WRITE setnote)
    T_DEFINE_PROPERTY(QString, note)
    Q_PROPERTY(QDateTime updated_at READ getupdated_at WRITE setupdated_at)
    T_DEFINE_PROPERTY(QDateTime, updated_at)
    Q_PROPERTY(int lock_revision READ getlock_revision WRITE setlock_revision)
    T_DEFINE_PROPERTY(int, lock_revision)
};

#endif // CAFFETRAINEDMODELOBJECT_H
