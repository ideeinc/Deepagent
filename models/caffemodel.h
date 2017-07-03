#ifndef CAFFEMODEL_H
#define CAFFEMODEL_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include "caffedata.h"

class TModelObject;
class CaffeModelObject;
class QJsonArray;
class Dataset;


class T_MODEL_EXPORT CaffeModel : public CaffeData
{
public:
    CaffeModel();
    CaffeModel(const CaffeModel &other);
    ~CaffeModel();

    QString datasetId() const;
    void setDatasetId(const QString &datasetId);
    // QString meanFile() const;
    // QString meanFilePath() const { return dirPath() % meanFile(); }
    // void setMeanFile(const QString &meanFile);
    // QString meanData() const;
    // void setMeanData(const QString &meanData);
    QString solverFile() const;
    QString solverFilePath() const { return dirPath() + solverFile(); }
    void setSolverFile(const QString &solverFile);
    QString solverPrototxt() const;
    QString trainFile() const;
    QString trainFilePath() const { return dirPath() + trainFile(); }
    void setTrainFile(const QString &trainFile);
    QString trainPrototxt() const;
    QString deployFile() const;
    QString deployFilePath() const { return dirPath() + deployFile(); }
    void setDeployFile(const QString &deployFile);
    QString deployPrototxt() const;
    QString networkFile() const;
    QString networkFilePath() const { return dirPath() + networkFile(); }
    void setNetworkFile(const QString &networkFile);
    QString networkPrototxt() const;
    QStringList trainedModelFiles() const;
    QString trainedModelFilePath(const QString &trainedModelFile) const { return dirPath() % trainedModelFile; }
    void setTrainedModelFiles(const QStringList &trainedModelFiles);
    QString logFile() const;
    void setLogFile(const QString &logFile);
    CaffeModel &operator=(const CaffeModel &other);

    QString solverPrototxtPath() const;
    QString caffeInfoLogPath() const;
    Dataset getDataset() const;

    bool create() override;
    bool update() override;
    bool save()   override;
    void setProperties(const QVariantMap &properties) override;
    QVariantMap toVariantMap() const override;
    void clear();
    static CaffeModel create(const QVariantMap &values);
    static CaffeModel get(const QString &id);
    //static int count();
    static QList<CaffeModel> getAll();
    //static QJsonArray getAllJson();

private:
    QSharedDataPointer<CaffeModelObject> d;

    TModelObject *modelData() override;
    const TModelObject *modelData() const override;
    friend QDataStream &operator<<(QDataStream &ds, const CaffeModel &model);
    friend QDataStream &operator>>(QDataStream &ds, CaffeModel &model);
};

Q_DECLARE_METATYPE(CaffeModel)
Q_DECLARE_METATYPE(QList<CaffeModel>)

#endif // CAFFEMODEL_H
