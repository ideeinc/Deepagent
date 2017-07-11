#ifndef DBCLASSIFICATIONCONVERTOPERATION_H
#define DBCLASSIFICATIONCONVERTOPERATION_H

#include "logics/dbconvertoperation.h"


class DbClassificationConvertOperation : public DbConvertOperation
{
public:
    using DbConvertOperation::DbConvertOperation;

    virtual void execute(const DbConverter&, const QString&, const QString&);
};

#endif // DBCLASSIFICATIONCONVERTOPERATION_H
