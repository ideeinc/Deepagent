#ifndef RETAGSERVICE_H
#define RETAGSERVICE_H

#include <QtCore>
#include <THttpRequest>

class TSession;
class RetagSearchContainer;
class RetagShowContainer;


class RetagService
{
public:
    RetagService();

    RetagSearchContainer search(const QString &tag0, const QString &tag1, const QString &tag2, THttpRequest &request, TSession& session);
    RetagShowContainer startSequential(const QString &recentPath, THttpRequest &request, TSession& session);
    RetagShowContainer sequential(const QString &index, THttpRequest &request, TSession& session);
    RetagShowContainer show(const QString &image, TSession &session);
    void saveForm(const QString &image, TSession &session) {}
    void save(const QString &image, int nextStop, TSession &session) {}
};

#endif // RETAGSERVICE_H
