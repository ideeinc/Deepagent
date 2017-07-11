#include "logics/dbconverter.h"
#include "logics/dbconvertoperation.h"


QMap<CaffeData::Category, DbConverterCreator> DbConverter::_registry;


DbConverter::DbConverter(const CaffeData::Category& layout, const QString& sourceDir)
    : _backend(Backend::LMDB), _layout(layout), _sourceDir(sourceDir), _imageSize(300, 300), _shuffle(true), _generateImageMean(true)
{
}

DbConverter::~DbConverter()
{
}

QString
DbConverter::backendName() const
{
    return QString(QMetaEnum::fromType<DbConverter::Backend>().valueToKey(static_cast<int>(_backend))).toLower();
}

DbConverter::Backend
DbConverter::backend() const
{
    return _backend;
}

void
DbConverter::setBackend(const DbConverter::Backend& backend)
{
    _backend = backend;
}

QString
DbConverter::layoutName() const
{
    return QString(QMetaEnum::fromType<CaffeData::Category>().valueToKey(static_cast<int>(_layout))).toLower();
}

CaffeData::Category
DbConverter::layout() const
{
    return _layout;
}

void
DbConverter::setLayout(const CaffeData::Category& layout)
{
    _layout = layout;
}

QString
DbConverter::sourceDir() const
{
    return _sourceDir;
}

void
DbConverter::setSourceDir(const QString& dir)
{
    _sourceDir = dir;
}

QString
DbConverter::name() const
{
    return _name;
}

void
DbConverter::setName(const QString& name)
{
    _name = name;
}

QSize
DbConverter::imageSize() const
{
    return _imageSize;
}

void
DbConverter::setImageSize(const QSize& size)
{
    _imageSize = size;
}

bool
DbConverter::needsShuffle() const
{
    return _shuffle;
}

void
DbConverter::setNeedsShuffle(const bool& flag)
{
    _shuffle = flag;
}

QStringList
DbConverter::labelNames() const
{
    return _labelNames;
}

void
DbConverter::setLabelNames(const QStringList& labels)
{
    _labelNames = labels;
}

bool
DbConverter::isGenerateImageMean() const
{
    return _generateImageMean;
}

void
DbConverter::setGenerateImageMean(const bool& flag)
{
    _generateImageMean = flag;
}

std::unique_ptr<DbConverter>
DbConverter::create(const CaffeData::Category& layout, const QString& sourceDir)
{
    std::unique_ptr<DbConverter> operation(nullptr);

    const auto creator = _registry[layout];
    if (nullptr != creator) {
        operation.reset(creator(layout, sourceDir));
    }

    return operation;
}

void
DbConverter::registerConverter(const CaffeData::Category& layout, const DbConverterCreator& creator)
{
    _registry.insert(layout, creator);
}

DbConverterCreator
DbConverter::unregisterConverter(const CaffeData::Category& layout)
{
    return _registry.take(layout);
}
