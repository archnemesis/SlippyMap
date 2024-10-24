#include "SlippyMap/SlippyMapLayerMarker.h"

#include <QPainter>
#include <QPoint>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QFontMetrics>
#include <QPalette>
#include <QGuiApplication>

#include <math.h>

using namespace SlippyMap;

SlippyMapLayerMarker::SlippyMapLayerMarker(QObject *parent) :
        SlippyMapLayerObject (parent),
        m_color(Qt::white),
        m_radius(5)
{
    initStyle();
}

SlippyMapLayerMarker::SlippyMapLayerMarker(const QPointF &position, QObject *parent) :
        SlippyMapLayerMarker (parent)
{
    m_position = position;
}

// QDataStream &operator<<(QDataStream &stream, const SlippyMapLayerMarker &marker)
// {
//     stream << marker.label();
//     stream << marker.description();
//     stream << marker.position();
//     stream << marker.radius();
//     stream << marker.color();
//     stream << marker.icon();
//
//     return stream;
// }
//
// QDataStream &operator>>(QDataStream &stream, SlippyMapLayerMarker &marker)
// {
//     QString label;
//     QString description;
//     QPointF position;
//     int radius;
//     QColor color;
//     QImage icon;
//
//     stream >> label;
//     stream >> description;
//     stream >> position;
//     stream >> radius;
//     stream >> color;
//     stream >> icon;
//
//     marker.setLabel(label);
//     marker.setDescription(description);
//     marker.setPosition(position);
//     marker.setRadius(radius);
//     marker.setColor(color);
//     marker.setIcon(icon);
//
//     return stream;
// }

void SlippyMapLayerMarker::draw(QPainter *painter, const QTransform &transform, int zoom)
{
    QPointF pos = transform.map(m_position);

    if (!m_pixmap.isNull()) {
        int pixmap_w = m_pixmap.width();
        int pixmap_h = m_pixmap.height();
        int offset_x = pixmap_w / 2;
        int offset_y = pixmap_h / 2;

        painter->drawPixmap(pos.x() - offset_x, pos.y() - offset_y, pixmap_w, pixmap_h, m_pixmap);

        QFontMetrics metrics(painter->font());

        if (m_label.length() > 0) {
            qint32 label_w = metrics.horizontalAdvance(m_label);
            qint32 label_h = metrics.height();
            qint32 label_x = static_cast<qint32>(pos.x()) - (label_w / 2);
            qint32 label_y = static_cast<qint32>(pos.y()) - (pixmap_h / 2) - (label_h + 10);
            painter->setBrush(m_labelBrush);
            painter->setPen(m_labelPen);
            painter->drawRoundedRect(
                    label_x - 5,
                    label_y - 5,
                    label_w + 10,
                    label_h + 10,
                    5, 5);
            painter->setBrush(m_labelTextBrush);
            painter->setPen(m_labelTextPen);
            painter->drawText(
                    label_x,
                    label_y + (label_h / 2) + 5,
                    m_label);
        }
    }
    else {
        if (isActive()) {
            painter->setBrush(m_activeDotBrush);
            painter->setPen(m_activeDotPen);
            painter->drawEllipse(pos, m_radius + 2, m_radius + 2);
        }

        painter->setBrush(m_dotBrush);
        painter->setPen(m_dotPen);
        painter->drawEllipse(pos, m_radius, m_radius);

        QFontMetrics metrics(painter->font());

        if (m_label.length() > 0) {
            qint32 label_w = metrics.horizontalAdvance(m_label);
            qint32 label_h = metrics.height();
            qint32 label_x = static_cast<qint32>(pos.x()) - (label_w / 2);
            qint32 label_y = static_cast<qint32>(pos.y()) - (label_h + 15);
            painter->setBrush(m_labelBrush);
            painter->setPen(m_labelPen);
            painter->drawRoundedRect(
                    label_x - 5,
                    label_y - 5,
                    label_w + 10,
                    label_h + 10,
                    5, 5);
            painter->setBrush(m_labelTextBrush);
            painter->setPen(m_labelTextPen);
            painter->drawText(
                    label_x,
                    label_y + (label_h / 2) + 5,
                    m_label);
        }
    }
}

bool SlippyMapLayerMarker::isIntersectedBy(const QRectF &rect) const
{
    return rect.contains(m_position);
}

bool SlippyMapLayerMarker::contains(const QPointF &point, int zoom) const
{
    double deg_per_pixel = (360.0 / pow(2.0, zoom)) / 256.0;
    double deg_radius = deg_per_pixel * m_radius;
    QRectF deg_rect(
            m_position.x() - deg_radius,
            m_position.y() - deg_radius,
            deg_radius * 2,
            deg_radius * 2);
    return deg_rect.contains(point);
}

void SlippyMapLayerMarker::setPosition(const QPointF &position)
{
    m_position = position;
}

void SlippyMapLayerMarker::setColor(const QColor &color)
{
    m_color = color;
}

void SlippyMapLayerMarker::setRadius(int radius)
{
    m_radius = radius;
}

void SlippyMapLayerMarker::setEditable(bool editable)
{
    m_editable = editable;
}

void SlippyMapLayerMarker::setIcon(const QImage &icon)
{
    m_icon = icon;
    m_pixmap = QPixmap::fromImage(m_icon);
}

bool SlippyMapLayerMarker::isEditable() const
{
    return m_editable;
}

const QPointF& SlippyMapLayerMarker::position() const
{
    return m_position;
}

const QColor &SlippyMapLayerMarker::color() const
{
    return m_color;
}

const QImage &SlippyMapLayerMarker::icon() const
{
    return m_icon;
}

const QString SlippyMapLayerMarker::statusBarText() const
{
    return m_label;
}

int SlippyMapLayerMarker::radius() const
{
    return m_radius;
}

void SlippyMapLayerMarker::initStyle()
{
    QPalette systemPalette = QGuiApplication::palette();

    m_dotBrush.setStyle(Qt::SolidPattern);
    m_dotBrush.setColor(systemPalette.highlight().color());
    m_dotPen.setStyle(Qt::NoPen);

    m_activeDotBrush.setStyle(Qt::NoBrush);
    m_activeDotPen.setColor(systemPalette.highlight().color());
    m_activeDotPen.setWidth(2);

    m_labelBrush.setStyle(Qt::SolidPattern);
    m_labelBrush.setColor(systemPalette.window().color());
    m_labelPen.setStyle(Qt::SolidLine);
    m_labelPen.setColor(systemPalette.dark().color());

    m_labelTextBrush.setStyle(Qt::NoBrush);
    m_labelTextPen.setStyle(Qt::SolidLine);
    m_labelTextPen.setColor(systemPalette.text().color());
}