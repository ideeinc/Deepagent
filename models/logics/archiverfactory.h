#ifndef ARCHIVERFACTORY_H
#define ARCHIVERFACTORY_H

#include <QtCore/QtCore>
#include <functional>

class Archiver;
typedef std::function<Archiver*(void)> ArchiverCreator;

class ArchiverFactory
{
    Q_GADGET
public:
    enum class ArchiveType {
        Compress,
        Database
    };
    Q_ENUM(ArchiveType)

    static void registerArchiver(const ArchiveType&, const ArchiverCreator&);
    static ArchiverCreator unregisterArchiver(const ArchiveType&);

    static std::unique_ptr<Archiver> create(const ArchiveType&);
    static std::unique_ptr<Archiver> create(const QString&);

private:
    static QMap<ArchiveType, ArchiverCreator> _registry;
};

#endif // ARCHIVERFACTORY_H
