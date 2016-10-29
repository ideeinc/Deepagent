#include "logtailendpoint.h"
#include "tail.h"
#include <atomic>
#include <QMutex>
#include <TScheduler>

class Tailer;

static QMutex mutex;
static std::atomic_ulong idgen {0};
static QMap<QString, Tailer*> tailerMap;


class Tailer : public TScheduler
{
public:
    Tailer(const QString &path, const QString &topic) : _path(path), _topic(topic) {}
    void job();

private:
    QString _path;
    QString _topic;
    int _offset {0};
};


void Tailer::job()
{
    auto text = Tail(_path).tail(4096, _offset);
    _offset += text.length();
    publish(_topic, QString::fromUtf8(text.data(), text.length()));

    restart();
}


LogTailEndpoint::LogTailEndpoint(const LogTailEndpoint &)
    : ApplicationEndpoint()
{ }

bool LogTailEndpoint::onOpen(const TSession &sess)
{
    if (sess.isEmpty()) {
        return false;
    }

    // session : "logFile"
    // session : "tailOffset"
    session().reset();
    session().unite(sess);
    QString log = session().value("logFile").toString();

    int id = idgen.fetch_add(1);
    QString topic = QString::number(id);
    session().insert("topic", topic);
    subscribe(topic);

    QMutexLocker locker(&mutex);
    auto *tail = new Tailer(log, topic);
    tail->setSingleShot(true);
    tail->start(1000);
    tailerMap.insert(topic, tail);

    return true;
}

void LogTailEndpoint::onClose(int)
{
    QString topic = session().value("topic").toString();
    if (!topic.isEmpty()) {
        auto *tail = tailerMap.take(topic);
        if (tail) {
            tail->stop();
            tail->wait(5000);
            tail->deleteLater();
        }
    }
}

void LogTailEndpoint::onTextReceived(const QString &)
{
    // write code
}

void LogTailEndpoint::onBinaryReceived(const QByteArray &)
{ }


// Don't remove below this line
T_REGISTER_CONTROLLER(logtailendpoint)
