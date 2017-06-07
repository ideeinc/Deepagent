#ifndef TRAININGMODEL_H
#define TRAININGMODEL_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include <TAbstractModel>

class TModelObject;
class TrainingModelObject;
class QJsonArray;


class T_MODEL_EXPORT TrainingModel : public TAbstractModel
{
public:
    TrainingModel();
    TrainingModel(const TrainingModel &other);
    //TrainingModel(const TrainingModelObject &object);
    ~TrainingModel();

    QString id() const;
    QString name() const;
    void setName(const QString &name);
    QString solverPrototxt() const;
    void setSolverPrototxt(const QString &solverPrototxt);
    QString solverPrototxtPath() const;
    QString trainPrototxt() const;
    void setTrainPrototxt(const QString &trainPrototxt);
    QString trainPrototxtPath() const;
    QString deployPrototxt() const;
    void setDeployPrototxt(const QString &deployPrototxt);
    QString deployPrototxtPath() const;
    QString pretrainedModel() const;
    void setPretrainedModel(const QString &pretrainedModel);
    QString logPath() const;
    void setLogPath(const QString &logPath);
    QString state() const;
    void setState(const QString &state);
    QString createdAt() const;
    TrainingModel &operator=(const TrainingModel &other);

    QString caffeInfoLogPath() const;

    bool create();
    bool update();
    bool save();
    bool remove();
    void clear();
    bool isNull() const;

    // static TrainingModel create(const QString &name, const QString &solverPrototxt, const QString &trainPrototxt, const QString &deployPrototxt, const QString &pretrainedModel, const QString &logPath, const QString &state);
    static TrainingModel create(const QVariantMap &values);
    static TrainingModel get(const QString &id);
    static int count();
    static QList<TrainingModel> getAll();
    // static QJsonArray getAllJson();

private:
    QSharedDataPointer<TrainingModelObject> d;

    TModelObject *modelData();
    const TModelObject *modelData() const;
};

QDataStream &operator<<(QDataStream &ds, const TrainingModel &model);
QDataStream &operator>>(QDataStream &ds, TrainingModel &model);
Q_DECLARE_METATYPE(TrainingModel)
Q_DECLARE_METATYPE(QList<TrainingModel>)

#endif // TRAININGMODEL_H
