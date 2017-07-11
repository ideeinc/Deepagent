#ifndef DBDETECTIONCONVERTER_H
#define DBDETECTIONCONVERTER_H

#include "logics/dbconverter.h"


class DbDetectionConverter : public DbConverter
{
public:
    DbDetectionConverter(const CaffeData::Category& layout, const QString& sourceDir);

    virtual void convertTo(const QString&);
};

#endif // DBDETECTIONCONVERTER_H
