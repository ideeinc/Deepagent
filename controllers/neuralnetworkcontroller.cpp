#include "neuralnetworkcontroller.h"
#include "neuralnetwork.h"


NeuralNetworkController::NeuralNetworkController(const NeuralNetworkController &)
    : ApplicationController()
{ }

void NeuralNetworkController::index()
{
    QList<NeuralNetwork> neuralNetworkList = NeuralNetwork::getAll();
    texport(neuralNetworkList);
    render();
}

void NeuralNetworkController::show(const QString &pk)
{
    auto neuralNetwork = NeuralNetwork::get(pk.toInt());
    texport(neuralNetwork);
    render();
}

void NeuralNetworkController::entry()
{
    renderEntry();
}

void NeuralNetworkController::create()
{
    if (httpRequest().method() != Tf::Post) {
        return;
    }

    auto form = httpRequest().formItems("neuralNetwork");
    auto neuralNetwork = NeuralNetwork::create(form);
    if (!neuralNetwork.isNull()) {
        QString notice = "Created successfully.";
        tflash(notice);
        redirect(urla("show", neuralNetwork.id()));
    } else {
        QString error = "Failed to create.";
        texport(error);
        renderEntry(form);
    }
}

void NeuralNetworkController::renderEntry(const QVariantMap &neuralNetwork)
{
    texport(neuralNetwork);
    render("entry");
}

void NeuralNetworkController::edit(const QString &pk)
{
    auto neuralNetwork = NeuralNetwork::get(pk.toInt());
    if (!neuralNetwork.isNull()) {
        session().insert("neuralNetwork_lockRevision", neuralNetwork.lockRevision());
        renderEdit(neuralNetwork.toVariantMap());
    } else {
        redirect(urla("entry"));
    }
}

void NeuralNetworkController::save(const QString &pk)
{
    if (httpRequest().method() != Tf::Post) {
        return;
    }

    QString error;
    int rev = session().value("neuralNetwork_lockRevision").toInt();
    auto neuralNetwork = NeuralNetwork::get(pk.toInt(), rev);
    if (neuralNetwork.isNull()) {
        error = "Original data not found. It may have been updated/removed by another transaction.";
        tflash(error);
        redirect(urla("edit", pk));
        return;
    }

    auto form = httpRequest().formItems("neuralNetwork");
    neuralNetwork.setProperties(form);
    if (neuralNetwork.save()) {
        QString notice = "Updated successfully.";
        tflash(notice);
        redirect(urla("show", pk));
    } else {
        error = "Failed to update.";
        texport(error);
        renderEdit(form);
    }
}

void NeuralNetworkController::renderEdit(const QVariantMap &neuralNetwork)
{
    texport(neuralNetwork);
    render("edit");
}

void NeuralNetworkController::remove(const QString &pk)
{
    if (httpRequest().method() != Tf::Post) {
        return;
    }

    auto neuralNetwork = NeuralNetwork::get(pk.toInt());
    neuralNetwork.remove();
    redirect(urla("index"));
}


// Don't remove below this line
T_REGISTER_CONTROLLER(neuralnetworkcontroller)
