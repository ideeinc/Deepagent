#ifndef CAFFETRAINEDMODELPREDICTCONTAINER_H
#define CAFFETRAINEDMODELPREDICTCONTAINER_H

#include <QtCore/QtCore>


class CaffeTrainedModelPredictContainer
{
public:
    QStringList results;
    QFileInfo jpegFile;
};

Q_DECLARE_METATYPE(CaffeTrainedModelPredictContainer)

#endif // CAFFETRAINEDMODELPREDICTCONTAINER_H
