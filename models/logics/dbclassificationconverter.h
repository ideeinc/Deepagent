#ifndef DBCLASSIFICATIONCONVERTER_H
#define DBCLASSIFICATIONCONVERTER_H

#include "logics/dbconverter.h"


class DbClassificationConverter : public DbConverter
{
public:
    DbClassificationConverter(const CaffeData::Category& layout, const QString& sourceDir);

    virtual void convertTo(const QString&);

    void saveLabelNamesTo(const QString&) const;
    void saveImageMean(const QString&);
};

#endif // DBCLASSIFICATIONCONVERTER_H
