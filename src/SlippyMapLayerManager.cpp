#include "SlippyMap/SlippyMapLayerManager.h"
#include "SlippyMap/SlippyMapLayer.h"
#include "SlippyMap/SlippyMapLayerObject.h"
#include "SlippyMap/SlippyMapAnimatedLayer.h"

#include <QDebug>
#include <QFile>

using namespace SlippyMap;

SlippyMapLayerManager::SlippyMapLayerManager(QObject *parent) : QAbstractItemModel(parent)
{
    //m_hiddenFont.setItalic(true);
    m_activeFont.setBold(true);
}

QModelIndex SlippyMapLayerManager::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    if (!parent.isValid()) {
        if (m_layers.count() > row) {
            return createIndex(row, column, m_layers.at(row).get());
        }
        else {
            return QModelIndex();
        }
    }
    else {
        auto *ptr = static_cast<SlippyMapLayer*>(parent.internalPointer());

        for (const auto& layer: m_layers) {
            if (layer.get() == ptr)
                return createIndex(row, column, layer->objects().at(row).get());
        }

        return QModelIndex();
    }
}

QModelIndex SlippyMapLayerManager::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    auto *ptr =static_cast<SlippyMapLayer*>(index.internalPointer());
    for (const auto& layer: m_layers) {
        if (layer == ptr)
            return QModelIndex();
    }

    SlippyMapLayerObject *obj = static_cast<SlippyMapLayerObject*>(index.internalPointer());

    for (auto &layer : m_layers) {
        for (const auto& object: layer->objects()) {
            if (object == obj)
                return createIndex(
                        layer->objects().indexOf(object),
                        0,
                        layer.get());
        }
    }

    return QModelIndex();
}

int SlippyMapLayerManager::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) {
        return m_layers.count();
    }

    SlippyMapLayer *ptr = static_cast<SlippyMapLayer*>(parent.internalPointer());
    if (ptr != nullptr) {
        for (auto const& layer: m_layers) {
            if (layer == ptr)
                return layer->objects().length();
        }
    }

    return 0;
}

int SlippyMapLayerManager::columnCount(const QModelIndex &parent) const
{
    (void)parent;
    return 1;
}

QVariant SlippyMapLayerManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.column() == 0) {
        auto ptr = static_cast<SlippyMapLayer *>(index.internalPointer());
        for (const auto &layer: m_layers) {
            if (layer == ptr) {
                switch (role) {
                    case Qt::DisplayRole:
                        return layer->name();
                    case Qt::FontRole:
                        if (!layer->isVisible()) {
                            return m_hiddenFont;
                        }
                        else if (m_activeLayer == layer) {
                            return m_activeFont;
                        }
                        return QVariant();
                    case Qt::ForegroundRole:
                        if (!layer->isVisible()) {
                            return QColor(Qt::lightGray);
                        }
                        return QVariant();
                    default:
                        return QVariant();
                }
            }
        }
    }

    SlippyMapLayerObject *obj = static_cast<SlippyMapLayerObject*>(index.internalPointer());

    for (const auto& layer : m_layers) {
        for (const auto& object: layer->objects()) {
            if (object == obj) {
                switch (role) {
                    case Qt::UserRole:
                        if (index.column() == 1)
                            return QVariant::fromValue<void*>(obj);
                        return QVariant();
                    case Qt::DisplayRole:
                        switch (index.column()) {
                            case 0:
                                return obj->label();
                            default:
                                return QVariant();
                        }
                    case Qt::FontRole:
                        if (!obj->isVisible()) {
                            return m_hiddenFont;
                        }
                        if (obj->isActive()) {
                            return m_activeFont;
                        }
                        return QVariant();
                    case Qt::ForegroundRole:
                        if (!obj->isVisible()) {
                            return QColor(Qt::lightGray);
                        }
                        return QVariant();
                }
            }
        }
    }

    return QVariant();
}

QVariant SlippyMapLayerManager::headerData(int section, Qt::Orientation orientation, int role) const
{
    (void)orientation;

    if (role != Qt::DisplayRole) {
        return {};
    }

    if (section == 0)
        return tr("Layers");

    return {};
}

void SlippyMapLayerManager::addLayer(SlippyMapLayer::Ptr layer)
{
    Q_CHECK_PTR(layer);

    int first = m_layers.count();
    int last = first + 1;
    beginInsertRows(QModelIndex(), first, last);
    m_layers.append(SlippyMapLayer::Ptr(layer));
    connect(layer.get(),
            &SlippyMapLayer::objectAdded,
            this,
            &SlippyMapLayerManager::layer_onObjectAdded);
    connect(layer.get(),
            &SlippyMapLayer::objectUpdated,
            this,
            &SlippyMapLayerManager::layer_onObjectUpdated);
    endInsertRows();
}

void SlippyMapLayerManager::addLayerObject(SlippyMapLayer::Ptr layer, const SlippyMapLayerObject::Ptr& object)
{
    Q_CHECK_PTR(layer);
    Q_CHECK_PTR(object);
    Q_ASSERT(m_layers.contains(SlippyMapLayer::Ptr(layer)));

    int first = layer->objects().count();
    int last = first + 1;
    beginInsertRows(createIndex(
            m_layers.indexOf(layer), 0, layer.get()), first, last);
    layer->addObject(object);
    connect(object.get(), &SlippyMapLayerObject::updated, [this, object]() {
        emit layerObjectUpdated(object);
    });
    endInsertRows();
    emit layerObjectAdded(layer, object);
}

void SlippyMapLayerManager::layer_onObjectAdded(const SlippyMapLayerObject::Ptr& object)
{
    Q_CHECK_PTR(object);

    // find out which row it belongs to
    int r = 0;
    for (int i = 0; i < m_layers.size(); i++) {
        if (m_layers.at(i)->contains(object)) {
            r = i;
        }
    }

    QModelIndex begin = index(r, 0);
    QModelIndex end = index(m_layers.at(r)->indexOf(object), 0, index(r, 0));
    emit dataChanged(begin, end);
}

void SlippyMapLayerManager::addLayerObject(const SlippyMapLayerObject::Ptr& object)
{
    Q_CHECK_PTR(object);

    if (m_activeLayer == nullptr)
        m_activeLayer = m_defaultLayer;

    Q_ASSERT(m_activeLayer != nullptr);

    addLayerObject(m_activeLayer, object);
}

void SlippyMapLayerManager::removeLayerObject(const SlippyMapLayer::Ptr& layer, const SlippyMapLayerObject::Ptr& object)
{
    Q_CHECK_PTR(layer);
    Q_CHECK_PTR(object);
    Q_ASSERT(m_layers.contains(layer));

    beginRemoveRows(
            createIndex(m_layers.indexOf(layer), 0, layer.get()),
                layer->objects().indexOf(object),
                layer->objects().indexOf(object));

    layer->takeObject(object);
    endRemoveRows();
}

void SlippyMapLayerManager::removeLayerObjects(SlippyMapLayer::Ptr layer)
{
    Q_CHECK_PTR(layer);

    beginRemoveRows(
            createIndex(
                    m_layers.indexOf(layer),
                    0,
                    layer.get()),
            0, layer->objects().count() - 1);

    layer->removeAll();
    endRemoveRows();
}

void SlippyMapLayerManager::takeLayer(SlippyMapLayer::Ptr layer)
{
    Q_CHECK_PTR(layer);
    Q_ASSERT(m_layers.contains(SlippyMapLayer::Ptr(layer)));

    if (m_activeLayer == layer)
        m_activeLayer.clear();

    beginRemoveRows(QModelIndex(),
            m_layers.indexOf(layer),
            m_layers.indexOf(layer));
    m_layers.removeOne(layer);
    endRemoveRows();
}

void SlippyMapLayerManager::setActiveLayer(SlippyMapLayer::Ptr layer)
{
    Q_CHECK_PTR(layer);
    Q_ASSERT(m_layers.contains(layer));

    m_activeLayer = layer;
    emit dataChanged(
            index(0, 0),
            index(m_layers.count() - 1, 0));
}

void SlippyMapLayerManager::setDefaultLayer(SlippyMapLayer::Ptr layer)
{
    Q_CHECK_PTR(layer);
    Q_ASSERT(m_layers.contains(layer));

    m_defaultLayer = layer;
}

void SlippyMapLayerManager::saveToFile(QString fileName)
{
    QFile outfile(fileName);

    outfile.open(QIODevice::WriteOnly);

    QDataStream out(&outfile);
    out.setVersion(QDataStream::Qt_5_12);
    out << (quint32)0xA0B0C0D0;
    out << (quint16)m_layers.count();

    for (const auto& layer : m_layers) {
        out << layer->name();
        out << layer->description();
        out << layer->objects().count();

        for (SlippyMapLayerObject::Ptr object : layer->objects()) {
            out << object->label();

        }
    }
}

bool SlippyMapLayerManager::contains(const SlippyMapLayerObject::Ptr& object)
{
    for (const auto& layer : m_layers) {
        if (layer->contains(object)) {
            return true;
        }
    }
    return false;
}

bool SlippyMapLayerManager::containsLayer(SlippyMapLayer::Ptr layer)
{
    return m_layers.contains(layer);
}

QList<SlippyMapLayer::Ptr> SlippyMapLayerManager::layers()
{
    return m_layers;
}

SlippyMapLayer::Ptr SlippyMapLayerManager::activeLayer()
{
    return m_activeLayer;
}

SlippyMapLayer::Ptr SlippyMapLayerManager::defaultLayer()
{
    return m_defaultLayer;
}

QList<SlippyMapLayerObject::Ptr> SlippyMapLayerManager::objectsAtPoint(QPointF point, int zoomLevel)
{
    QList<SlippyMapLayerObject::Ptr> objects;

    for (const auto& layer : m_layers) {
        for (const auto& object: layer->objects()) {
            if (object->contains(point, zoomLevel)) {
                objects.append(object);
            }
        }
    }
    return objects;
}

void SlippyMapLayerManager::layer_onObjectUpdated(const SlippyMapLayerObject::Ptr& object)
{
    int r = 0;
    for (int i = 0; i < m_layers.size(); i++) {
        if (m_layers.at(i)->contains(object)) {
            r = i;
        }
    }

    QModelIndex begin = index(r, 0);
    QModelIndex end = index(m_layers.at(r)->indexOf(object), 0, index(r, 0));
    emit dataChanged(begin, end);
}

int SlippyMapLayerManager::objectCount() const
{
    int count = 0;
    for (const auto& layer : m_layers) {
        count += layer->objects().count();
    }
    return count;
}

void SlippyMapLayerManager::deactivateActiveObject()
{
    for (const auto& layer: m_layers)
        layer->deactivateAll();
}

void SlippyMapLayerManager::updateActiveLayer()
{
    int r = m_layers.indexOf(m_activeLayer);
    QModelIndex begin = index(r, 0);
    QModelIndex end = index(r, 0);
    emit dataChanged(begin, end);
}

void SlippyMapLayerManager::replaceObject(const SlippyMapLayerObject::Ptr& object, SlippyMapLayerObject::Ptr replacement)
{
    for (const auto& layer: m_layers) {
        if (layer->contains(object)) {
            layer->replace(object, replacement);
            layer_onObjectUpdated(replacement);
            return;
        }
    }
}
