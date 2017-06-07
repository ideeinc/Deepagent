#ifndef CLASSLABEL_H
#define CLASSLABEL_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include <TAbstractModel>

class TModelObject;
class ClassLabelObject;
class QJsonArray;


class T_MODEL_EXPORT ClassLabel : public TAbstractModel
{
public:
    ClassLabel();
    ClassLabel(const ClassLabel &other);
    ClassLabel(const ClassLabelObject &object);
    ~ClassLabel();

    int id() const;
    int datasetId() const;
    void setDatasetId(int datasetId);
    int idx() const;
    void setIdx(int idx);
    QString name() const;
    void setName(const QString &name);
    QString metaInfo() const;
    void setMetaInfo(const QString &metaInfo);
    QDateTime updatedAt() const;
    int lockRevision() const;
    ClassLabel &operator=(const ClassLabel &other);

    bool create() { return TAbstractModel::create(); }
    bool update() { return TAbstractModel::update(); }
    bool save()   { return TAbstractModel::save(); }
    bool remove() { return TAbstractModel::remove(); }

    static ClassLabel create(int datasetId, int idx, const QString &name, const QString &metaInfo);
    static ClassLabel create(const QVariantMap &values);
    static ClassLabel get(int id);
    static ClassLabel get(int id, int lockRevision);
    static int count();
    static QList<ClassLabel> getAll();
    static QList<ClassLabel> getListByDatasetId(int id);
    static QMap<int, ClassLabel> getMapByDatasetId(int id);
    static QJsonArray getAllJson();

private:
    QSharedDataPointer<ClassLabelObject> d;

    TModelObject *modelData();
    const TModelObject *modelData() const;
};

Q_DECLARE_METATYPE(ClassLabel)
Q_DECLARE_METATYPE(QList<ClassLabel>)

#endif // CLASSLABEL_H
