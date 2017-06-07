#include "roccurve.h"
#include <TTemporaryFile>


RocCurve::RocCurve(const QList<QPair<float, int>> &rocData)
    : _rocData(rocData)
{}

void RocCurve::setRocData(const QList<QPair<float, int>> &rocData)
{
    _rocData = rocData;
}

// Calculates AUC
float RocCurve::getAuc() const
{
    const auto rscriptPath = Tf::app()->getConfig("settings").value("RscriptPath").toString();
    float auc = std::numeric_limits<float>::quiet_NaN();

    TTemporaryFile rocFile;
    if (!rocFile.open()) {
        tError("File open error. %s", qPrintable(rocFile.absoluteFilePath()));
        return auc;
    }

    QByteArray line;
    for (auto &r : _rocData) {
        line = QByteArray::number((double)r.first) + ',' + QByteArray::number(r.second) + '\n';
        rocFile.write(line);
    }
    rocFile.close();

    QProcess rscript;
    QStringList args = { "tools/auc.R", rocFile.absoluteFilePath() };
    rscript.start(rscriptPath, args);
    if (!rscript.waitForStarted(5000)) {
        tError("R error. CMD: %s", qPrintable(rscriptPath));
        return auc;
    }
    rscript.waitForFinished();
    auc = rscript.readAllStandardOutput().toFloat();
    return auc;
}
