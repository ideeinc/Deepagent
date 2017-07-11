#ifndef DIRLAYOUTFACTORY_H
#define DIRLAYOUTFACTORY_H

#include <QtCore/QtCore>
#include <functional>
#include "caffedata.h"

class DirLayoutDescriptor;
typedef std::function<DirLayoutDescriptor*(void)> LayoutDesciprtorCreator;


class DirLayoutFactory
{
public:
    static void registerLayoutCreator(const CaffeData::Category&, const LayoutDesciprtorCreator&);
    static LayoutDesciprtorCreator unregisterLayoutCreator(const CaffeData::Category&);

    static std::unique_ptr<DirLayoutDescriptor> create(const CaffeData::Category&);
    static std::unique_ptr<DirLayoutDescriptor> create(const QString&);

private:
    static QMap<CaffeData::Category, LayoutDesciprtorCreator> _registry;
};

#endif // DIRLAYOUTFACTORY_H
