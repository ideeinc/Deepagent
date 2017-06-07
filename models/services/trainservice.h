#ifndef TRAINSERVICE_H
#define TRAINSERVICE_H

#include "containers/trainindexcontainer.h"
#include "containers/trainshowcontainer.h"
#include "containers/traincreatecontainer.h"

class QString;
class THttpRequest;


class TrainService
{
public:
    TrainIndexContainer index();
    QString create(THttpRequest &request);
    TrainShowContainer show(const QString &id);
    bool remove(const QString &id);
    TrainCreateContainer clone(const QString &id);
    QByteArray getPrototxt(const QString &id, const QString &prototxt);
};

#endif // TRAINSERVICE_H
