#ifndef TAGCONTROLLER_H
#define TAGCONTROLLER_H

#include "applicationcontroller.h"
#include "services/tagservice.h"


class T_CONTROLLER_EXPORT TagController : public ApplicationController
{
    Q_OBJECT
public:
    Q_INVOKABLE
    TagController() { }
    TagController(const TagController &other);

public slots:
    void index();
    void show(const QString& groupName);
    void show(const QString& groupName, const QString& tagName);
    void show(const QString& groupName, const QString& tagName, const QString& index);
    void upload();

    void create();
    void destroy();
    void append();
    void remove();
    void update();
    void updateGroup();
    void batchUpdate();

private:
    TagService service {};
};

T_DECLARE_CONTROLLER(TagController, tagcontroller)

#endif // TAGCONTROLLER_H
