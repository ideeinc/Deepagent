#ifndef DIRDETECTIONLAYOUTDESCRIPTOR_H
#define DIRDETECTIONLAYOUTDESCRIPTOR_H

#include "logics/dirlayoutdescriptor.h"


class DirDetectionLayoutDescriptor : public DirLayoutDescriptor
{
public:
    virtual void layout(const QMap<QString, QStringList>&, const QString&);

private:
    static void copyScaledImageAndLabel(const QString &sourceImagePath, const QString &destinationImagePath, const QString &sourceLabelPath, const QString &destinationLabelPath, const QSize& size, const QString& alternateLabel = "");
    static QString scaleLabelFromLineBuffer(const QString &lineBuffer, const QPointF& scale, const QString& alternateLabel = "");

    static void copyRotatedImageAndLabel(const QString &sourceImagePath, const QString &destinationImagePath, const QString &sourceLabelPath, const QString &destinationLabelPath, const float angle);
    static QString rotateLabelFromLineBuffer(const QString &lineBuffer, const float angle, const QPointF &center);
};

#endif // DIRDETECTIONLAYOUTDESCRIPTOR_H
