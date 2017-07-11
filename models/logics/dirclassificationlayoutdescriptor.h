#ifndef DIRCLASSIFICATIONLAYOUTDESCRIPTOR_H
#define DIRCLASSIFICATIONLAYOUTDESCRIPTOR_H

#include "logics/dirlayoutdescriptor.h"


class DirClassificationLayoutDescriptor : public DirLayoutDescriptor
{
public:
    virtual void layout(const QMap<QString, QStringList>&, const QString&);

private:
    static void copyAugmentedImage(const QDir& sourceDir, const QDir& destinationDir, const QSize& size, const long multiple);
};

#endif // DIRCLASSIFICATIONLAYOUTDESCRIPTOR_H
