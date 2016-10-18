#ifndef LOGTAILENDPOINT_H
#define LOGTAILENDPOINT_H

#include "applicationendpoint.h"

class T_CONTROLLER_EXPORT LogTailEndpoint : public ApplicationEndpoint
{
public:
    LogTailEndpoint() { }
    LogTailEndpoint(const LogTailEndpoint &other);

protected:
    bool onOpen(const TSession &httpSession);
    void onClose(int closeCode);
    void onTextReceived(const QString &text);
    void onBinaryReceived(const QByteArray &binary);
};

T_DECLARE_CONTROLLER(LogTailEndpoint, logtailendpoint)

#endif // LOGTAILENDPOINT_H
