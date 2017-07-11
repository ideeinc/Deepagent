#include "logics/dirlayoutfactory.h"
#include "logics/dirlayoutdescriptor.h"


QMap<CaffeData::Category, LayoutDesciprtorCreator> DirLayoutFactory::_registry;


void
DirLayoutFactory::registerLayoutCreator(const CaffeData::Category& type, const LayoutDesciprtorCreator& creator)
{
    _registry[type] = creator;
}

LayoutDesciprtorCreator
DirLayoutFactory::unregisterLayoutCreator(const CaffeData::Category& type)
{
    LayoutDesciprtorCreator creator = nullptr;

    if (_registry.contains(type)) {
        creator = _registry.take(type);
    }

    return creator;
}

std::unique_ptr<DirLayoutDescriptor>
DirLayoutFactory::create(const CaffeData::Category& type)
{
    std::unique_ptr<DirLayoutDescriptor> descriptor(nullptr);

    const auto creator = _registry[type];
    if (nullptr != creator) {
        descriptor.reset(creator());
    }

    return descriptor;
}

std::unique_ptr<DirLayoutDescriptor>
DirLayoutFactory::create(const QString& type)
{
    bool found = false;
    const QString capitalizedType = type.left(1).toUpper() + type.mid(1).toLower();
    const int value = QMetaEnum::fromType<CaffeData::Category>().keyToValue(capitalizedType.toUtf8().data(), &found);
    if (found) {
        return create(static_cast<CaffeData::Category>(value));
    }

    return nullptr;
}
