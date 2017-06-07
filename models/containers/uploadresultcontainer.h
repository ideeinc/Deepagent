#ifndef UPLOADRESULTCONTAINER_H
#define UPLOADRESULTCONTAINER_H

#include <QtCore/QtCore>


class UploadResultContainer
{
public:
    bool completed { false };
    QMap<QString, QStringList> errors;
    QString backPageURL;
};

Q_DECLARE_METATYPE(UploadResultContainer)

#endif // UPLOADRESULTCONTAINER_H
