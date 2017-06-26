#include "traincontroller.h"


TrainController::TrainController()
    : ApplicationController()
{ }

void TrainController::index()
{
    auto container = service.index();
    texport(container);
    render();
}

void TrainController::create()
{
    switch (httpRequest().method()) {
    case Tf::Get: {
        render();
        break; }

    case Tf::Post: {
        auto id = service.create(httpRequest());
        redirect(urla("show", QStringList(id)));
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void TrainController::createSsd()
{
    switch (httpRequest().method()) {
    case Tf::Get: {
        render();
        break; }

    case Tf::Post: {
        auto id = service.createSsd(httpRequest());
        redirect(urla("show", QStringList(id)));
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void TrainController::classify(const QString &id)
{
    switch (httpRequest().method()) {
    case Tf::Get:
    case Tf::Post: {
        auto container = service.classify(id, httpRequest());
        texport(container);
        render();
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void TrainController::detect(const QString &id)
{
    switch (httpRequest().method()) {
    case Tf::Get:
    case Tf::Post: {
        auto container = service.detect(id, httpRequest());
        texport(container);
        render();
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void TrainController::show(const QString &id)
{
    switch (httpRequest().method()) {
    case Tf::Get:
    case Tf::Post: {
        TrainShowContainer container = service.show(id);
        texport(container);
        render();
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void TrainController::clone(const QString &id)
{
    switch (httpRequest().method()) {
    case Tf::Get:
    case Tf::Post: {
        auto container = service.clone(id);
        texport(container);
        tflash(container);
        redirect(urla("create"));
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void TrainController::remove(const QString &id)
{
    switch (httpRequest().method()) {
    case Tf::Post: {
        service.remove(id);
        redirect(urla("index"));
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

void TrainController::getPrototxt(const QString &id, const QString &prototxt)
{
    switch (httpRequest().method()) {
    case Tf::Get:
    case Tf::Post: {
        auto data = service.getPrototxt(id, prototxt);
        sendData(data, "text/plain");
        break; }

    default:
        renderErrorResponse(Tf::NotFound);
        break;
    }
}

// Don't remove below this line
T_DEFINE_CONTROLLER(TrainController)
