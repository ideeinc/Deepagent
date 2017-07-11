#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <QtCore/QtCore>
#include "caffedata.h"
#include "logics/archiverfactory.h"


class Archiver
{
public:
    Archiver();
    virtual ~Archiver();

    virtual QString archive(const QString& sourceDir, const QString& destinationDir) = 0;

    QString name() const;
    void setName(const QString&);
    QSize imageSize() const;
    void setImageSize(const QSize&);
    CaffeData::Category layoutType() const;
    void setLayoutType(const CaffeData::Category&);
    void setLayoutType(const QString&);
    QStringList labelNames() const;
    void setLabelNames(const QStringList&);
private:
    QString _name;
    QSize _imageSize { 0, 0 };
    CaffeData::Category _layoutType { CaffeData::Category::Any };
    QStringList _labelNames;
};

#define REGISTER_ARCHIVER(type, cls)    namespace { struct cls##registrar { cls##registrar() { ArchiverFactory::registerArchiver(ArchiverFactory::ArchiveType::type, []{ return new cls(); }); }; } cls##registrar; }

#endif // ARCHIVER_H
