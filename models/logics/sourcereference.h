#ifndef SOURCEREFERENCE_H
#define SOURCEREFERENCE_H

#include <QtCore/QtCore>

/**
 */
bool
MakeReferenceFromSourcePath(const QString& sourcePath);

/**
 */
QString
GetAbsolutePathFromSourceName(const QString& fileName);

/**
 */
QString
GetPublicPathFromSourceName(const QString& fileName);

#endif // SOURCEREFERENCE_H
