#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include <TAbstractModel>

class TModelObject;
class NeuralNetworkObject;
class QJsonArray;


class T_MODEL_EXPORT NeuralNetwork : public TAbstractModel
{
public:
    NeuralNetwork();
    NeuralNetwork(const NeuralNetwork &other);
    NeuralNetwork(const NeuralNetworkObject &object);
    ~NeuralNetwork();

    int id() const;
    QString name() const;
    void setName(const QString &name);
    QString type() const;
    void setType(const QString &type);
    QString absFilePath() const;
    void setAbsFilePath(const QString &absFilePath);
    QDateTime updatedAt() const;
    int lockRevision() const;
    NeuralNetwork &operator=(const NeuralNetwork &other);

    bool create() { return TAbstractModel::create(); }
    bool update() { return TAbstractModel::update(); }
    bool save()   { return TAbstractModel::save(); }
    bool remove() { return TAbstractModel::remove(); }

    static NeuralNetwork create(const QString &name, const QString &type, const QString &absFilePath);
    static NeuralNetwork create(const QVariantMap &values);
    static NeuralNetwork get(int id);
    static NeuralNetwork get(int id, int lockRevision);
    static NeuralNetwork getOneByName(const QString &name);
    static int count();
    static QList<NeuralNetwork> getAll();
    static QJsonArray getAllJson();

private:
    QSharedDataPointer<NeuralNetworkObject> d;

    TModelObject *modelData();
    const TModelObject *modelData() const;
};

Q_DECLARE_METATYPE(NeuralNetwork)
Q_DECLARE_METATYPE(QList<NeuralNetwork>)

#endif // NEURALNETWORK_H
