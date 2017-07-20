#ifndef TAGSERVICE_H
#define TAGSERVICE_H

#include <QtCore>
#include "containers/taginfocontainer.h"
#include "containers/taggedimageinfocontainer.h"
#include "containers/tagtablecontainer.h"
#include "containers/uploadresultcontainer.h"
#include "logics/tagrepository.h"
#include <THttpRequest>
#include <TSession>

class TMimeEntity;


class TagService
{
public:
    TagService();

    void create(const THttpRequest&);
    void destroy(const THttpRequest&);
    void destroy(const QJsonObject&);
    bool update(const THttpRequest&);
    bool updateGroup(const THttpRequest&);
    bool append(const THttpRequest&, TSession&);
    void remove(const THttpRequest&, TSession&);
    void batchUpdate(const THttpRequest&);

    UploadResultContainer uploadImages(THttpRequest&);
    TagInfoContainer find(const THttpRequest&, TSession&);
    TagInfoContainer showTagInfo(const THttpRequest&, TSession&, const QString& groupName, const QString& tagName) const;
    TaggedImageInfoContainer showTagImage(const THttpRequest&, const TSession&, const QString& groupName, const QString& primaryTag, const long& index) const;
    TaggedImageInfoContainer showTableImage(const THttpRequest&, TSession&, const QString& rowGroupName, const QString& rowTagName, const QString& colGroupName, const QString colTagName) const;
    TagTableContainer table(const THttpRequest&, TSession&) const;

private:
    TagRepository _repository;
};

#endif // TAGSERVICE_H
