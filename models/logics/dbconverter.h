#ifndef DBCONVERTER_H
#define DBCONVERTER_H

#include <QtCore/QtCore>
#include <functional>
#include "caffedata.h"

class DbConverter;
typedef std::function<DbConverter*(const CaffeData::Category&, const QString&)> DbConverterCreator;


class DbConverter {
    Q_GADGET
public:
    enum class Backend : int {
        LMDB,
        LEVELDB,
    };
    Q_ENUM(Backend)

    virtual ~DbConverter();
    virtual void convertTo(const QString& destinationDir) = 0;

    QString backendName() const;
    Backend backend() const;
    void setBackend(const Backend&);
    QString layoutName() const;
    CaffeData::Category layout() const;
    void setLayout(const CaffeData::Category&);
    QString sourceDir() const;
    void setSourceDir(const QString&);
    QString name() const;
    void setName(const QString&);
    QSize imageSize() const;
    void setImageSize(const QSize& size);
    bool needsShuffle() const;
    void setNeedsShuffle(const bool&);
    QStringList labelNames() const;
    void setLabelNames(const QStringList&);
    bool isGenerateImageMean() const;
    void setGenerateImageMean(const bool&);

    static std::unique_ptr<DbConverter> create(const CaffeData::Category&, const QString&);
    static void registerConverter(const CaffeData::Category&, const DbConverterCreator&);
    static DbConverterCreator unregisterConverter(const CaffeData::Category&);

protected:
    DbConverter(const CaffeData::Category& layout, const QString& sourceDir);

private:
    Backend _backend;
    CaffeData::Category _layout;
    QString _sourceDir;
    QString _name;
    QSize _imageSize;
    bool _shuffle;
    QStringList _labelNames;
    bool _generateImageMean;

    static QMap<CaffeData::Category, DbConverterCreator> _registry;
};

#define REGISTER_DBCONVERTER(type, cls)             namespace { struct cls##type##registrar { cls##type##registrar() { DbConverter::registerConverter(CaffeData::Category::type, [](const CaffeData::Category& layout, const QString& sourceDir){ return new cls(layout, sourceDir); }); }; } cls##type##registrar; }

#endif // DBCONVERTER_H
