#ifndef SLIPPYMAPLAYER_H
#define SLIPPYMAPLAYER_H

#include <SlippyMap/SlippyMap.h>
#include <SlippyMap/SlippyMapLayerObject.h>

#include <QObject>
#include <QList>
#include <QVariant>

namespace SlippyMap
{
    class SLIPPYMAPSHARED_EXPORT SlippyMapLayer : public QObject
    {
        Q_OBJECT
    public:
        typedef QSharedPointer<SlippyMapLayer> Ptr;

        explicit SlippyMapLayer(QObject *parent = nullptr);
        ~SlippyMapLayer();
        void addObject(SlippyMapLayerObject::Ptr object);
        void takeObject(SlippyMapLayerObject::Ptr object);
        QList<SlippyMapLayerObject::Ptr> objects() const;
        QVariant id() const;
        QString name() const;
        QString description() const;
        int order() const;
        bool isVisible();
        bool isEditable();
        bool isSynced();
        void setId(const QVariant& id);
        void setName(const QString &name);
        void setDescription(const QString &description);
        void setVisible(const bool visible);
        void setEditable(const bool editable);
        void setSynced(bool synced);
        void setOrder(int order);
        bool contains(SlippyMapLayerObject::Ptr object);
        void replace(SlippyMapLayerObject::Ptr object, SlippyMapLayerObject::Ptr replacement);
        int indexOf(SlippyMapLayerObject::Ptr object);
        void deactivateAll();
        void removeAll();
        void showAll();
        void hideAll();

    signals:
        void objectAdded(SlippyMapLayerObject::Ptr object);
        void objectRemoved(SlippyMapLayerObject::Ptr object);
        void objectUpdated(SlippyMapLayerObject::Ptr object);

    protected slots:
        void objectChanged();

    protected:
        int m_zOrder = 0;
        bool m_visible = true;
        bool m_editable = true;
        bool m_synced = false;
        QVariant m_id;
        QString m_name;
        QString m_description;
        QList<SlippyMapLayerObject::Ptr> m_objects;
    };
}

QDataStream & operator<< (QDataStream& stream, const SlippyMap::SlippyMapLayer* layer);
QDataStream & operator>> (QDataStream& stream, SlippyMap::SlippyMapLayer* layer);

#endif // SLIPPYMAPLAYER_H
