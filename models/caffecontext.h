#ifndef CAFFECONTEXT_H
#define CAFFECONTEXT_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include <TAbstractModel>

class TModelObject;
class CaffeContextObject;
class QJsonArray;


class T_MODEL_EXPORT CaffeContext : public TAbstractModel
{
public:
    CaffeContext();
    CaffeContext(const CaffeContext &other);
    CaffeContext(const CaffeContextObject &object);
    ~CaffeContext();

    int id() const;
    int solverProtoId() const;
    void setSolverProtoId(int solverProtoId);
    int neuralNetId() const;
    void setNeuralNetId(int neuralNetId);
    qlonglong caffePid() const;
    void setCaffePid(const qlonglong &caffePid);
    QString topic() const;
    void setTopic(const QString &topic);
    QString logPath() const;
    void setLogPath(const QString &logPath);
    QDateTime createdAt() const;
    int lockRevision() const;
    CaffeContext &operator=(const CaffeContext &other);

    bool create() { return TAbstractModel::create(); }
    bool update() { return TAbstractModel::update(); }
    bool save()   { return TAbstractModel::save(); }
    bool remove() { return TAbstractModel::remove(); }

    static CaffeContext create(int solverProtoId, int neuralNetId, const qlonglong &caffePid, const QString &topic, const QString &logPath);
    static CaffeContext create(const QVariantMap &values);
    static CaffeContext get(int id);
    static CaffeContext get(int id, int lockRevision);
    static int count();
    static QList<CaffeContext> getAll();
    static QJsonArray getAllJson();

private:
    QSharedDataPointer<CaffeContextObject> d;

    TModelObject *modelData();
    const TModelObject *modelData() const;
};

Q_DECLARE_METATYPE(CaffeContext)
Q_DECLARE_METATYPE(QList<CaffeContext>)

#endif // CAFFECONTEXT_H
