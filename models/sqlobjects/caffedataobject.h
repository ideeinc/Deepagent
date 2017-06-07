#ifndef CAFFEDATAOBJECT_H
#define CAFFEDATAOBJECT_H

#include <TSqlObject>
#include <QSharedData>


class T_MODEL_EXPORT CaffeDataObject : public TSqlObject, public QSharedData
{
public:
    QString id;
    QString category;
    QString data_type;
    QString name;
    QString username;
    QString state;
    QString note;
    QString caffe_version;
    QString created_at;
    QString completed_at;

    enum PropertyIndex {
        Id = 0,
        Category,
        DataType,
        Name,
        Username,
        State,
        Note,
        CaffeVersion,
        CreatedAt,
        CompletedAt,
    };

    int primaryKeyIndex() const override { return Id; }
    int autoValueIndex() const override { return -1; }
    QString tableName() const override { return QLatin1String("caffe_data"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(QString id READ getid WRITE setid)
    T_DEFINE_PROPERTY(QString, id)
    Q_PROPERTY(QString category READ getcategory WRITE setcategory)
    T_DEFINE_PROPERTY(QString, category)
    Q_PROPERTY(QString data_type READ getdata_type WRITE setdata_type)
    T_DEFINE_PROPERTY(QString, data_type)
    Q_PROPERTY(QString name READ getname WRITE setname)
    T_DEFINE_PROPERTY(QString, name)
    Q_PROPERTY(QString username READ getusername WRITE setusername)
    T_DEFINE_PROPERTY(QString, username)
    Q_PROPERTY(QString state READ getstate WRITE setstate)
    T_DEFINE_PROPERTY(QString, state)
    Q_PROPERTY(QString note READ getnote WRITE setnote)
    T_DEFINE_PROPERTY(QString, note)
    Q_PROPERTY(QString caffe_version READ getcaffe_version WRITE setcaffe_version)
    T_DEFINE_PROPERTY(QString, caffe_version)
    Q_PROPERTY(QString created_at READ getcreated_at WRITE setcreated_at)
    T_DEFINE_PROPERTY(QString, created_at)
    Q_PROPERTY(QString completed_at READ getcompleted_at WRITE setcompleted_at)
    T_DEFINE_PROPERTY(QString, completed_at)
};

#endif // CAFFEDATAOBJECT_H
