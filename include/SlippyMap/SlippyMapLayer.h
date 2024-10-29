#ifndef SLIPPYMAPLAYER_H
#define SLIPPYMAPLAYER_H

#include <QObject>
#include <QList>

namespace SlippyMap
{
    class SlippyMapLayerObject;

    class SlippyMapLayer : public QObject
    {
        Q_OBJECT
    public:
        explicit SlippyMapLayer(QObject *parent = nullptr);

        void addObject(SlippyMapLayerObject *object);
        void takeObject(SlippyMapLayerObject *object);
        QList<SlippyMapLayerObject*> objects() const;
        QString name() const;
        QString description() const;
        bool isVisible();
        void setName(const QString &name);
        void setDescription(const QString &description);
        void setVisible(const bool visible);
        bool contains(SlippyMapLayerObject *object);
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
        QString m_name;
        QString m_description;
        QList<SlippyMapLayerObject*> m_objects;
    };
}

QDataStream & operator<< (QDataStream& stream, const SlippyMap::SlippyMapLayer* layer);
QDataStream & operator>> (QDataStream& stream, SlippyMap::SlippyMapLayer* layer);

#endif // SLIPPYMAPLAYER_H
