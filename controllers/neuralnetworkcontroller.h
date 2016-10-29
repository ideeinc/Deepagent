#ifndef NEURALNETWORKCONTROLLER_H
#define NEURALNETWORKCONTROLLER_H

#include "applicationcontroller.h"


class T_CONTROLLER_EXPORT NeuralNetworkController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    NeuralNetworkController() { }
    NeuralNetworkController(const NeuralNetworkController &other);

public slots:
    void index();
    void show(const QString &pk);
    void entry();
    void create();
    void edit(const QString &pk);
    void save(const QString &pk);
    void remove(const QString &pk);

private:
    void renderEntry(const QVariantMap &neuralNetwork = QVariantMap());
    void renderEdit(const QVariantMap &neuralNetwork = QVariantMap());
};

T_DECLARE_CONTROLLER(NeuralNetworkController, neuralnetworkcontroller)

#endif // NEURALNETWORKCONTROLLER_H
