#ifndef ANSWERCONFIGCONTAINER_H
#define ANSWERCONFIGCONTAINER_H

#include <QtCore/QtCore>
#include "taggroup.h"


class AnswerConfigContainer
{
public:
    QString examGroup;
    QString answerGroup;
    QString excludeWord;
    QString includeWord;
    QString trustUserName;

    QList<Tag> labelTags() const;
};

Q_DECLARE_METATYPE(AnswerConfigContainer)

#endif // ANSWERCONFIGCONTAINER_H
