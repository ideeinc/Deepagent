#include "logics/archiverfactory.h"
#include "logics/archiver.h"


QMap<ArchiverFactory::ArchiveType, ArchiverCreator> ArchiverFactory::_registry;


void
ArchiverFactory::registerArchiver(const ArchiveType& type, const ArchiverCreator& creator)
{
    _registry[type] = creator;
}

ArchiverCreator
ArchiverFactory::unregisterArchiver(const ArchiveType& type)
{
    ArchiverCreator creator = nullptr;
    if (_registry.contains(type)) {
        creator = _registry.take(type);
    }
    return creator;
}

std::unique_ptr<Archiver>
ArchiverFactory::create(const ArchiveType& type)
{
    std::unique_ptr<Archiver> archiver(nullptr);

    const auto creator = _registry[type];
    if (nullptr != creator) {
        archiver.reset(creator());
    }

    return archiver;
}

std::unique_ptr<Archiver>
ArchiverFactory::create(const QString& type)
{
    bool found = false;
    const QString capitalizedType = type.left(1).toUpper() + type.mid(1).toLower();
    const int value = QMetaEnum::fromType<ArchiveType>().keyToValue(capitalizedType.toUtf8().data(), &found);
    if (found) {
        return create(static_cast<ArchiveType>(value));
    }

    return nullptr;
}
