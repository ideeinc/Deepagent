#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include <TAbstractModel>

class TModelObject;
class WorkspaceObject;
class QJsonArray;


class T_MODEL_EXPORT Workspace : public TAbstractModel
{
public:
    Workspace();
    Workspace(const Workspace &other);
    Workspace(const WorkspaceObject &object);
    ~Workspace();

    int id() const;
    int solverProtoId() const;
    void setSolverProtoId(int solverProtoId);
    int neuralNetId() const;
    void setNeuralNetId(int neuralNetId);
    QString logPath() const;
    void setLogPath(const QString &logPath);
    QDateTime createdAt() const;
    QDateTime updatedAt() const;
    int lockRevision() const;
    Workspace &operator=(const Workspace &other);

    bool create() { return TAbstractModel::create(); }
    bool update() { return TAbstractModel::update(); }
    bool save()   { return TAbstractModel::save(); }
    bool remove() { return TAbstractModel::remove(); }

    static Workspace create(int solverProtoId, int neuralNetId, const QString &logPath);
    static Workspace create(const QVariantMap &values);
    static Workspace get(int id);
    static Workspace get(int id, int lockRevision);
    static int count();
    static QList<Workspace> getAll();
    static QJsonArray getAllJson();

private:
    QSharedDataPointer<WorkspaceObject> d;

    TModelObject *modelData();
    const TModelObject *modelData() const;
};

Q_DECLARE_METATYPE(Workspace)
Q_DECLARE_METATYPE(QList<Workspace>)

#endif // WORKSPACE_H
