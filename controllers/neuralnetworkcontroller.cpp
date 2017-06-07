#include "neuralnetworkcontroller.h"
#include "neuralnetwork.h"


NeuralNetworkController::NeuralNetworkController()
    : ApplicationController()
{ }

void NeuralNetworkController::index()
{
    auto neuralNetworkList = NeuralNetwork::getAll();
    texport(neuralNetworkList);
    render();
}

void NeuralNetworkController::show(const QString &id)
{
    auto neuralNetwork = NeuralNetwork::get(id.toInt());
    texport(neuralNetwork);
    render();
}

void NeuralNetworkController::create()
{
    switch (httpRequest().method()) {
    case Tf::Get:
        render();
        break;

    case Tf::Post: {
        auto neuralNetwork = httpRequest().formItems("neuralNetwork");
        auto model = NeuralNetwork::create(neuralNetwork);

        if (!model.isNull()) {
            QString notice = "Created successfully.";
            tflash(notice);
            redirect(urla("show", model.id()));
        } else {
            QString error = "Failed to create.";
            texport(error);
            texport(neuralNetwork);
            render();
        }
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void NeuralNetworkController::save(const QString &id)
{
    switch (httpRequest().method()) {
    case Tf::Get: {
        auto model = NeuralNetwork::get(id.toInt());
        if (!model.isNull()) {
            session().insert("neuralNetwork_lockRevision", model.lockRevision());
            auto neuralNetwork = model.toVariantMap();
            texport(neuralNetwork);
            render();
        }
        break; }

    case Tf::Post: {
        QString error;
        int rev = session().value("neuralNetwork_lockRevision").toInt();
        auto model = NeuralNetwork::get(id.toInt(), rev);

        if (model.isNull()) {
            error = "Original data not found. It may have been updated/removed by another transaction.";
            tflash(error);
            redirect(urla("save", id));
            break;
        }

        auto neuralNetwork = httpRequest().formItems("neuralNetwork");
        model.setProperties(neuralNetwork);
        if (model.save()) {
            QString notice = "Updated successfully.";
            tflash(notice);
            redirect(urla("show", model.id()));
        } else {
            error = "Failed to update.";
            texport(error);
            texport(neuralNetwork);
            render();
        }
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void NeuralNetworkController::remove(const QString &id)
{
    if (httpRequest().method() != Tf::Post) {
        renderErrorResponse(Tf::NotFound);
        return;
    }

    auto neuralNetwork = NeuralNetwork::get(id.toInt());
    neuralNetwork.remove();
    redirect(urla("index"));
}


// Don't remove below this line
T_DEFINE_CONTROLLER(NeuralNetworkController)
