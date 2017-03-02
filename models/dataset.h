#ifndef DATASET_H
#define DATASET_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include <TAbstractModel>

class TModelObject;
class DatasetObject;
class QJsonArray;


class T_MODEL_EXPORT Dataset : public TAbstractModel
{
public:
    Dataset();
    Dataset(const Dataset &other);
    Dataset(const DatasetObject &object);
    ~Dataset();

    int id() const;
    QString dbPath() const;
    void setDbPath(const QString &dbPath);
    QString dbType() const;
    void setDbType(const QString &dbType);
    QString meanPath() const;
    void setMeanPath(const QString &meanPath);
    QDateTime updatedAt() const;
    int lockRevision() const;
    Dataset &operator=(const Dataset &other);

    bool create() { return TAbstractModel::create(); }
    bool update() { return TAbstractModel::update(); }
    bool save()   { return TAbstractModel::save(); }
    bool remove() { return TAbstractModel::remove(); }

    static Dataset create(const QString &dbPath, const QString &dbType, const QString &meanPath);
    static Dataset create(const QVariantMap &values);
    static Dataset get(int id);
    static Dataset get(int id, int lockRevision);
    static int count();
    static QList<Dataset> getAll();
    static QJsonArray getAllJson();

private:
    QSharedDataPointer<DatasetObject> d;

    TModelObject *modelData();
    const TModelObject *modelData() const;
};

Q_DECLARE_METATYPE(Dataset)
Q_DECLARE_METATYPE(QList<Dataset>)

#endif // DATASET_H
