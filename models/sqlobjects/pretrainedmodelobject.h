#ifndef PRETRAINEDMODELOBJECT_H
#define PRETRAINEDMODELOBJECT_H

#include <TSqlObject>
#include <QSharedData>


class T_MODEL_EXPORT PretrainedModelObject : public TSqlObject, public QSharedData
{
public:
    QString caffe_model_id;
    QString pretrained_model_file;

    enum PropertyIndex {
        CaffeModelId = 0,
        PretrainedModelFile,
    };

    int primaryKeyIndex() const override { return -1; }
    int autoValueIndex() const override { return -1; }
    QString tableName() const override { return QLatin1String("pretrained_model"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(QString caffe_model_id READ getcaffe_model_id WRITE setcaffe_model_id)
    T_DEFINE_PROPERTY(QString, caffe_model_id)
    Q_PROPERTY(QString pretrained_model_file READ getpretrained_model_file WRITE setpretrained_model_file)
    T_DEFINE_PROPERTY(QString, pretrained_model_file)
};

#endif // PRETRAINEDMODELOBJECT_H
