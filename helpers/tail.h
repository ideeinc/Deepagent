#ifndef TAIL_H
#define TAIL_H

#include <TGlobal>
#include <QByteArray>
#include <QString>
#include "applicationhelper.h"


class T_HELPER_EXPORT Tail : public ApplicationHelper
{
public:
    Tail(const QString &filePath);
    QByteArray tail(int maxlen, qint64 offset = 0) const;

private:
    QString path;
};

#endif // TAIL_H
