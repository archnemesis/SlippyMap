#include "SlippyMap/SlippyMapLayerPolygon.h"
#include "SlippyMap/SlippyMapLayerPolygonPropertyPage.h"

#define NOMINMAX
#include <algorithm>
#include <corecrt_math_defines.h>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

using namespace std;
using namespace SlippyMap;
using namespace boost::geometry;

typedef model::d2::point_xy<double> point_t;
typedef model::polygon<point_t> polygon_t;

SlippyMapLayerPolygon::SlippyMapLayerPolygon(QObject *parent) :
    SlippyMapLayerObject(parent),
    m_points({}),
    m_fillColor(qApp->palette().base().color()),
    m_strokeColor(qApp->palette().highlight().color()),
    m_strokeWidth(1)
{
    initStyle();
}

SlippyMapLayerPolygon::SlippyMapLayerPolygon(const QVector<QPointF>& points, QObject *parent) :
        SlippyMapLayerPolygon(parent)
{
    m_points = points;
}

SlippyMapLayerPolygon::SlippyMapLayerPolygon(const SlippyMapLayerPolygon &other)
{
    setId(other.id());
    setLabel(other.label());
    setDescription(other.description());
    setPoints(other.points());
    setStrokeWidth(other.strokeWidth());
    setStrokeColor(other.strokeColor());
    setFillColor(other.fillColor());
}

void SlippyMapLayerPolygon::copy(SlippyMapLayerObject *other)
{
    auto *polygon = dynamic_cast<SlippyMapLayerPolygon*>(other);

    setId(polygon->id());
    setLabel(polygon->label());
    setDescription(polygon->description());
    setPoints(polygon->points());
    setStrokeWidth(polygon->strokeWidth());
    setStrokeColor(polygon->strokeColor());
    setFillColor(polygon->fillColor());
    setPosition(polygon->position());
}

void SlippyMapLayerPolygon::hydrateFromDatabase(const QJsonObject &json, const QString &geometry)
{
    QColor strokeColor(json["strokeColor"].toString());
    setStrokeColor(strokeColor);
    setStrokeWidth(json["strokeWidth"].toInt());

    QColor fillColor(json["fillColor"].toString());
    setFillColor(fillColor);

    auto polygon = from_wkt<polygon_t>(geometry.toStdString());

    m_points.clear();
    for (auto it = boost::begin(exterior_ring(polygon)); it != boost::end(exterior_ring(polygon)); ++it) {
        double x = get<0>(*it);
        double y = get<1>(*it);
        QPointF point(x, y);
        m_points.append(point);
    }

    // we close the ring for PostGIS, but not needed for Qt
    m_points.removeLast();

    emit updated();
}

void SlippyMapLayerPolygon::packageObjectData(QJsonObject &json, QString &geometry)
{
    json["strokeWidth"] = strokeWidth();
    json["strokeColor"] = strokeColor().name(QColor::HexArgb);
    json["fillColor"] = fillColor().name(QColor::HexArgb);

    QStringList pointStrings;
    for (const auto &point: m_points) {
        pointStrings.append(QString("%1 %2")
            .arg(point.x(), 0, 'f', 7)
            .arg(point.y(), 0, 'f', 7));
    }

    // close the ring by copying the first point to the end
    pointStrings.append(QString("%1 %2")
        .arg(m_points.at(0).x(), 0, 'f', 7)
        .arg(m_points.at(0).y(), 0, 'f', 7));

    geometry = QString("POLYGON((%1))")
            .arg(pointStrings.join(", "));
}

SlippyMapLayerPolygon *SlippyMapLayerPolygon::clone() const
{
    return new SlippyMapLayerPolygon(*this);
}

SlippyMapLayerPolygon::~SlippyMapLayerPolygon()
{

}

void SlippyMapLayerPolygon::draw(QPainter *painter, const QTransform &transform, ObjectState state)
{
    painter->setPen(state == SelectedState ? m_strokePenSelected : m_strokePen);
    painter->setBrush(state == SelectedState ? m_fillBrushSelected : m_fillBrush);

    painter->save();
    painter->setWorldTransform(transform);
    painter->drawConvexPolygon(m_points.data(), m_points.length());
    painter->restore();

    /* resize handles */
    if (state == SlippyMapLayerObject::SelectedState) {
        for (QPointF point: m_points) {
            QPointF mapped = transform.map(point);
            drawResizeHandle(painter, QPoint(static_cast<int>(mapped.x()), static_cast<int>(mapped.y())));
        }
    }
}

QVector<QPointF> SlippyMapLayerPolygon::points() const
{
    return m_points;
}

void SlippyMapLayerPolygon::setPoints(const QVector<QPointF> &points)
{
    m_points = points;
}

bool SlippyMapLayerPolygon::isIntersectedBy(const QRectF& rect) const
{
    for (QPointF point : m_points) {
        if (rect.contains(point)) {
            return true;
        }
    }

    return false;
}

bool SlippyMapLayerPolygon::contains(const QPointF& point, int zoom) const
{
    int n = m_points.length();
    int count = 0;

    for (int i = 0; i < n; i++) {
        QPointF p1 = m_points.at(i);
        QPointF p2 = m_points.at((i + 1) % n);

        if (point.y() > min(p1.y(), p2.y()) \
                && (point.y() <= max(p1.y(), p2.y())) \
                && (point.x() <= max(p1.x(), p2.x()))) {
            double x_intersect = (point.y() - p1.y()) \
                * (p2.x() - p1.x()) \
                / (p2.y() - p1.y()) \
                + p1.x();

            if (p1.x() == p2.x() || point.x() <= x_intersect) {
                count++;
            }
        }
    }

    return (count % 2) != 0;
}

const QPointF SlippyMapLayerPolygon::position() const
{
    const qreal MIN = numeric_limits<qreal>().min();
    const qreal MAX = numeric_limits<qreal>().max();
    qreal x = MAX;
    qreal y = MIN;

    for (QPointF point : m_points) {
        if (point.x() < x) {
            x = point.x();
        }

        if (point.y() > y) {
            y = point.y();
        }
    }

    return QPointF(x, y);
}

void SlippyMapLayerPolygon::setPosition(const QPointF &position) {
    QPointF curr = this->position();
    QPointF diff = position - curr;
    for (auto& point : m_points) {
        point += diff;
    }
    emit updated();
}

const QSizeF SlippyMapLayerPolygon::size() const
{
    QPointF pos = position();

    const qreal MIN = numeric_limits<qreal>().min();
    const qreal MAX = numeric_limits<qreal>().max();
    qreal x = -MAX;
    qreal y = MAX;

    for (QPointF point : m_points) {
        qDebug() << "Point:" << point;

        if (point.x() > x) {
            x = point.x();
        }

        if (point.y() < y) {
            y = point.y();
        }
    }

    qDebug() << "Got X:" << x << "and Y:" << y;

    qreal width = x - pos.x();
    qreal height = pos.y() - y;
    return QSizeF(width, height);
}

const QColor & SlippyMapLayerPolygon::strokeColor() const
{
    return m_strokeColor;
}

const QColor & SlippyMapLayerPolygon::fillColor() const
{
    return m_fillColor;
}

int SlippyMapLayerPolygon::strokeWidth() const
{
    return m_strokeWidth;
}

void SlippyMapLayerPolygon::initStyle()
{
    m_strokePen.setStyle(Qt::SolidLine);
    //m_strokePen.setCosmetic(true);
    m_strokePen.setJoinStyle(Qt::MiterJoin);
    m_strokePen.setCapStyle(Qt::SquareCap);
    m_strokePen.setWidth(m_strokeWidth);
    m_strokePen.setColor(m_strokeColor);

    // width is in meters
    // convert to degrees
    qreal lat_dist_per_deg = 111.32 * 1000; // 111.32 km, or 1,113,200 meters
    qreal lon_dist_per_deg = 111.32 * 1000 * cos((M_PI / 180.0) * position().y());
    qreal avg_dist_per_deg = (lat_dist_per_deg + lon_dist_per_deg) / 2.0;
    qreal deg_per_1m = 1.0 / avg_dist_per_deg;

    m_strokePen.setWidthF(deg_per_1m * static_cast<qreal>(m_strokeWidth));

    m_strokePenSelected.setStyle(Qt::SolidLine);
    m_strokePenSelected.setJoinStyle(Qt::MiterJoin);
    m_strokePenSelected.setCapStyle(Qt::SquareCap);
    m_strokePenSelected.setWidthF(deg_per_1m * static_cast<qreal>(m_strokeWidth));
    m_strokePenSelected.setColor(m_strokeColor.lighter());

    m_fillBrush.setStyle(Qt::SolidPattern);
    m_fillBrush.setColor(m_fillColor);

    m_fillBrushSelected.setStyle(Qt::SolidPattern);
    m_fillBrushSelected.setColor(m_fillColor.lighter());
}

bool SlippyMapLayerPolygon::test_point(const QPointF& a, const QPointF& b, const QPointF& p) const
{
    const qreal epsilon = numeric_limits<qreal>().epsilon();
    const numeric_limits<qreal> DOUBLE;
    const qreal MIN = DOUBLE.min();
    const qreal MAX = DOUBLE.max();

    if (a.y() > b.y()) return test_point(b, a, p);
    if (p.y() == a.y() || p.y() == b.y()) return test_point(a, b, QPointF(p.x(), p.y() + epsilon));
    if (p.y() > b.y() || p.y() < a.y() || p.x() > max(a.x(), b.x())) return false;
    if (p.x() < min(a.x(), b.x())) return true;
    double blue = abs(a.x() - p.x()) > MIN ? (p.y() - a.y()) / (p.x() - a.x()) : MAX;
    double red  = abs(a.x() - b.x()) > MIN ? (b.y() - a.y()) / (b.x() - a.x()) : MAX;
    return blue >= red;
}

QDataStream &SlippyMapLayerPolygon::serialize(QDataStream &stream) const
{
    stream << id();
    stream << label();
    stream << description();
    stream << position();
    stream << strokeColor();
    stream << strokeWidth();
    stream << fillColor();
    stream << points();
    return stream;
}

void SlippyMapLayerPolygon::unserialize(QDataStream &stream)
{
    QVariant id;
    QString label;
    QString description;
    QPointF position;
    QColor strokeColor;
    int strokeWidth;
    QColor fillColor;
    QVector<QPointF> points;

    stream >> id;
    stream >> label;
    stream >> description;
    stream >> position;
    stream >> strokeColor;
    stream >> strokeWidth;
    stream >> fillColor;
    stream >> points;

    setId(id);
    setLabel(label);
    setDescription(description);
    setPosition(position);
    setStrokeColor(strokeColor);
    setStrokeWidth(strokeWidth);
    setFillColor(fillColor);
    setPoints(points);
}

void SlippyMapLayerPolygon::setStrokeColor(const QColor &color)
{
    m_strokeColor = color;
    initStyle();
    emit updated();
}

void SlippyMapLayerPolygon::setStrokeWidth(int width)
{
    m_strokeWidth = width;
    initStyle();
    emit updated();
}

void SlippyMapLayerPolygon::setFillColor(const QColor &color)
{
    m_fillColor = color;
    initStyle();
    emit updated();
}

QList<SlippyMapLayerObjectPropertyPage*> SlippyMapLayerPolygon::propertyPages(
        SlippyMapLayerObject::Ptr object
        ) const
{
    return {new SlippyMapLayerPolygonPropertyPage(object)};
}

QList<QRectF> SlippyMapLayerPolygon::resizeHandles(const QTransform& transform) const
{
    return {};
}
