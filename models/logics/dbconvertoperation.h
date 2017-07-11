#ifndef DBCONVERTOPERATION_H
#define DBCONVERTOPERATION_H

#include <QtCore/QtCore>

class DbConverter;


class DbConvertOperation {
    Q_GADGET
public:
    enum class Role {
        Train,
        Val,
    };
    Q_ENUM(Role)

    DbConvertOperation(const Role&);
    virtual ~DbConvertOperation();

    virtual void execute(const DbConverter&, const QString&, const QString&) = 0;

    Role role() const;
    QString roleName() const;
    static QString roleName(const Role&);

private:
    Role _role;
};

#endif // DBCONVERTOPERATION_H

