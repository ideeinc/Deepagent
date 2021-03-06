#ifndef PRETRAINEDMODEL_H
#define PRETRAINEDMODEL_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include "caffedata.h"

class TModelObject;
class PretrainedModelObject;
class QJsonArray;


class T_MODEL_EXPORT PretrainedModel : public CaffeData
{
public:
    PretrainedModel();
    PretrainedModel(const PretrainedModel &other);
    ~PretrainedModel();

    QString caffeModelId() const;
    void setCaffeModelId(const QString &caffeModelId);
    QString pretrainedModelFile() const;
    void setPretrainedModelFile(const QString &pretrainedModelFile);
    PretrainedModel &operator=(const PretrainedModel &other);

    bool create() override;
    bool update() override;
    bool save()   override;
    void setProperties(const QVariantMap &properties) override;
    QVariantMap toVariantMap() const override;
    void clear() override;

    static PretrainedModel create(const QVariantMap &values);
    static PretrainedModel get(const QString &id);
    static int count();
    static QList<PretrainedModel> getAll();

private:
    QSharedDataPointer<PretrainedModelObject> d;

    TModelObject *modelData() override;
    const TModelObject *modelData() const override;
    friend QDataStream &operator<<(QDataStream &ds, const PretrainedModel &model);
    friend QDataStream &operator>>(QDataStream &ds, PretrainedModel &model);
};

Q_DECLARE_METATYPE(PretrainedModel)
Q_DECLARE_METATYPE(QList<PretrainedModel>)

#endif // PRETRAINEDMODEL_H
