#include "SlippyMap/SlippyMapLayer.h"
#include "SlippyMap/SlippyMapLayerObject.h"

#include <QDebug>

using namespace SlippyMap;

SlippyMapLayer::SlippyMapLayer(QObject *parent) : QObject(parent)
{

}

void SlippyMapLayer::addObject(SlippyMapLayerObject *object)
{
    m_objects.append(object);
    connect(object, &SlippyMapLayerObject::activeChanged, this, &SlippyMapLayer::objectChanged);
    connect(object, &SlippyMapLayerObject::labelChanged, this, &SlippyMapLayer::objectChanged);
    connect(object, &SlippyMapLayerObject::visibilityChanged, this, &SlippyMapLayer::objectChanged);
    emit objectAdded(object);
}

void SlippyMapLayer::takeObject(SlippyMapLayerObject *object)
{
    m_objects.removeOne(object);
    emit objectRemoved(object);
}

QList<SlippyMapLayerObject *> SlippyMapLayer::objects() const
{
    return m_objects;
}

QString SlippyMapLayer::name() const
{
    return m_name;
}

QString SlippyMapLayer::description() const
{
    return m_description;
}

bool SlippyMapLayer::isVisible()
{
    return m_visible;
}

void SlippyMapLayer::setName(const QString &name)
{
    m_name = name;
}

void SlippyMapLayer::setDescription(const QString &description)
{
    m_description = description;
}

void SlippyMapLayer::setVisible(const bool visible)
{
    m_visible = visible;
}

bool SlippyMapLayer::contains(SlippyMapLayerObject *object)
{
    return m_objects.contains(object);
}

int SlippyMapLayer::indexOf(SlippyMapLayerObject *object)
{
    return m_objects.indexOf(object);
}

void SlippyMapLayer::deactivateAll()
{
    for (SlippyMapLayerObject *object : m_objects) {
        object->setActive(false);
    }
}

void SlippyMapLayer::removeAll()
{
    for (SlippyMapLayerObject *object : m_objects) {
        delete object;
    }

    m_objects.clear();
}

void SlippyMapLayer::showAll()
{
    for (SlippyMapLayerObject *object : m_objects) {
        object->setVisible(true);
    }
}

void SlippyMapLayer::hideAll()
{
    for (SlippyMapLayerObject *object : m_objects) {
        object->setVisible(false);
    }
}

void SlippyMapLayer::objectChanged()
{
    emit objectUpdated(qobject_cast<SlippyMapLayerObject*>(sender()));
}

void SlippyMapLayer::setEditable(const bool editable)
{
    m_editable = editable;
}

bool SlippyMapLayer::isEditable()
{
    return m_editable;
}

void SlippyMapLayer::replace(SlippyMapLayerObject *object, SlippyMapLayerObject *replacement)
{
    if (m_objects.contains(object))
        m_objects.replace(m_objects.indexOf(object), replacement);
}

QVariant SlippyMapLayer::id() const
{
    return m_id;
}

void SlippyMapLayer::setId(const QVariant &id)
{
    m_id = id;
}

bool SlippyMapLayer::isSynced()
{
    return m_synced;
}

void SlippyMapLayer::setSynced(bool synced)
{
    m_synced = synced;
}

QDataStream &operator<<(QDataStream &stream, const SlippyMapLayer *layer)
{
    stream << layer->id();
    stream << layer->name();
    stream << layer->description();
    stream << layer->objects().length();
    return stream;
}

QDataStream &operator>>(QDataStream &stream, SlippyMapLayer *layer)
{
    QVariant id;
    QString name;
    QString description;
    int objectCount;

    stream >> id;
    stream >> name;
    stream >> description;
    stream >> objectCount;

    layer->setId(id);
    layer->setName(name);
    layer->setDescription(description);

    for (int i = 0; i < objectCount; i++) {

    }

    return stream;
}
