#include "SlippyMap/SlippyMap.h"

#include <QPointF>
#include <QString>

QString SlippyMap::latLonToString(const QPointF &point)
{
    char dir_lat;
    char dir_lon;
    double lat = point.y();
    double lon = point.x();

    if (lat > 0) {
        dir_lat = 'N';
    }
    else {
        dir_lat = 'S';
    }

    if (lon > 0) {
        dir_lon = 'E';
    }
    else {
        dir_lon = 'W';
    }

    QString ret = QString("%1 %2 %3 %4")
            .arg(fabs(lat), 0, 'f', 6, '0')
            .arg(dir_lat)
            .arg(fabs(lon), 0, 'f', 6, '0')
            .arg(dir_lon);

    return ret;
}