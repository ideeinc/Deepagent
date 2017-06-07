#ifndef TRAINDATASETOBJECT_H
#define TRAINDATASETOBJECT_H

#include <TSqlObject>
#include <QSharedData>


class T_MODEL_EXPORT TrainDatasetObject : public TSqlObject, public QSharedData
{
public:
    int id {0};
    QString db_path;
    QString db_type;
    QString mean_path;
    QDateTime updated_at;
    int lock_revision {0};

    enum PropertyIndex {
        Id = 0,
        DbPath,
        DbType,
        MeanPath,
        UpdatedAt,
        LockRevision,
    };

    int primaryKeyIndex() const override { return Id; }
    int autoValueIndex() const override { return Id; }
    QString tableName() const override { return QLatin1String("dataset"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(int id READ getid WRITE setid)
    T_DEFINE_PROPERTY(int, id)
    Q_PROPERTY(QString db_path READ getdb_path WRITE setdb_path)
    T_DEFINE_PROPERTY(QString, db_path)
    Q_PROPERTY(QString db_type READ getdb_type WRITE setdb_type)
    T_DEFINE_PROPERTY(QString, db_type)
    Q_PROPERTY(QString mean_path READ getmean_path WRITE setmean_path)
    T_DEFINE_PROPERTY(QString, mean_path)
    Q_PROPERTY(QDateTime updated_at READ getupdated_at WRITE setupdated_at)
    T_DEFINE_PROPERTY(QDateTime, updated_at)
    Q_PROPERTY(int lock_revision READ getlock_revision WRITE setlock_revision)
    T_DEFINE_PROPERTY(int, lock_revision)
};

#endif // TRAINDATASETOBJECT_H
