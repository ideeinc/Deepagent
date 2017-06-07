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


class T_MODEL_EXPORT CaffeModel : public CaffeData
{
public:
    CaffeModel();
    CaffeModel(const CaffeModel &other);
    //CaffeModel(const CaffeModelObject &object);
    ~CaffeModel();

    // QString id() const;
    // void setId(const QString &id);
    QString datasetId() const;
    void setDatasetId(const QString &datasetId);
    QString meanFile() const;
    void setMeanFile(const QString &meanFile);
    QString meanData() const;
    void setMeanData(const QString &meanData);
    QString solverFile() const;
    void setSolverFile(const QString &solverFile);
    QString solverPrototxt() const;
    void setSolverPrototxt(const QString &solverPrototxt);
    QString trainFile() const;
    void setTrainFile(const QString &trainFile);
    QString trainPrototxt() const;
    void setTrainPrototxt(const QString &trainPrototxt);
    QString deployFile() const;
    void setDeployFile(const QString &deployFile);
    QString deployPrototxt() const;
    void setDeployPrototxt(const QString &deployPrototxt);
    QString networkFile() const;
    void setNetworkFile(const QString &networkFile);
    QString networkPrototxt() const;
    void setNetworkPrototxt(const QString &networkPrototxt);
    QString trainedModelFiles() const;
    void setTrainedModelFiles(const QString &trainedModelFiles);
    QString logFile() const;
    void setLogFile(const QString &logFile);
    CaffeModel &operator=(const CaffeModel &other);

    // QString solverPrototxt() const;
    // void setSolverPrototxt(const QString &prototxt);
    // QString trainPrototxt() const;
    // void setTrainPrototxt(const QString &prototxt);
    // QString deployPrototxt() const;
    // void setDeployPrototxt(const QString &prototxt);
    // QString networkPrototxt() const;
    // void setNetworkPrototxt(const QString &prototxt);
    QString solverPrototxtPath() const;
    QString caffeInfoLogPath() const;

    bool create() override;
    bool update() override;
    bool save()   override;
    void setProperties(const QVariantMap &properties) override;
    QVariantMap toVariantMap() const override;
    void clear();

    //static CaffeModel create(const QString &id, const QString &datasetId, const QString &meanFile, const QString &solverFile, const QString &trainFile, const QString &deployFile, const QString &networkFile, const QString &trainedModelFiles, const QString &logFile);
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
