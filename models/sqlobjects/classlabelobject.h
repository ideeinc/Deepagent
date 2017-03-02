#ifndef CLASSLABELOBJECT_H
#define CLASSLABELOBJECT_H

#include <TSqlObject>
#include <QSharedData>


class T_MODEL_EXPORT ClassLabelObject : public TSqlObject, public QSharedData
{
public:
    int id {0};
    int dataset_id {0};
    int idx {0};
    QString name;
    QString meta_info;
    QDateTime updated_at;
    int lock_revision {0};

    enum PropertyIndex {
        Id = 0,
        DatasetId,
        Idx,
        Name,
        MetaInfo,
        UpdatedAt,
        LockRevision,
    };

    int primaryKeyIndex() const override { return Id; }
    int autoValueIndex() const override { return Id; }
    QString tableName() const override { return QLatin1String("class_label"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(int id READ getid WRITE setid)
    T_DEFINE_PROPERTY(int, id)
    Q_PROPERTY(int dataset_id READ getdataset_id WRITE setdataset_id)
    T_DEFINE_PROPERTY(int, dataset_id)
    Q_PROPERTY(int idx READ getidx WRITE setidx)
    T_DEFINE_PROPERTY(int, idx)
    Q_PROPERTY(QString name READ getname WRITE setname)
    T_DEFINE_PROPERTY(QString, name)
    Q_PROPERTY(QString meta_info READ getmeta_info WRITE setmeta_info)
    T_DEFINE_PROPERTY(QString, meta_info)
    Q_PROPERTY(QDateTime updated_at READ getupdated_at WRITE setupdated_at)
    T_DEFINE_PROPERTY(QDateTime, updated_at)
    Q_PROPERTY(int lock_revision READ getlock_revision WRITE setlock_revision)
    T_DEFINE_PROPERTY(int, lock_revision)
};

#endif // CLASSLABELOBJECT_H
