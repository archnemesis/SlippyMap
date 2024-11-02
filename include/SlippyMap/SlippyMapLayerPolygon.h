#ifndef SLIPPYMAPWIDGETPOLYGON_H
#define SLIPPYMAPWIDGETPOLYGON_H

#include <SlippyMap/SlippyMap.h>
#include <SlippyMap/SlippyMapLayerObject.h>

#include <QObject>
#include <QVector>
#include <QBrush>
#include <QPainter>


namespace SlippyMap {
    class SLIPPYMAPSHARED_EXPORT SlippyMapLayerPolygon : public SlippyMapLayerObject
    {
        Q_OBJECT
    public:
        explicit Q_INVOKABLE SlippyMapLayerPolygon(QObject *parent = nullptr);
        explicit Q_INVOKABLE SlippyMapLayerPolygon(QVector<QPointF> points, QObject *parent = nullptr);
        ~SlippyMapLayerPolygon() override;
        void draw(QPainter *painter, const QTransform &transform, ObjectState state) override;
        QVector<QPointF> points() const;
        void setPoints(const QVector<QPointF> &points);
        QDataStream& serialize(QDataStream& stream) const override;
        QList<SlippyMapLayerObjectPropertyPage*> propertyPages() const;
        bool contains(const QPointF& point, int zoom) const override;
        bool isIntersectedBy(const QRectF& rect) const override;
        const QPointF position() const override;
        const QSizeF size() const override;
        const QColor& strokeColor() const;
        const QColor& fillColor() const;
        int strokeWidth() const;
        void setPosition(const QPointF& position) override;
        void unserialize(QDataStream& stream) override;
        QList<QRectF> resizeHandles(const QTransform& transform) const override;
        void setStrokeColor(const QColor& color);
        void setStrokeWidth(int width);
        void setFillColor(const QColor& color);

    protected:
        QVector<QPointF> m_points;
        QColor m_fillColor;
        QColor m_strokeColor;
        int m_strokeWidth;
        QPen m_strokePen;
        QBrush m_fillBrush;

        void initStyle();
        bool test_point(const QPointF& a, const QPointF& b, const QPointF& p) const;
    };
}

#endif // SLIPPYMAPWIDGETPOLYGON_H
