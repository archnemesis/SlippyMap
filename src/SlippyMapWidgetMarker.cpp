#include "SlippyMap/SlippyMapWidgetMarker.h"
#include "SlippyMap/SlippyMapLayerMarkerPropertyPage.h"

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

SlippyMapWidgetMarker::SlippyMapWidgetMarker(QObject *parent) :
        SlippyMapLayerObject (parent),
        m_color(qApp->palette().highlight().color()),
        m_radius(10)
{
    initStyle();
}

SlippyMapWidgetMarker::SlippyMapWidgetMarker(const QPointF &position, QObject *parent) :
        SlippyMapWidgetMarker (parent)
{
    m_position = position;
    initStyle();
}

SlippyMapWidgetMarker::SlippyMapWidgetMarker(const SlippyMapWidgetMarker &other)
{
    setLabel(other.label());
    setDescription(other.description());
    SlippyMapWidgetMarker::setPosition(other.position());
    setRadius(other.radius());
    setColor(other.color());
    setIcon(other.icon());
}

QDataStream &operator<<(QDataStream &stream, const SlippyMapWidgetMarker marker)
{
    stream << marker.label();
    stream << marker.description();
    stream << marker.position();
    stream << marker.radius();
    stream << marker.color();
    stream << marker.icon();

    return stream;
}

QDataStream &operator>>(QDataStream &stream, SlippyMapWidgetMarker &marker)
{
    QString label;
    QString description;
    QPointF position;
    int radius;
    QColor color;
    QImage icon;

    stream >> label;
    stream >> description;
    stream >> position;
    stream >> radius;
    stream >> color;
    stream >> icon;

    marker.setLabel(label);
    marker.setDescription(description);
    marker.setPosition(position);
    marker.setRadius(radius);
    marker.setColor(color);
    marker.setIcon(icon);

    return stream;
}

void SlippyMapWidgetMarker::draw(QPainter *painter, const QTransform &transform, ObjectState state)
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
        if (state == ObjectState::SelectedState) {
            painter->setBrush(m_activeDotBrush);
            painter->setPen(m_activeDotPen);
            painter->drawEllipse(pos, m_radius + 5, m_radius + 5);
        }

        painter->setBrush(m_dotBrush);
        painter->setPen(m_dotPen);
        painter->drawEllipse(pos, m_radius, m_radius);

        QFontMetrics metrics(painter->font());

        if (m_label.length() > 0) {
            qint32 label_w = metrics.horizontalAdvance(m_label);
            qint32 label_h = metrics.height();
            qint32 label_x = static_cast<qint32>(pos.x()) - (label_w / 2);
            qint32 label_y = static_cast<qint32>(pos.y()) - (label_h + m_radius + 15);
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

bool SlippyMapWidgetMarker::isIntersectedBy(const QRectF &rect) const
{
    return rect.contains(m_position);
}

bool SlippyMapWidgetMarker::contains(const QPointF &point, int zoom) const
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

const QSizeF SlippyMapWidgetMarker::size() const
{
    double diameter = m_radius * 2;
    return QSizeF(diameter, diameter);
}

void SlippyMapWidgetMarker::setPosition(const QPointF &position)
{
    m_position = position;
    emit updated();
}

void SlippyMapWidgetMarker::setColor(const QColor &color)
{
    m_color = color;
    initStyle();
    emit updated();
}

void SlippyMapWidgetMarker::setRadius(int radius)
{
    m_radius = radius;
    emit updated();
}

void SlippyMapWidgetMarker::setIcon(const QImage &icon)
{
    m_icon = icon;
    m_pixmap = QPixmap::fromImage(m_icon);
    emit updated();
}

const QPointF SlippyMapWidgetMarker::position() const
{
    return m_position;
}

const QColor &SlippyMapWidgetMarker::color() const
{
    return m_color;
}

const QImage &SlippyMapWidgetMarker::icon() const
{
    return m_icon;
}

const QString SlippyMapWidgetMarker::statusBarText() const
{
    return m_label;
}

int SlippyMapWidgetMarker::radius() const
{
    return m_radius;
}

void SlippyMapWidgetMarker::initStyle()
{
    QPalette systemPalette = QGuiApplication::palette();

    m_dotBrush.setStyle(Qt::SolidPattern);
    m_dotBrush.setColor(m_color);
    m_dotPen.setStyle(Qt::NoPen);

    m_activeDotBrush.setStyle(Qt::NoBrush);
    m_activeDotPen.setColor(m_color);
    m_activeDotPen.setWidth(3);

    m_labelBrush.setStyle(Qt::SolidPattern);
    m_labelBrush.setColor(systemPalette.window().color());
    m_labelPen.setStyle(Qt::SolidLine);
    m_labelPen.setColor(systemPalette.dark().color());

    m_labelTextBrush.setStyle(Qt::NoBrush);
    m_labelTextPen.setStyle(Qt::SolidLine);
    m_labelTextPen.setColor(systemPalette.text().color());
}

QDataStream& SlippyMapWidgetMarker::serialize(QDataStream& stream) const
{
    stream << label();
    stream << description();
    stream << position();
    stream << radius();
    stream << color();
    stream << icon();
    return stream;
}

void SlippyMapWidgetMarker::unserialize(QDataStream& stream)
{
    QString label;
    QString description;
    QPointF position;
    int radius;
    QColor color;
    QImage icon;

    stream >> label;
    stream >> description;
    stream >> position;
    stream >> radius;
    stream >> color;
    stream >> icon;

    setLabel(label);
    setDescription(description);
    setPosition(position);
    setRadius(radius);
    setColor(color);
    setIcon(icon);
    initStyle();
}

QList<SlippyMapLayerObjectPropertyPage*> SlippyMapWidgetMarker::propertyPages() const
{
    return {new SlippyMapLayerMarkerPropertyPage((SlippyMapLayerObject *) this)};
}

SlippyMapLayerObject *SlippyMapWidgetMarker::clone() const
{
    return new SlippyMapWidgetMarker(*this);
}

void SlippyMapWidgetMarker::copy(SlippyMapLayerObject *other)
{
    auto *marker = dynamic_cast<SlippyMapWidgetMarker*>(other);
    setLabel(marker->label());
    setDescription(marker->description());
    setRadius(marker->radius());
    setColor(marker->color());
    setIcon(marker->icon());
    setPosition(marker->position());
    initStyle();
    emit updated();
}


