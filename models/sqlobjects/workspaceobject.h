#ifndef WORKSPACEOBJECT_H
#define WORKSPACEOBJECT_H

#include <TSqlObject>
#include <QSharedData>


class T_MODEL_EXPORT WorkspaceObject : public TSqlObject, public QSharedData
{
public:
    int id {0};
    int solver_proto_id {0};
    int neural_net_id {0};
    QString log_path;
    QDateTime created_at;
    QDateTime updated_at;
    int lock_revision {0};

    enum PropertyIndex {
        Id = 0,
        SolverProtoId,
        NeuralNetId,
        LogPath,
        CreatedAt,
        UpdatedAt,
        LockRevision,
    };

    int primaryKeyIndex() const override { return Id; }
    int autoValueIndex() const override { return Id; }
    QString tableName() const override { return QLatin1String("workspace"); }

private:    /*** Don't modify below this line ***/
    Q_OBJECT
    Q_PROPERTY(int id READ getid WRITE setid)
    T_DEFINE_PROPERTY(int, id)
    Q_PROPERTY(int solver_proto_id READ getsolver_proto_id WRITE setsolver_proto_id)
    T_DEFINE_PROPERTY(int, solver_proto_id)
    Q_PROPERTY(int neural_net_id READ getneural_net_id WRITE setneural_net_id)
    T_DEFINE_PROPERTY(int, neural_net_id)
    Q_PROPERTY(QString log_path READ getlog_path WRITE setlog_path)
    T_DEFINE_PROPERTY(QString, log_path)
    Q_PROPERTY(QDateTime created_at READ getcreated_at WRITE setcreated_at)
    T_DEFINE_PROPERTY(QDateTime, created_at)
    Q_PROPERTY(QDateTime updated_at READ getupdated_at WRITE setupdated_at)
    T_DEFINE_PROPERTY(QDateTime, updated_at)
    Q_PROPERTY(int lock_revision READ getlock_revision WRITE setlock_revision)
    T_DEFINE_PROPERTY(int, lock_revision)
};

#endif // WORKSPACEOBJECT_H
