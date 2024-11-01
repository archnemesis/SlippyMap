//
// Created by robin on 10/31/2024.
//

#ifndef SLIPPYMAPLAYERELLIPSE_H
#define SLIPPYMAPLAYERELLIPSE_H

#include "SlippyMap/SlippyMapLayerObject.h"


namespace SlippyMap {
    class SlippyMapLayerEllipse : public SlippyMapLayerObject {
        Q_OBJECT
    public:
        explicit SlippyMapLayerEllipse(double rx, double ry, QObject *parent = nullptr);
        void draw(QPainter *painter, const QTransform& transform, ObjectState state) override;
        double rx() const;
        double ry() const;
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
        void setStrokeColor(const QColor& color);
        void setStrokeWidth(int width);
        void setFillColor(const QColor& color);

    };
}

#endif //SLIPPYMAPLAYERELLIPSE_H
