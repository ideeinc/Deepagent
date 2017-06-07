#ifndef TRAINDATASET_H
#define TRAINDATASET_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include <TAbstractModel>

class TModelObject;
class TrainDatasetObject;
class QJsonArray;


class T_MODEL_EXPORT TrainDataset : public TAbstractModel
{
public:
    TrainDataset();
    TrainDataset(const TrainDataset &other);
    TrainDataset(const TrainDatasetObject &object);
    ~TrainDataset();

    int id() const;
    QString dbPath() const;
    void setDbPath(const QString &dbPath);
    QString dbType() const;
    void setDbType(const QString &dbType);
    QString meanPath() const;
    void setMeanPath(const QString &meanPath);
    QDateTime updatedAt() const;
    int lockRevision() const;
    TrainDataset &operator=(const TrainDataset &other);

    bool create() { return TAbstractModel::create(); }
    bool update() { return TAbstractModel::update(); }
    bool save()   { return TAbstractModel::save(); }
    bool remove() { return TAbstractModel::remove(); }

    static TrainDataset create(const QString &dbPath, const QString &dbType, const QString &meanPath);
    static TrainDataset create(const QVariantMap &values);
    static TrainDataset get(int id);
    static TrainDataset get(int id, int lockRevision);
    static int count();
    static QList<TrainDataset> getAll();
    static QJsonArray getAllJson();

private:
    QSharedDataPointer<TrainDatasetObject> d;

    TModelObject *modelData();
    const TModelObject *modelData() const;
};

Q_DECLARE_METATYPE(TrainDataset)
Q_DECLARE_METATYPE(QList<TrainDataset>)

#endif // TRAINDATASET_H
