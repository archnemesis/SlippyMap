//
// Created by Robin on 11/1/2024.
//

#include "SlippyMap/SlippyMapLayerPath.h"
#include <QJsonObject>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

using namespace SlippyMap;
using namespace boost::geometry;

typedef model::d2::point_xy<double> point_t;
typedef model::linestring<point_t> linestring_t;

SlippyMapLayerPath::SlippyMapLayerPath(QObject *parent) :
    SlippyMapLayerObject(parent),
    m_lineWidth(10),
    m_strokeWidth(1)
{
    initStyle();
}

SlippyMapLayerPath::SlippyMapLayerPath(const QVector<QPointF> &points) :
        SlippyMapLayerObject(),
        m_points(points),
        m_lineWidth(10),
        m_strokeWidth(1)
{
    initStyle();
}

SlippyMapLayerPath::SlippyMapLayerPath(const SlippyMapLayerPath &other) :
        SlippyMapLayerObject(),
        m_lineWidth(10),
        m_strokeWidth(1)
{
    setId(other.id());
    setLabel(other.label());
    setDescription(other.description());
    setPoints(other.points());
    setLineWidth(other.lineWidth());
    setLineColor(other.lineColor());
    setStrokeWidth(other.strokeWidth());
    setStrokeColor(other.strokeColor());
}

void SlippyMapLayerPath::copy(SlippyMapLayerObject *other)
{
    auto *path = dynamic_cast<SlippyMapLayerPath*>(other);
    setId(path->id());
    setLabel(path->label());
    setDescription(path->description());
    setPoints(path->points());
    setLineWidth(path->lineWidth());
    setLineColor(path->lineColor());
    setStrokeWidth(path->strokeWidth());
    setStrokeColor(path->strokeColor());
}

void SlippyMapLayerPath::hydrateFromDatabase(const QJsonObject &json, const QString &geometry)
{
    int lineWidth = json["lineWidth"].toInt();
    int strokeWidth = json["strokeWidth"].toInt();
    setLineWidth(lineWidth);
    setStrokeWidth(strokeWidth);

    QColor lineColor(json["lineColor"].toString());
    QColor strokeColor(json["strokeColor"].toString());
    setLineColor(lineColor);
    setStrokeColor(strokeColor);

    auto linestring = from_wkt<linestring_t>(geometry.toStdString());

    m_points.clear();
    for (auto it = boost::begin(linestring); it != boost::end(linestring); it++) {
        double x = get<0>(*it);
        double y = get<1>(*it);
        QPointF point(x, y);
        m_points.append(point);
    }

    emit updated();
}

void SlippyMapLayerPath::packageObjectData(QJsonObject &json, QString &geometry)
{
    json["lineWidth"] = lineWidth();
    json["lineColor"] = lineColor().name(QColor::HexArgb);
    json["strokeWidth"] = strokeWidth();
    json["strokeColor"] = strokeColor().name(QColor::HexArgb);

    QStringList pointStrings;
    for (const auto& point: m_points) {
        pointStrings.append(QString("%1 %2")
            .arg(point.x(), 0, 'f', 7)
            .arg(point.y(), 0, 'f', 7));
    }

    geometry = QString("LINESTRING(%1)")
            .arg(pointStrings.join(", "));
}

QDataStream &SlippyMapLayerPath::serialize(QDataStream &stream) const
{
    stream << id();
    stream << label();
    stream << description();
    stream << points();
    stream << lineWidth();
    stream << lineColor();
    stream << strokeWidth();
    stream << strokeColor();
    return stream;
}

void SlippyMapLayerPath::unserialize(QDataStream &stream)
{
    QVariant id;
    QString label;
    QString description;
    QVector<QPointF> points;
    int lineWidth;
    QColor lineColor;
    int strokeWidth;
    QColor strokeColor;

    stream >> id;
    stream >> label;
    stream >> description;
    stream >> lineWidth;
    stream >> lineColor;
    stream >> strokeWidth;
    stream >> strokeColor;

    setId(id);
    setLabel(label);
    setDescription(description);
    setLineWidth(lineWidth);
    setLineColor(lineColor);
    setStrokeWidth(strokeWidth);
    setStrokeColor(strokeColor);
}

void SlippyMapLayerPath::draw(QPainter *painter, const QTransform &transform, SlippyMapLayerObject::ObjectState state)
{
    if (m_strokeWidth > 0) {
        for (int i = 0; i < m_points.length(); i++) {
            QPointF thisPoint = transform.map(m_points.at(i));

            if ((i + 1) < m_points.length()) {
                QPointF nextPoint = transform.map(m_points.at(i + 1));
                painter->setPen(state == SlippyMapLayerObject::SelectedState ? m_strokePenSelected : m_strokePen);
                painter->setBrush(Qt::NoBrush);
                painter->drawLine(thisPoint, nextPoint);
            }
        }
    }
    for (int i = 0; i < m_points.length(); i++) {
        QPointF thisPoint = transform.map(m_points.at(i));

        if ((i + 1) < m_points.length()) {
            QPointF nextPoint = transform.map(m_points.at(i + 1));
            painter->setPen(state == SlippyMapLayerObject::SelectedState ? m_linePenSelected : m_linePen);
            painter->setBrush(Qt::NoBrush);
            painter->drawLine(thisPoint, nextPoint);
        }
    }
}

bool SlippyMapLayerPath::isIntersectedBy(const QRectF &rect) const
{
    for (QPointF point : m_points) {
        if (rect.contains(point)) {
            return true;
        }
    }

    return false;
}

bool SlippyMapLayerPath::contains(const QPointF &point, int zoom) const
{
    double deg_per_pixel = (360.0 / pow(2.0, zoom)) / 256.0;
    double deg_lineWidth = deg_per_pixel * m_lineWidth;

    for (int i = 0; i < m_points.length() - 1; i++) {
        QPointF p1 = m_points.at(i);
        QPointF p2 = m_points.at(i+1);

        QRectF bbox = {
                p1.x(),
                p1.y(),
                p2.x() - p1.x(),
                p2.y() - p1.y()};

        if (!bbox.contains(point)) continue;

        //
        // vertical line, so if point.x is within +/- deg_lineWidth of
        // the line, then it's a hit
        //
        if ((p2.x() - p1.x()) == 0) {
            if ((p1.x() - (deg_lineWidth / 2)) < point.x() && point.x() < (p1.x() + (deg_lineWidth / 2))) {
                return true;
            }
        }
        //
        // same for horizontal line
        //
        else if ((p2.y() - p1.y()) == 0) {
            if ((p1.y() - (deg_lineWidth / 2)) < point.y() && point.y() < (p1.y() + (deg_lineWidth / 2))) {
                return true;
            }
        }

        double slope = (p2.y() - p1.y()) / (p2.x() - p1.x());
        double intercept = p1.y() - slope * p1.x();

        double intersect_y = (slope * point.x()) + intercept;
        double intersect_x = (intersect_y - intercept) / slope;

        QRectF deg_rect(
                intersect_x - (deg_lineWidth),
                intersect_y - (deg_lineWidth),
                deg_lineWidth * 2,
                deg_lineWidth * 2);

        if (deg_rect.contains(point)) {
            return true;
        }
    }

    return false;
}

const QPointF SlippyMapLayerPath::position() const
{
    const qreal MIN = std::numeric_limits<qreal>().min();
    const qreal MAX = std::numeric_limits<qreal>().max();
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

void SlippyMapLayerPath::setPosition(const QPointF &position)
{
    QPointF curr = this->position();
    QPointF diff = position - curr;
    for (auto& point : m_points) {
        point += diff;
    }
    emit updated();
}

const QSizeF SlippyMapLayerPath::size() const
{
    QPolygonF poly(m_points);
    QRectF boundingRect = poly.boundingRect();
    return boundingRect.size();
}

void SlippyMapLayerPath::initStyle()
{
    m_linePen.setStyle(Qt::SolidLine);
    m_linePen.setColor(m_lineColor);
    m_linePen.setWidth(m_lineWidth);
    m_linePen.setCapStyle(Qt::RoundCap);

    m_linePenSelected.setStyle(Qt::SolidLine);
    m_linePenSelected.setColor(m_lineColor.lighter());
    m_linePenSelected.setWidth(m_lineWidth);
    m_linePenSelected.setCapStyle(Qt::RoundCap);

    m_strokePen.setStyle(Qt::SolidLine);
    m_strokePen.setColor(m_strokeColor);
    m_strokePen.setWidth(m_lineWidth + (m_strokeWidth * 2));
    m_strokePen.setCapStyle(Qt::RoundCap);

    m_strokePenSelected.setStyle(Qt::SolidLine);
    m_strokePenSelected.setColor(m_strokeColor.lighter());
    m_strokePenSelected.setWidth(m_lineWidth + (m_strokeWidth * 2));
    m_strokePenSelected.setCapStyle(Qt::RoundCap);
}

void SlippyMapLayerPath::setLineWidth(int width)
{
    m_lineWidth = width;
    initStyle();
    emit updated();
}

void SlippyMapLayerPath::setLineColor(const QColor &color)
{
    m_lineColor = color;
    initStyle();
    emit updated();
}

void SlippyMapLayerPath::setStrokeWidth(int width)
{
    m_strokeWidth = width;
    initStyle();
    emit updated();
}

void SlippyMapLayerPath::setStrokeColor(const QColor &color)
{
    m_strokeColor = color;
    initStyle();
    emit updated();
}

int SlippyMapLayerPath::lineWidth() const
{
    return m_lineWidth;
}

const QColor &SlippyMapLayerPath::lineColor() const
{
    return m_lineColor;
}

int SlippyMapLayerPath::strokeWidth() const
{
    return m_strokeWidth;
}

const QColor &SlippyMapLayerPath::strokeColor() const
{
    return m_strokeColor;
}

QList<SlippyMapLayerObjectPropertyPage *> SlippyMapLayerPath::propertyPages(SlippyMapLayerObject::Ptr object) const
{
    return {};
}

void SlippyMapLayerPath::setPoints(const QVector<QPointF> &points)
{
    m_points = points;
}

const QVector<QPointF> &SlippyMapLayerPath::points() const
{
    return m_points;
}

SlippyMapLayerPath *SlippyMapLayerPath::clone() const
{
    return new SlippyMapLayerPath(*this);
}

