#ifndef LOGTAILENDPOINT_H
#define LOGTAILENDPOINT_H

#include "applicationendpoint.h"


class T_CONTROLLER_EXPORT LogTailEndpoint : public ApplicationEndpoint
{
    Q_OBJECT
public:
    Q_INVOKABLE
    LogTailEndpoint() : ApplicationEndpoint() {}

protected:
    bool onOpen(const TSession &httpSession);
    void onClose(int closeCode);
    void onTextReceived(const QString &text);
    void onBinaryReceived(const QByteArray &binary);
};

#endif // LOGTAILENDPOINT_H
