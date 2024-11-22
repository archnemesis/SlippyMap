#include "SlippyMap/SlippyMapLayerObject.h"
#include <QDebug>

using namespace SlippyMap;

SlippyMapLayerObject::SlippyMapLayerObject(QObject *parent) : QObject(parent)
{
    m_pen.setStyle(Qt::SolidLine);
    m_pen.setCosmetic(true);
    m_pen.setWidth(2);
    m_pen.setColor(QColor(153, 153, 102, 128));
    m_pen.setJoinStyle(Qt::MiterJoin);
    m_pen.setCapStyle(Qt::SquareCap);

    m_brush.setStyle(Qt::SolidPattern);
    m_brush.setColor(QColor(255, 255, 51, 128));

    m_selectionHandlePen.setColor(Qt::black);
    m_selectionHandlePen.setStyle(Qt::SolidLine);
    m_selectionHandlePen.setCosmetic(true);
    m_selectionHandlePen.setWidth(1);
    m_selectionHandlePen.setJoinStyle(Qt::MiterJoin);
    m_selectionHandlePen.setCapStyle(Qt::SquareCap);

    m_selectionHandleBrush.setColor(Qt::white);
    m_selectionHandleBrush.setStyle(Qt::SolidPattern);

    m_activePen.setStyle(Qt::DotLine);
    m_activePen.setCosmetic(true);
    m_activePen.setColor(Qt::white);
    m_activePen.setWidth(2);
    m_activeBrush.setStyle(Qt::SolidPattern);
    m_activeBrush.setColor(Qt::gray);

    m_selectedPen.setStyle(Qt::DotLine);
    m_selectedPen.setCosmetic(true);
    m_selectedPen.setColor(Qt::darkBlue);
    m_selectedPen.setWidth(2);
    m_selectedBrush.setStyle(Qt::SolidPattern);
    m_selectedBrush.setColor(Qt::lightGray);
}

void SlippyMapLayerObject::setMovable(bool movable)
{
    m_movable = true;
}

void SlippyMapLayerObject::setActive(bool active)
{
    m_active = active;
    emit activeChanged();
}

QString SlippyMapLayerObject::label() const
{
    return m_label;
}

QString SlippyMapLayerObject::description() const
{
    return m_description;
}

void SlippyMapLayerObject::setLabel(const QString &name)
{
    m_label = name;
    emit labelChanged();
    emit updated();
}

void SlippyMapLayerObject::setDescription(const QString &description)
{
    m_description = description;
    emit updated();
}

void SlippyMapLayerObject::setVisible(bool visible)
{
    m_visible = visible;
    emit visibilityChanged();
    emit updated();
}

bool SlippyMapLayerObject::isVisible()
{
    return m_visible;
}

bool SlippyMapLayerObject::isActive()
{
    return m_active;
}

void SlippyMapLayerObject::setBrush(QBrush brush)
{
    m_brush = brush;
}

void SlippyMapLayerObject::setPen(QPen pen)
{
    m_pen = pen;
}

bool SlippyMapLayerObject::isMovable()
{
    return m_movable;
}

QString SlippyMapLayerObject::statusBarText()
{
    return {""};
}

void SlippyMapLayerObject::drawResizeHandle(QPainter *painter, QPoint point) const
{
    painter->setBrush(m_selectionHandleBrush);
    painter->setPen(m_selectionHandlePen);
    painter->drawRect(
                (point.x() - (m_resizeHandleWidth / 2)),
                (point.y() - (m_resizeHandleWidth / 2)),
                m_resizeHandleWidth,
                m_resizeHandleWidth);
}

bool SlippyMapLayerObject::isEditable()
{
    return m_editable;
}

void SlippyMapLayerObject::setEditable(bool editable)
{
    m_editable = editable;
}

void SlippyMapLayerObject::copy(SlippyMapLayerObject *other)
{

}

SlippyMapLayerObject::~SlippyMapLayerObject()
{
    qDebug() << "Object deleted";
}

void SlippyMapLayerObject::setId(const QVariant &id)
{
    m_id = id;
}

void SlippyMapLayerObject::setLayerId(const QVariant& layerId)
{
    m_layerId = layerId;
}

QVariant SlippyMapLayerObject::id() const
{
    return m_id;
}

QVariant SlippyMapLayerObject::layerId() const
{
    return m_layerId;
}

void SlippyMapLayerObject::setSynced(bool synced)
{
    m_synced = synced;
}

bool SlippyMapLayerObject::isSynced() const
{
    return m_synced;
}

