#include "SlippyMap/SlippyMapLayerPolygon.h"
#include "SlippyMap/SlippyMapLayerPolygonPropertyPage.h"

#define NOMINMAX
#include <algorithm>
#include <QApplication>
#include <QDebug>

using namespace std;
using namespace SlippyMap;

SlippyMapLayerPolygon::SlippyMapLayerPolygon(QObject *parent) :
    SlippyMapLayerObject(parent),
    m_points({}),
    m_fillColor(qApp->palette().base().color()),
    m_strokeColor(qApp->palette().highlight().color()),
    m_strokeWidth(1)
{
    initStyle();
}

SlippyMapLayerPolygon::SlippyMapLayerPolygon(QVector<QPointF> points, QObject *parent) :
    SlippyMapLayerObject(parent),
    m_points(points),
    m_fillColor(qApp->palette().base().color()),
    m_strokeColor(qApp->palette().highlight().color()),
    m_strokeWidth(1)
{
    initStyle();
}

SlippyMapLayerPolygon::~SlippyMapLayerPolygon()
{

}

void SlippyMapLayerPolygon::draw(QPainter *painter, const QTransform &transform, ObjectState state)
{
    switch (state) {
    case NormalState:
        painter->setPen(m_strokePen);
        painter->setBrush(m_fillBrush);

        painter->save();
        painter->setWorldTransform(transform);
        painter->drawConvexPolygon(m_points.data(), m_points.length());
        painter->restore();

        break;
    case SelectedState:

        painter->save();

        painter->setPen(QPen(Qt::NoPen));
        painter->setBrush(m_selectedBrush);
        painter->setWorldTransform(transform);
        painter->drawConvexPolygon(m_points.data(), m_points.length());

        painter->setPen(m_selectedPen);
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawConvexPolygon(m_points.data(), m_points.length());

        painter->restore();


        /* resize handles */
        for (QPointF point : m_points) {
            QPointF mapped = transform.map(point);
            drawResizeHandle(painter, QPoint(static_cast<int>(mapped.x()), static_cast<int>(mapped.y())));
        }
        break;
    default:
        break;
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
    m_strokePen.setCosmetic(true);
    m_strokePen.setJoinStyle(Qt::MiterJoin);
    m_strokePen.setCapStyle(Qt::SquareCap);
    m_strokePen.setWidth(m_strokeWidth);
    m_strokePen.setColor(m_strokeColor);

    m_fillBrush.setStyle(Qt::SolidPattern);
    m_fillBrush.setColor(m_fillColor);
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
    QString label;
    QString description;
    QPointF position;
    QColor strokeColor;
    int strokeWidth;
    QColor fillColor;
    QVector<QPointF> points;

    stream >> label;
    stream >> description;
    stream >> position;
    stream >> strokeColor;
    stream >> strokeWidth;
    stream >> fillColor;
    stream >> points;

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

QList<SlippyMapLayerObjectPropertyPage*> SlippyMapLayerPolygon::propertyPages() const
{
    return {new SlippyMapLayerPolygonPropertyPage((SlippyMapLayerObject *) this)};
}
