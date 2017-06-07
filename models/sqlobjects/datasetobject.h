#ifndef DATASETOBJECT_H
#define DATASETOBJECT_H

#include <TSqlObject>
#include <QSharedData>


class T_MODEL_EXPORT DatasetObject : public TSqlObject, public QSharedData
{
public:
    int image_width {0};
    int image_height {0};
    QString mean_file;
    QString label_file;
    QString train_db_path;
    QString val_db_path;
    QString log_file;

    enum PropertyIndex {
        ImageWidth = 0,
        ImageHeight,
        MeanFile,
        LabelFile,
        TrainDbPath,
        ValDbPath,
        LogFile,
    };

    int primaryKeyIndex() const override { return -1; }
    int autoValueIndex() const override { return -1; }
    QString tableName() const override { return QLatin1String("dataset"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(int image_width READ getimage_width WRITE setimage_width)
    T_DEFINE_PROPERTY(int, image_width)
    Q_PROPERTY(int image_height READ getimage_height WRITE setimage_height)
    T_DEFINE_PROPERTY(int, image_height)
    Q_PROPERTY(QString mean_file READ getmean_file WRITE setmean_file)
    T_DEFINE_PROPERTY(QString, mean_file)
    Q_PROPERTY(QString label_file READ getlabel_file WRITE setlabel_file)
    T_DEFINE_PROPERTY(QString, label_file)
    Q_PROPERTY(QString train_db_path READ gettrain_db_path WRITE settrain_db_path)
    T_DEFINE_PROPERTY(QString, train_db_path)
    Q_PROPERTY(QString val_db_path READ getval_db_path WRITE setval_db_path)
    T_DEFINE_PROPERTY(QString, val_db_path)
    Q_PROPERTY(QString log_file READ getlog_file WRITE setlog_file)
    T_DEFINE_PROPERTY(QString, log_file)
};

#endif // DATASETOBJECT_H
