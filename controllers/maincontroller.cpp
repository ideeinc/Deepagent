#include "maincontroller.h"
#include "tail.h"
#include <TWebApplication>
#include <THttpUtility>
#include <QProcess>
#include "solverproto.h"


MainController::MainController(const MainController &)
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


void MainController::train()
{
    const QString CaffeCammand = "/home/aoyama/caffe/build/tools/caffe";
    QString solver = "/home/aoyama/caffe/examples/mnist/lenet_solver.prototxt";
    QStringList args = { "train", (QLatin1String("--solver=") + solver),  };

    auto *caffe = new QProcess(Tf::app());
    caffe->setWorkingDirectory("/home/aoyama/caffe");
    caffe->setProcessChannelMode(QProcess::MergedChannels);
    caffe->setStandardOutputFile("/var/tmp/hogehoge.log");
    connect(caffe, SIGNAL(finished(int, QProcess::ExitStatus)), caffe, SLOT(deleteLater()));
    caffe->start(CaffeCammand, args);

    session().insert("logFile", "/var/tmp/hogehoge.log");
    render();
}


void MainController::train(const QString &no)
{
    const QString CaffeCammand = "/home/aoyama/caffe/build/tools/caffe";

    SolverProto sp = SolverProto::get(no.toInt());
    if (sp.isNull()) {
        return;
    }

    QString solverPath = "/home/aoyama/caffe/examples/mnist/hogehoge_solver.prototxt";
    QFile solver(solverPath);
    if (solver.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&solver);
        out << sp.toText();
    } else {
        return;
    }

    QStringList args = { "train", (QLatin1String("--solver=") + solverPath) };

    auto *caffe = new QProcess(Tf::app());
    caffe->setWorkingDirectory("/home/aoyama/caffe");
    caffe->setProcessChannelMode(QProcess::MergedChannels);
    caffe->setStandardOutputFile("/var/tmp/hogehoge.log");
    connect(caffe, SIGNAL(finished(int, QProcess::ExitStatus)), caffe, SLOT(deleteLater()));
    caffe->start(CaffeCammand, args);

    session().insert("logFile", "/var/tmp/hogehoge.log");
    render();
}

// Don't remove below this line
T_REGISTER_CONTROLLER(maincontroller)
