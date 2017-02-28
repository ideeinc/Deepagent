#ifndef CAFFEPROCESS_H
#define CAFFEPROCESS_H

#include <TGlobal>
#include <QProcess>
#include <QStringList>
#include "applicationhelper.h"

class SolverProto;
class NeuralNetwork;


class T_HELPER_EXPORT CaffeProcess : public QProcess
{
    Q_OBJECT
public:
    CaffeProcess(QObject *parent = nullptr);
    ~CaffeProcess();

    void start(const QString &program, const QStringList &arguments, OpenMode mode = ReadWrite);
    void start(const SolverProto &solver, const NeuralNetwork &network, OpenMode mode = ReadWrite);
    static CaffeProcess *getProcess(qint64 pid);

public slots:
    void cleanup();

private:
    qint64 caffeContextId {-1};
};



// class CaffeCleanup : public QObject
// {
//     Q_OBJECT
// public:
//     CaffeCleanup() = default;
//     ~CaffeCleanup() = default;

// public slots:
//     void cleanup();
// }
    ;

#endif // CAFFEPROCESS_H
