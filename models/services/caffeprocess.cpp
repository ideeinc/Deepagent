#include "caffeprocess.h"
#include "caffecontext.h"
#include "solverproto.h"
#include <TWebApplication>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include "tsystemglobal.h"

static QMutex mutexManager;
static QMap<qint64, CaffeProcess*> processManager;


CaffeProcess::CaffeProcess(QObject *parent) : QProcess(parent)
{ }


CaffeProcess::~CaffeProcess()
{
    tDebug("~CaffeProcess");
}


void CaffeProcess::start(const QString &program, const QStringList &arguments, OpenMode mode)
{
    QProcess::start(program, arguments, mode);
}


void CaffeProcess::start(const SolverProto &solver, const NeuralNetwork &network, OpenMode mode)
{
    //const QString CaffeCammand = "/home/aoyama/caffe/build/tools/caffe";
    const QString CaffeCammand = "/home/aoyama/tmp/echo.sh";
    //const QString CaffeCammand = "/bin/ls";

    if (solver.isNull()) {
        return;
    }

    setWorkingDirectory("/home/aoyama/caffe");
    setProcessChannelMode(QProcess::MergedChannels);
    setStandardOutputFile("/var/tmp/hogehoge.log");

    QString solverPath = "/home/aoyama/caffe/examples/mnist/hogehoge_solver.prototxt";
    QFile solverTxt(solverPath);
    if (solverTxt.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&solverTxt);
        out << solver.toText();
    } else {
        return;
    }

    QFile("/var/tmp/hogehoge.log").remove();

    QStringList args = { "train", (QLatin1String("--solver=") + solverPath) };
    QProcess::start(CaffeCammand, args, mode);

    auto pid = processId();
    tDebug("pid: %lld", pid);
    if (pid > 0) {
        auto cc = CaffeContext::create(solver.id(), 0, pid, QString(), "/var/tmp/hogehoge.log");

        if (!cc.isNull()) {
            caffeContextId = cc.id();
            QMutexLocker lock(&mutexManager);
            processManager.insert(processId(), this);
        } else {
            caffeContextId = -1;
            kill();
        }
    }
}


void CaffeProcess::cleanup()
{
    tDebug("CaffeProcess::cleanup");

    if (caffeContextId > 0) {
        auto cc = CaffeContext::get(caffeContextId);
        tDebug("cleanup caffeContextId:%d  id:%d pid: %lld", caffeContextId, cc.id(), cc.caffePid());
        //cc.setCaffePid(0);
    }
    deleteLater();
}


CaffeProcess *CaffeProcess::getProcess(qint64 pid)
{
    QMutexLocker lock(&mutexManager);
    return processManager.value(pid);
}
