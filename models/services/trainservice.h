#ifndef TRAINSERVICE_H
#define TRAINSERVICE_H

#include "containers/trainindexcontainer.h"
#include "containers/trainclassifycontainer.h"
#include "containers/traindetectcontainer.h"
#include "containers/trainshowcontainer.h"
#include "containers/traincreatecontainer.h"

class QString;
class THttpRequest;


class TrainService
{
public:
    TrainIndexContainer index();
    QString create(THttpRequest &request);
    QString createSsd(THttpRequest &request);
    TrainClassifyContainer classify(const QString &id, THttpRequest &request);
    TrainDetectContainer detect(const QString &id, THttpRequest &request);
    TrainShowContainer show(const QString &id);
    bool remove(const QString &id);
    TrainCreateContainer clone(const QString &id);
    QByteArray getPrototxt(const QString &id, const QString &prototxt);
    void uploadTrainedModel(THttpRequest &request);
};

#endif // TRAINSERVICE_H
