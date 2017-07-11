#ifndef DATABASEARCHIVER_H
#define DATABASEARCHIVER_H

#include "logics/archiver.h"


class DatabaseArchiver : public Archiver
{
public:
    virtual QString archive(const QString& sourceDir, const QString& destinationDir);
};

#endif // DATABASEARCHIVER_H
