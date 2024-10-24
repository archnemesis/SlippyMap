#ifndef SLIPPYMAPLAYERMARKER_H
#define SLIPPYMAPLAYERMARKER_H

#include "SlippyMapLayerObject.h"

#include <QPixmap>
#include <QImage>
#include <QPointF>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QVariant>

namespace SlippyMap
{
    class SlippyMapLayerMarker : public SlippyMapLayerObject
    {
        Q_OBJECT
            Q_PROPERTY(QPointF position MEMBER m_position READ position WRITE setPosition NOTIFY positionChanged)
            Q_PROPERTY(QColor color MEMBER m_color)
            Q_PROPERTY(int radius MEMBER m_radius)
            Q_PROPERTY(QImage icon MEMBER m_icon)
            Q_PROPERTY(bool editable MEMBER m_editable READ isEditable)

        public:
        SlippyMapLayerMarker(QObject *parent = nullptr);
        SlippyMapLayerMarker(const QPointF &position, QObject *parent = nullptr);

        void draw(QPainter *painter, const QTransform &transform, int zoom);
        bool contains(const QPointF &point, int zoom) const;
        bool isIntersectedBy(const QRectF &rect) const;
        const QString statusBarText() const;

        void setPosition(const QPointF& position);
        void setColor(const QColor& color);
        void setRadius(int radius);
        void setEditable(bool editable);
        void setIcon(const QImage& icon);

        const QPointF& position() const;
        bool isEditable() const;
        const QColor& color() const;
        const QImage& icon() const;
        int radius() const;


        signals:
            void positionChanged();
        void colorChanged();
        void radiusChanged();
        void editableChanged();
        void iconChanged();

    protected:
        void initStyle();

        int m_radius;
        QHash<QString,QVariant> m_metadata;
        QPointF m_position;
        QString m_information;
        QColor m_color;
        QBrush m_activeDotBrush;
        QBrush m_dotBrush;
        QBrush m_labelBrush;
        QBrush m_labelTextBrush;
        QPen m_activeDotPen;
        QPen m_dotPen;
        QPen m_labelPen;
        QPen m_labelTextPen;
        QPixmap m_pixmap;
        QImage m_icon;
        bool m_active = false;
        bool m_editable = true;
    };
}

// QDataStream &operator<<(QDataStream& stream, const SlippyMap::SlippyMapLayerMarker& marker);
// QDataStream &operator>>(QDataStream& stream, SlippyMap::SlippyMapLayerMarker& marker);

#endif // SLIPPYMAPLAYERMARKER_H
