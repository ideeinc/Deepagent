#ifndef COMPRESSARCHIVER_H
#define COMPRESSARCHIVER_H

#include "logics/archiver.h"


class CompressArchiver : public Archiver
{
public:
    virtual QString archive(const QString& sourceDir, const QString& destinationDir);
};

#endif // COMPRESSARCHIVER_H
