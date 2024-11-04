#ifndef SLIPPYMAPLAYER_H
#define SLIPPYMAPLAYER_H

#include <SlippyMap/SlippyMap.h>

#include <QObject>
#include <QList>
#include <QVariant>

namespace SlippyMap
{
    class SlippyMapLayerObject;

    class SLIPPYMAPSHARED_EXPORT SlippyMapLayer : public QObject
    {
        Q_OBJECT
    public:
        explicit SlippyMapLayer(QObject *parent = nullptr);
        void addObject(SlippyMapLayerObject *object);
        void takeObject(SlippyMapLayerObject *object);
        QList<SlippyMapLayerObject*> objects() const;
        QVariant id() const;
        QString name() const;
        QString description() const;
        bool isVisible();
        bool isEditable();
        bool isSynced();
        void setId(const QVariant& id);
        void setName(const QString &name);
        void setDescription(const QString &description);
        void setVisible(const bool visible);
        void setEditable(const bool editable);
        void setSynced(bool synced);
        bool contains(SlippyMapLayerObject *object);
        void replace(SlippyMapLayerObject *object, SlippyMapLayerObject *replacement);
        int indexOf(SlippyMapLayerObject *object);
        void deactivateAll();
        void removeAll();
        void showAll();
        void hideAll();

    signals:
        void objectAdded(SlippyMapLayerObject *object);
        void objectRemoved(SlippyMapLayerObject *object);
        void objectUpdated(SlippyMapLayerObject *object);

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
        QList<SlippyMapLayerObject*> m_objects;
    };
}

QDataStream & operator<< (QDataStream& stream, const SlippyMap::SlippyMapLayer* layer);
QDataStream & operator>> (QDataStream& stream, SlippyMap::SlippyMapLayer* layer);

#endif // SLIPPYMAPLAYER_H
