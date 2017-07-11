#include "logics/dbconvertoperation.h"
#include "logics/dbconverter.h"


DbConvertOperation::DbConvertOperation(const DbConvertOperation::Role& role)
    : _role(role)
{
}

DbConvertOperation::~DbConvertOperation()
{
}

DbConvertOperation::Role
DbConvertOperation::role() const
{
    return _role;
}

QString
DbConvertOperation::roleName() const
{
    return roleName(_role);
}

QString
DbConvertOperation::roleName(const Role& role)
{
    return QString(QMetaEnum::fromType<DbConvertOperation::Role>().valueToKey(static_cast<int>(role))).toLower();
}
