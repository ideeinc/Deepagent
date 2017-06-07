#ifndef ROCCURVE_H
#define ROCCURVE_H

#include <QtCore>
#include <TGlobal>
#include "image.h"


class T_MODEL_EXPORT RocCurve
{
public:
    RocCurve() { }
    RocCurve(const QList<QPair<float, int>> &rocData);

    void setRocData(const QList<QPair<float, int>> &rocData);
    float getAuc() const;
    Image renderRoc() const { return Image(); }

private:
    QList<QPair<float, int>> _rocData;
};

#endif // ROCCURVE_H
