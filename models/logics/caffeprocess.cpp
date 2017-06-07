#include "caffeprocess.h"
#include "caffecontext.h"
#include "solverproto.h"
#include "trainingmodel.h"
#include <TWebApplication>
#include <TBackgroundProcess>
#include <QMutex>
#include <QFileInfo>
#include <QTextStream>
#include <QThread>
#include "tsystemglobal.h"

static QMutex mutexManager;
//static QMap<qint64, CaffeProcess*> processManager;
static QMap<void*, QString> caffeManager;


// CaffeProcess::CaffeProcess(QObject *parent) : QProcess(parent)
// { }


// CaffeProcess::~CaffeProcess()
// {
//     tDebug("~CaffeProcess");
// }


void CaffeProcess::train(const QString &solverPath, const QString &pretrainedModel)
{
    qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");

    static const auto CaffeCommandPath = Tf::app()->getConfig("settings").value("CaffeCommand").toString();
    auto *caffeCmd = new TBackgroundProcess();
    //caffeCmd->setAutoDelete(true);
    //caffeCmd->moveToThread(Tf::app()->databaseContextMainThread());
    //QObject::connect(caffeCmd, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(cleanupProcess()), Qt::QueuedConnection);

    //CleanupHandler *handler = new CleanupHandler();


    // QObject::connect(caffeCmd, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this,
    // &CaffeProcess::cleanupProcess, Qt::QueuedConnection);

//    QObject::connect(caffeCmd, &QProcess::finished, this, &CaffeProcess::train, Qt::QueuedConnection);
    auto workDir = QFileInfo(solverPath).absolutePath();
    caffeCmd->setWorkingDirectory(workDir);
    QStringList args = {"train", "-solver", solverPath, "-log_dir", workDir};
    tDebug() << "caffe command: " << CaffeCommandPath << " " << args.join(" ");
    //caffeCmd->start(CaffeCommandPath, args, QProcess::ReadWrite, handler);
    caffeCmd->waitForStarted();
    caffeCmd->closeReadChannel(QProcess::StandardOutput);
    caffeCmd->closeReadChannel(QProcess::StandardError);
    caffeCmd->closeWriteChannel();

    auto dirName = QFileInfo(workDir).fileName();
    caffeManager.insert(caffeCmd, dirName);
    tDebug() << "################# started";
    //caffeCmd->moveToThread(Tf::app()->databaseContextMainThread());
    //Tf::app()->databaseContextMainThread()->start();
}


// void CaffeProcess::CleanupHandler::handleFinished(int exitCode, QProcess::ExitStatus exitStatus)
// {
//     tDebug() << "################# ##########!!!!!!!!!!!!!!";
// }


