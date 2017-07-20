#ifndef ANSWERDISPLAYIMAGECONTAINER_H
#define ANSWERDISPLAYIMAGECONTAINER_H

#include <QtCore/QtCore>
#include "taggroup.h"


class AnswerDisplayImageContainer
{
public:
    bool isTrusted;
    QString imageFile;
    QString originalFileName;
    QSize size { 0, 0 };
    QList<Tag> labelTags;

    int amount { 0 };
    int current { 0 };
    int previous { 0 };
    int next { 0 };
};

Q_DECLARE_METATYPE(AnswerDisplayImageContainer)

#endif // ANSWERDISPLAYIMAGECONTAINER_H
