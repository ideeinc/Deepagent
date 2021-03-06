#ifndef NEURALNETWORKOBJECT_H
#define NEURALNETWORKOBJECT_H

#include <TSqlObject>
#include <QSharedData>


class T_MODEL_EXPORT NeuralNetworkObject : public TSqlObject, public QSharedData
{
public:
    int id {0};
    QString name;
    QString type;
    QString abs_file_path;
    QDateTime updated_at;
    int lock_revision {0};

    enum PropertyIndex {
        Id = 0,
        Name,
        Type,
        AbsFilePath,
        UpdatedAt,
        LockRevision,
    };

    int primaryKeyIndex() const override { return Id; }
    int autoValueIndex() const override { return Id; }
    QString tableName() const override { return QLatin1String("neural_network"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(int id READ getid WRITE setid)
    T_DEFINE_PROPERTY(int, id)
    Q_PROPERTY(QString name READ getname WRITE setname)
    T_DEFINE_PROPERTY(QString, name)
    Q_PROPERTY(QString type READ gettype WRITE settype)
    T_DEFINE_PROPERTY(QString, type)
    Q_PROPERTY(QString abs_file_path READ getabs_file_path WRITE setabs_file_path)
    T_DEFINE_PROPERTY(QString, abs_file_path)
    Q_PROPERTY(QDateTime updated_at READ getupdated_at WRITE setupdated_at)
    T_DEFINE_PROPERTY(QDateTime, updated_at)
    Q_PROPERTY(int lock_revision READ getlock_revision WRITE setlock_revision)
    T_DEFINE_PROPERTY(int, lock_revision)
};

#endif // NEURALNETWORKOBJECT_H
