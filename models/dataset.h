#ifndef DATASET_H
#define DATASET_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include "caffedata.h"

class TModelObject;
class DatasetObject;
class QJsonArray;


class T_MODEL_EXPORT Dataset : public CaffeData
{
public:
    Dataset();
    Dataset(const Dataset &other);
    //Dataset(const DatasetObject &object);
    ~Dataset();

    // QString id() const;
    // void setId(const QString &id);
    int imageWidth() const;
    void setImageWidth(int imageWidth);
    int imageHeight() const;
    void setImageHeight(int imageHeight);
    QString meanFile() const;
    void setMeanFile(const QString &meanFile);
    QString labelFile() const;
    void setLabelFile(const QString &labelFile);
    QString trainDbPath() const;
    void setTrainDbPath(const QString &trainDbPath);
    QString valDbPath() const;
    void setValDbPath(const QString &valDbPath);
    QString logFile() const;
    void setLogFile(const QString &logFile);
    Dataset &operator=(const Dataset &other);

    bool create() override;
    bool update() override;
    bool save()   override;
    void setProperties(const QVariantMap &properties) override;
    QVariantMap toVariantMap() const override;
    void clear() override;

    //static Dataset create(const QString &id, int imageWidth, int imageHeight, const QString &meanFile, const QString &labelFile, const QString &valDbPath, const QString &logFile);
    static Dataset create(const QVariantMap &values);
    static Dataset get(const QString &id);
    static int count();
    static QList<Dataset> getAll();
    //static QJsonArray getAllJson();

private:
    QSharedDataPointer<DatasetObject> d;

    TModelObject *modelData() override;
    const TModelObject *modelData() const override;
    friend class CaffeData;
    friend QDataStream &operator<<(QDataStream &ds, const Dataset &model);
    friend QDataStream &operator>>(QDataStream &ds, Dataset &model);
};

Q_DECLARE_METATYPE(Dataset)
Q_DECLARE_METATYPE(QList<Dataset>)

#endif // DATASET_H
