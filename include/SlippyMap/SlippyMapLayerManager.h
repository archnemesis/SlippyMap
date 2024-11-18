#ifndef SLIPPYMAPLAYERMANAGER_H
#define SLIPPYMAPLAYERMANAGER_H

#include <SlippyMap/SlippyMap.h>
#include <SlippyMap/SlippyMapLayer.h>
#include <SlippyMap/SlippyMapAnimatedLayer.h>
#include <SlippyMap/SlippyMapLayerObject.h>

#include <QAbstractItemModel>
#include <QList>
#include <QFont>

namespace SlippyMap
{
    class SlippyMapLayerObject;

    class SLIPPYMAPSHARED_EXPORT SlippyMapLayerManager : public QAbstractItemModel
    {
        Q_OBJECT
    public:
        explicit SlippyMapLayerManager(QObject *parent = nullptr);

        enum LayerSortMethod {
            LayerSortOrder,
            LayerSortName
        };

        static bool sortComparisonName(const SlippyMapLayer::Ptr& left, const SlippyMapLayer::Ptr& right);
        static bool sortComparisonOrder(const SlippyMapLayer::Ptr& left, const SlippyMapLayer::Ptr& right);

        QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
        QModelIndex parent(const QModelIndex &index) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;
        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

        void addLayer(SlippyMapLayer::Ptr layer);
        void addLayerObject(SlippyMapLayer::Ptr layer, const SlippyMapLayerObject::Ptr& object);
        void addLayerObject(const SlippyMapLayerObject::Ptr& object);
        void removeLayerObject(const SlippyMapLayer::Ptr& layer, const SlippyMapLayerObject::Ptr& object);
        void removeLayerObjects(SlippyMapLayer::Ptr layer);
        void replaceObject(const SlippyMapLayerObject::Ptr& object, SlippyMapLayerObject::Ptr replacement);
        void takeLayer(SlippyMapLayer::Ptr layer);
        void setActiveLayer(SlippyMapLayer::Ptr layer);
        void deactivateLayer();
        void setDefaultLayer(SlippyMapLayer::Ptr layer);
        void deactivateActiveObject();
        void updateActiveLayer();
        void sort(LayerSortMethod method, Qt::SortOrder order);
        void saveToFile(QString fileName);
        void setVisbleObjectsList(const QList<SlippyMapLayerObject::Ptr>& visibleObjects);

        int objectCount() const;
        bool contains(const SlippyMapLayerObject::Ptr& object);
        bool containsLayer(SlippyMapLayer::Ptr layer);

        QList<SlippyMapLayer::Ptr> layers();
        SlippyMapLayer::Ptr activeLayer();
        SlippyMapLayer::Ptr defaultLayer();
        SlippyMapLayer::Ptr layerForObject(const SlippyMapLayerObject::Ptr& object);
        QList<SlippyMapLayerObject::Ptr> objectsAtPoint(QPointF point, int zoomLevel);

    signals:
        void activeLayerChanged(SlippyMapLayer::Ptr layer);
        void layerAdded(SlippyMapLayer::Ptr layer);
        void layerRemoved(SlippyMapLayer::Ptr layer);
        void layerUpdated(SlippyMapLayer::Ptr layer);
        void layerObjectAdded(SlippyMapLayer::Ptr layer, const SlippyMapLayerObject::Ptr& object);
        void layerObjectRemoved(SlippyMapLayer::Ptr layer, const SlippyMapLayerObject::Ptr& object);
        void layerObjectUpdated(const SlippyMapLayerObject::Ptr& object);
    protected:
        QList<SlippyMapLayer::Ptr> m_layers;
        QList<SlippyMapLayer::Ptr> m_hiddenLayers;
        QList<SlippyMapAnimatedLayer::Ptr> m_animatedLayers;
        SlippyMapLayer::Ptr m_activeLayer = nullptr;
        SlippyMapLayer::Ptr m_defaultLayer = nullptr;
        QList<SlippyMapLayerObject::Ptr> m_visibleObjects;
        QMap<SlippyMapLayerObject::Ptr,SlippyMapLayer::Ptr> m_objectLayerIndex;
        QFont m_hiddenFont;
        QFont m_activeFont;

    protected slots:
        void layer_onObjectAdded(const SlippyMapLayerObject::Ptr& object);
        void layer_onObjectUpdated(const SlippyMapLayerObject::Ptr& object);
    };
}

#endif // SLIPPYMAPLAYERMANAGER_H
