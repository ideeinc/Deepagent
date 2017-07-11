#include "logics/archiver.h"


Archiver::Archiver()
{
}

Archiver::~Archiver()
{
}

QString
Archiver::name() const
{
    return _name;
}

void
Archiver::setName(const QString& name)
{
    _name = name;
}

QSize
Archiver::imageSize() const
{
    return _imageSize;
}

void
Archiver::setImageSize(const QSize& size)
{
    _imageSize = size;
}

CaffeData::Category
Archiver::layoutType() const
{
    return _layoutType;
}

void
Archiver::setLayoutType(const CaffeData::Category& layout)
{
    _layoutType = layout;
}

void
Archiver::setLayoutType(const QString& layoutString)
{
    bool found = false;
    const QString type = layoutString.left(1).toUpper() + layoutString.mid(1).toLower();
    const int value = QMetaEnum::fromType<CaffeData::Category>().keyToValue(type.toUtf8().data(), &found);
    if (found) {
        setLayoutType(static_cast<CaffeData::Category>(value));
    }
}

QStringList
Archiver::labelNames() const
{
    return _labelNames;
}

void
Archiver::setLabelNames(const QStringList& labels)
{
    _labelNames = labels;
}
