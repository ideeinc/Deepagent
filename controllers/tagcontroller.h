#ifndef TAGCONTROLLER_H
#define TAGCONTROLLER_H

#include "applicationcontroller.h"
#include "services/tagservice.h"


class T_CONTROLLER_EXPORT TagController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    TagController() : ApplicationController() {}

public slots:
    void index();
    void show(const QString& groupName);
    void show(const QString& groupName, const QString& tagName);
    void show(const QString& groupName, const QString& tagName, const QString& index);
    void show(const QString& rowGroupName, const QString& rowTagName, const QString& colGroupName, const QString colTagName);
    void upload();
    void table();

    void create();
    void destroy();
    void append();
    void remove();
    void update();
    void updateGroup();
    void batchUpdate();
    void find();

private:
    TagService service {};
};

#endif // TAGCONTROLLER_H
