#ifndef DIRLAYOUTDESCRIPTOR_H
#define DIRLAYOUTDESCRIPTOR_H

#include <QtCore/QtCore>
#include <functional>
#include "logics/dirlayoutfactory.h"


class DirLayoutDescriptor
{
public:
    DirLayoutDescriptor();
    virtual ~DirLayoutDescriptor();

    virtual void layout(const QMap<QString, QStringList>&, const QString&) = 0;

    int augmentationRate() const;
    void setAugmentationRate(const int&);
    float validationRate() const;
    void setValidationRate(const float&);
    QSize imageSize() const;
    void setImageSize(const QSize&);

protected:
    class Statistics {
    public:
        QMap<QString, QVariant> originalTotal;
        QMap<QString, QVariant> originalTraining;
        QMap<QString, QVariant> originalValidation;
        QMap<QString, QVariant> augmentedTraining;
        QMap<QString, QVariant> augmentedValidation;
        int augmentationRate{0};
        float validationRate{0};

        void saveTo(const QString& dir, const QString& filename);
    };

    QDir makeSourceLayout(const QMap<QString, QStringList>&, const QString&, const std::function<bool(const QString&)>& filter = nullptr);

private:
    int _augmentationRage { 1 };
    float _validationRate { 0.2 };
    QSize _imageSize { 300, 300 };
};

#define REGISTER_LAYOUT_DESCRIPTOR(type, cls)   namespace { struct cls##registrar { cls##registrar() { DirLayoutFactory::registerLayoutCreator(CaffeData::Category::type, []{ return new cls(); }); } } cls##registrar; }

#endif // DIRLAYOUTDESCRIPTOR_H
