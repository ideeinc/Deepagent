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
    int num_classes {0};
    int num_train_image {0};
    int num_val_image {0};
    QString name_size_file;
    QString label_map_file;

    enum PropertyIndex {
        ImageWidth = 0,
        ImageHeight,
        MeanFile,
        LabelFile,
        TrainDbPath,
        ValDbPath,
        LogFile,
        NumClasses,
        NumTrainImage,
        NumValImage,
        NameSizeFile,
        LabelMapFile,
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
    Q_PROPERTY(int num_classes READ getnum_classes WRITE setnum_classes)
    T_DEFINE_PROPERTY(int, num_classes)
    Q_PROPERTY(int num_train_image READ getnum_train_image WRITE setnum_train_image)
    T_DEFINE_PROPERTY(int, num_train_image)
    Q_PROPERTY(int num_val_image READ getnum_val_image WRITE setnum_val_image)
    T_DEFINE_PROPERTY(int, num_val_image)
    Q_PROPERTY(QString name_size_file READ getname_size_file WRITE setname_size_file)
    T_DEFINE_PROPERTY(QString, name_size_file)
    Q_PROPERTY(QString label_map_file READ getlabel_map_file WRITE setlabel_map_file)
    T_DEFINE_PROPERTY(QString, label_map_file)
};

#endif // DATASETOBJECT_H
