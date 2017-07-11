#ifndef CAFFETRAINEDMODEL_H
#define CAFFETRAINEDMODEL_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include <TAbstractModel>
#include "neuralnetwork.h"
#include "dataset.h"

class TModelObject;
class CaffeTrainedModelObject;
class QJsonArray;


class T_MODEL_EXPORT CaffeTrainedModel : public TAbstractModel
{
public:
    CaffeTrainedModel();
    CaffeTrainedModel(const CaffeTrainedModel &other);
    CaffeTrainedModel(const CaffeTrainedModelObject &object);
    ~CaffeTrainedModel();

    int id() const;
    QString modelPath() const;
    void setModelPath(const QString &modelPath);
    QString neuralNetworkName() const;
    void setNeuralNetworkName(const QString &neuralNetworkName);
    int epoch() const;
    void setEpoch(int epoch);
    qint64 datasetId() const;
    void setDatasetId(qint64 datasetId);
    int imgWidth() const;
    void setImgWidth(int imgWidth);
    int imgHeight() const;
    void setImgHeight(int imgHeight);
    QString note() const;
    void setNote(const QString &note);
    QDateTime updatedAt() const;
    int lockRevision() const;
    CaffeTrainedModel &operator=(const CaffeTrainedModel &other);

    NeuralNetwork getNeuralNetwork() const;
    Dataset getDataset() const;

    bool create() { return TAbstractModel::create(); }
    bool update() { return TAbstractModel::update(); }
    bool save()   { return TAbstractModel::save(); }
    bool remove() { return TAbstractModel::remove(); }

    static CaffeTrainedModel create(const QString &modelPath, const QString &neuralNetworkName, int epoch, int datasetId, int imgWidth, int imgHeight, const QString &note);
    static CaffeTrainedModel create(const QVariantMap &values);
    static CaffeTrainedModel get(int id);
    static CaffeTrainedModel get(int id, int lockRevision);
    static int count();
    static QList<CaffeTrainedModel> getAll(Tf::SortOrder sortOrder = Tf::DescendingOrder);
    static QJsonArray getAllJson();

private:
    QSharedDataPointer<CaffeTrainedModelObject> d;

    TModelObject *modelData();
    const TModelObject *modelData() const;
};

Q_DECLARE_METATYPE(CaffeTrainedModel)
Q_DECLARE_METATYPE(QList<CaffeTrainedModel>)

#endif // CAFFETRAINEDMODEL_H
