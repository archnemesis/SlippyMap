#ifndef SLIPPYMAP_LIBRARY_H
#define SLIPPYMAP_LIBRARY_H

#include <cmath>
#include <QTypeInfo>
#include <QPointF>
#include <QtCore/qglobal.h>

#define SLIPPYMAPSHARED_EXPORT Q_DECL_EXPORT

namespace SlippyMap {
    QString latLonToString(const QPointF &point);
}

#endif //SLIPPYMAP_LIBRARY_H
