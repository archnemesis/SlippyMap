#ifndef SLIPPYMAPLAYERMARKER_H
#define SLIPPYMAPLAYERMARKER_H

#include <SlippyMap/SlippyMap.h>
#include <SlippyMap/SlippyMapLayerObject.h>

#include <QPixmap>
#include <QImage>
#include <QPointF>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QSizeF>
#include <QVariant>
#include <QDataStream>

namespace SlippyMap
{
    class SLIPPYMAPSHARED_EXPORT SlippyMapWidgetMarker : public SlippyMapLayerObject
    {
        Q_OBJECT
        Q_PROPERTY(QString label MEMBER m_label READ label WRITE setLabel)
        Q_PROPERTY(QString description MEMBER m_description READ description WRITE setDescription)
        Q_PROPERTY(bool visible MEMBER m_visible READ isVisible WRITE setVisible)
        Q_PROPERTY(QPointF position MEMBER m_position READ position WRITE setPosition NOTIFY positionChanged)
        Q_PROPERTY(QColor color MEMBER m_color READ color WRITE setColor)
        Q_PROPERTY(int radius MEMBER m_radius READ radius WRITE setRadius)
        Q_PROPERTY(QImage icon MEMBER m_icon READ icon WRITE setIcon)
        Q_PROPERTY(bool editable MEMBER m_editable READ isEditable)

    public:
        explicit Q_INVOKABLE SlippyMapWidgetMarker(QObject *parent = nullptr);
        explicit SlippyMapWidgetMarker(const QPointF &position, QObject *parent = nullptr);
        Q_INVOKABLE SlippyMapWidgetMarker(const SlippyMap::SlippyMapWidgetMarker& other);
        virtual SlippyMapLayerObject* clone() const;
        virtual void copy(SlippyMapLayerObject* other);
        void draw(QPainter *painter, const QTransform &transform, ObjectState state) override;
        bool contains(const QPointF &point, int zoom) const;
        const QSizeF size() const override;
        bool isIntersectedBy(const QRectF &rect) const;
        const QString statusBarText() const;

        void setPosition(const QPointF& position) override;
        void setColor(const QColor& color);
        void setRadius(int radius);
        void setIcon(const QImage& icon);

        const QPointF position() const;
        const QColor& color() const;
        const QImage& icon() const;
        int radius() const;
        QList<SlippyMapLayerObjectPropertyPage*> propertyPages() const;
        QDataStream& serialize(QDataStream& stream) const override;
        void unserialize(QDataStream& stream) override;

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

QDataStream &operator<<(QDataStream& stream, const SlippyMap::SlippyMapWidgetMarker& marker);
QDataStream &operator>>(QDataStream& stream, SlippyMap::SlippyMapWidgetMarker& marker);

#endif // SLIPPYMAPLAYERMARKER_H
