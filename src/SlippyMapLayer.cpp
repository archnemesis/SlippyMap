#include "SlippyMap/SlippyMapLayer.h"
#include "SlippyMap/SlippyMapLayerObject.h"

#include <QDebug>

using namespace SlippyMap;

SlippyMapLayer::SlippyMapLayer(QObject *parent) : QObject(parent)
{

}

void SlippyMapLayer::addObject(SlippyMapLayerObject::Ptr object)
{
    m_objects.append(object);
    connect(object.get(), &SlippyMapLayerObject::activeChanged, this, &SlippyMapLayer::objectChanged);
    connect(object.get(), &SlippyMapLayerObject::labelChanged, this, &SlippyMapLayer::objectChanged);
    connect(object.get(), &SlippyMapLayerObject::visibilityChanged, this, &SlippyMapLayer::objectChanged);
    emit objectAdded(object);
}

void SlippyMapLayer::takeObject(SlippyMapLayerObject::Ptr object)
{
    m_objects.removeOne(object);
    emit objectRemoved(object);
}

QList<SlippyMapLayerObject::Ptr> SlippyMapLayer::objects() const
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

int SlippyMapLayer::order() const
{
    return m_zOrder;
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

bool SlippyMapLayer::contains(SlippyMapLayerObject::Ptr object)
{
    return m_objects.contains(object);
}

int SlippyMapLayer::indexOf(SlippyMapLayerObject::Ptr object)
{
    return m_objects.indexOf(object);
}

void SlippyMapLayer::deactivateAll()
{
    for (const auto& object: m_objects) {
        object->setActive(false);
    }
}

void SlippyMapLayer::removeAll()
{
    m_objects.clear();
}

void SlippyMapLayer::showAll()
{
    for (const auto& object: m_objects) {
        object->setVisible(true);
    }
}

void SlippyMapLayer::hideAll()
{
    for (const auto& object: m_objects) {
        object->setVisible(false);
    }
}

void SlippyMapLayer::objectChanged()
{
    auto *ptr = qobject_cast<SlippyMapLayerObject*>(sender());
    for (const auto& object: m_objects) {
        if (object == ptr)
            emit objectUpdated(object);
    }
}

void SlippyMapLayer::setEditable(const bool editable)
{
    m_editable = editable;
}

bool SlippyMapLayer::isEditable()
{
    return m_editable;
}

void SlippyMapLayer::replace(SlippyMapLayerObject::Ptr object, SlippyMapLayerObject::Ptr replacement)
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

void SlippyMapLayer::setOrder(int order)
{
    m_zOrder = order;
}

SlippyMapLayer::~SlippyMapLayer()
{
    qDebug() << "Layer" << name() << "deleted";
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
