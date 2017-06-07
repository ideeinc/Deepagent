#include "maincontroller.h"
#include "tail.h"
#include "solverproto.h"
#include "caffecontext.h"
#include "neuralnetwork.h"
#include "logics/caffeprocess.h"
#include <TWebApplication>
#include <THttpUtility>
#include <TSystemGlobal>
#include <QThread>


MainController::MainController()
    : ApplicationController()
{ }


void MainController::index()
{
    QString path = "/home/aoyama/deepagent/log/access.log";
    // QString log = Tail(path).tail(0);
    // log.remove('\r');
    // log = THttpUtility::htmlEscape(log, Tf::Compatible);
    // log = log.replace("\n", "<br>");
    // texport(log);

    session().insert("logFile", path);
    //session().insert("tailOffset", log.length());
    render();
}


void MainController::train(const QString &no)
{
    // SolverProto sp = SolverProto::get(no.toInt());
    // if (sp.isNull()) {
    //     return;
    // }

    // auto *caffe = new CaffeProcess();
    // connect(caffe, SIGNAL(finished(int, QProcess::ExitStatus)), caffe, SLOT(cleanup()));
    // caffe->start(sp, NeuralNetwork());
    // caffe->moveToThread(Tf::app()->databaseContextMainThread());

    // session().insert("logFile", "/var/tmp/hogehoge.log");
    render();
}

// Don't remove below this line
T_DEFINE_CONTROLLER(MainController)
