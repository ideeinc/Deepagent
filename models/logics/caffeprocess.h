#ifndef CAFFEPROCESS_H
#define CAFFEPROCESS_H

#include <TGlobal>
#include <TBackgroundProcess>
#include <QStringList>

// class SolverProto;
// class NeuralNetwork;

/*
class T_HELPER_EXPORT CaffeProcess : public QProcess
{
    Q_OBJECT
public:
    CaffeProcess(QObject *parent = nullptr);
    ~CaffeProcess();

    //void train(const QString &solverPath);
    void start(const QString &program, const QStringList &arguments, OpenMode mode = ReadWrite);
    void start(const SolverProto &solver, const NeuralNetwork &network, OpenMode mode = ReadWrite);
    static CaffeProcess *getProcess(qint64 pid);

public slots:
    void cleanup();

private:

    qint64 caffeContextId {-1};
    QString jobDir;
};
*/

class CaffeProcess
{
public:
    CaffeProcess() {}

    void train(const QString &solverPath, const QString &pretrainedModel);

    // class CleanupHandler : public TBackgroundProcessHandler
    // {
    // public:
    //     CleanupHandler() {}

    // protected:
    //     void handleFinished(int exitCode, QProcess::ExitStatus exitStatus);
    // };
};

#endif // CAFFEPROCESS_H
