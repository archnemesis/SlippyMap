#include "SlippyMap/SlippyMapLayerObject.h"

using namespace SlippyMap;

SlippyMapLayerObject::SlippyMapLayerObject(QObject *parent) : QObject(parent)
{

}

const QString &SlippyMapLayerObject::label() const
{
    return m_label;
}

const QString &SlippyMapLayerObject::description() const
{
    return m_description;
}

void SlippyMapLayerObject::setLabel(const QString &label)
{
    m_label = label;
    emit labelChanged();
}

void SlippyMapLayerObject::setDescription(const QString &description)
{
    m_description = description;
    emit descriptionChanged();
}

void SlippyMapLayerObject::setVisible(bool visible)
{
    m_visible = visible;
    emit visibilityChanged();
}

void SlippyMapLayerObject::setMovable(bool movable)
{
    m_movable = movable;
    emit movabilityChanged();
}

void SlippyMapLayerObject::setActive(bool active)
{
    m_active = active;
    emit activeChanged();
}

bool SlippyMapLayerObject::isVisible() const
{
    return m_visible;
}

bool SlippyMapLayerObject::isMovable() const
{
    return m_movable;
}

bool SlippyMapLayerObject::isActive() const
{
    return m_active;
}