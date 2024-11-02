//
// Created by Robin on 11/1/2024.
//

#include "SlippyMap/SlippyMapLayerPath.h"

using namespace SlippyMap;

SlippyMapLayerPath::SlippyMapLayerPath(const QVector<QPointF> &points) :
        SlippyMapLayerObject(),
        m_points(points),
        m_lineWidth(10),
        m_strokeWidth(1)
{
    initStyle();
}

void SlippyMapLayerPath::draw(QPainter *painter, const QTransform &transform, SlippyMapLayerObject::ObjectState state)
{
    if (m_strokeWidth > 0) {
        for (int i = 0; i < m_points.length(); i++) {
            QPointF thisPoint = transform.map(m_points.at(i));

            if ((i + 1) < m_points.length()) {
                QPointF nextPoint = transform.map(m_points.at(i + 1));
                painter->setPen(m_strokePen);
                painter->setBrush(Qt::NoBrush);
                painter->drawLine(thisPoint, nextPoint);
            }
        }
    }
    for (int i = 0; i < m_points.length(); i++) {
        QPointF thisPoint = transform.map(m_points.at(i));

        if ((i + 1) < m_points.length()) {
            QPointF nextPoint = transform.map(m_points.at(i + 1));
            painter->setPen(m_linePen);
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
    double deg_radius = deg_per_pixel * 10;

    for (int i = 0; i < m_points.length(); i++) {
        QRectF deg_rect(
                m_points.at(i).x() - deg_radius,
                m_points.at(i).y() - deg_radius,
                deg_radius * 2,
                deg_radius * 2);

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

    m_strokePen.setStyle(Qt::SolidLine);
    m_strokePen.setColor(m_strokeColor);
    m_strokePen.setWidth(m_lineWidth + (m_strokeWidth * 2));
    m_strokePen.setCapStyle(Qt::RoundCap);
}

void SlippyMapLayerPath::unserialize(QDataStream &stream)
{

}

QDataStream &SlippyMapLayerPath::serialize(QDataStream &stream) const
{
    return stream;
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

QList<SlippyMapLayerObjectPropertyPage *> SlippyMapLayerPath::propertyPages() const
{
    return {};
}

