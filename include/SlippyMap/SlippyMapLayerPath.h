//
// Created by Robin on 11/1/2024.
//

#ifndef OSMEXPLORER_SLIPPYMAPLAYERPATH_H
#define OSMEXPLORER_SLIPPYMAPLAYERPATH_H

#include "SlippyMapLayerObject.h"


namespace SlippyMap
{
    class SlippyMapLayerPath : public SlippyMapLayerObject
    {
        Q_OBJECT
    public:
        typedef QSharedPointer<SlippyMapLayerPath> Ptr;

        explicit Q_INVOKABLE SlippyMapLayerPath(QObject *parent = nullptr);
        explicit SlippyMapLayerPath(const QVector<QPointF> &points);
        Q_INVOKABLE SlippyMapLayerPath(const SlippyMap::SlippyMapLayerPath& other);

        virtual SlippyMapLayerPath* clone() const;
        virtual void copy(SlippyMapLayerObject *other);
        void hydrateFromDatabase(const QJsonObject& json, const QString& geometry) override;
        void saveToDatabase(QJsonObject& json, QString& geometry) override;
        QList<SlippyMapLayerObjectPropertyPage *> propertyPages(SlippyMapLayerObject::Ptr object) const;
        bool contains(const QPointF &point, int zoom) const override;
        bool isIntersectedBy(const QRectF &rect) const override;
        const QPointF position() const override;
        const QSizeF size() const override;
        void draw(QPainter *painter, const QTransform &transform, ObjectState state) override;
        void setPosition(const QPointF &position) override;
        QDataStream &serialize(QDataStream &stream) const override;
        void unserialize(QDataStream &stream) override;
        void setPoints(const QVector<QPointF>& points);
        void setLineWidth(int width);
        void setLineColor(const QColor &color);
        void setStrokeWidth(int width);
        void setStrokeColor(const QColor &color);
        const QVector<QPointF>& points() const;
        int lineWidth() const;
        const QColor &lineColor() const;
        int strokeWidth() const;
        const QColor &strokeColor() const;

    protected:
        void initStyle();

        QPen m_linePen;
        QPen m_linePenSelected;
        QPen m_strokePen;
        QPen m_strokePenSelected;
        QBrush m_strokeBrush;
        QColor m_lineColor;
        QColor m_strokeColor;
        int m_lineWidth;
        int m_strokeWidth;
        QVector<QPointF> m_points;
    };
}

#endif //OSMEXPLORER_SLIPPYMAPLAYERPATH_H
