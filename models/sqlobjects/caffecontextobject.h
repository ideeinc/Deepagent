#ifndef CAFFECONTEXTOBJECT_H
#define CAFFECONTEXTOBJECT_H

#include <TSqlObject>
#include <QSharedData>


class T_MODEL_EXPORT CaffeContextObject : public TSqlObject, public QSharedData
{
public:
    int id {0};
    int solver_proto_id {0};
    int neural_net_id {0};
    qlonglong caffe_pid {0};
    QString topic;
    QString log_path;
    QDateTime created_at;
    int lock_revision {0};

    enum PropertyIndex {
        Id = 0,
        SolverProtoId,
        NeuralNetId,
        CaffePid,
        Topic,
        LogPath,
        CreatedAt,
        LockRevision,
    };

    int primaryKeyIndex() const override { return Id; }
    int autoValueIndex() const override { return Id; }
    QString tableName() const override { return QLatin1String("caffe_context"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(int id READ getid WRITE setid)
    T_DEFINE_PROPERTY(int, id)
    Q_PROPERTY(int solver_proto_id READ getsolver_proto_id WRITE setsolver_proto_id)
    T_DEFINE_PROPERTY(int, solver_proto_id)
    Q_PROPERTY(int neural_net_id READ getneural_net_id WRITE setneural_net_id)
    T_DEFINE_PROPERTY(int, neural_net_id)
    Q_PROPERTY(qlonglong caffe_pid READ getcaffe_pid WRITE setcaffe_pid)
    T_DEFINE_PROPERTY(qlonglong, caffe_pid)
    Q_PROPERTY(QString topic READ gettopic WRITE settopic)
    T_DEFINE_PROPERTY(QString, topic)
    Q_PROPERTY(QString log_path READ getlog_path WRITE setlog_path)
    T_DEFINE_PROPERTY(QString, log_path)
    Q_PROPERTY(QDateTime created_at READ getcreated_at WRITE setcreated_at)
    T_DEFINE_PROPERTY(QDateTime, created_at)
    Q_PROPERTY(int lock_revision READ getlock_revision WRITE setlock_revision)
    T_DEFINE_PROPERTY(int, lock_revision)
};

#endif // CAFFECONTEXTOBJECT_H
