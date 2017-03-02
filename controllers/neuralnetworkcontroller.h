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
    void show(const QString &id);
    void create();
    void save(const QString &id);
    void remove(const QString &id);
};

T_DECLARE_CONTROLLER(NeuralNetworkController, neuralnetworkcontroller)

#endif // NEURALNETWORKCONTROLLER_H
