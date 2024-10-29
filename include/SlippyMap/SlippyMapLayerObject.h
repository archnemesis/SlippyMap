//
// Created by robin on 9/23/24.
//

#ifndef SLIPPYMAPLAYEROBJECT_H
#define SLIPPYMAPLAYEROBJECT_H

#include <QObject>
#include <QPainter>
#include <QBrush>

namespace SlippyMap
{
    class SlippyMapLayerObject : public QObject
    {
        Q_OBJECT
    public:
        enum ObjectState {
            NormalState,
            SelectedState,
            DisabledState
        };

        explicit SlippyMapLayerObject(QObject *parent = nullptr);
        virtual void draw(QPainter *painter, const QTransform &transform, ObjectState state = NormalState) = 0;
        virtual void setMovable(bool movable);
        void setActive(bool active);
        void setBrush(QBrush brush);
        void setPen(QPen pen);
        virtual bool contains(const QPointF& point, int zoom) const = 0;
        virtual bool isIntersectedBy(const QRectF& rect) const = 0;
        virtual bool isMovable();
        virtual const QPointF position() const = 0;
        virtual const QSizeF size() const = 0;
        virtual QString statusBarText();
        QString label();
        QString description();
        void setLabel(const QString& name);
        void setDescription(const QString& description);
        void setVisible(bool visible);
        bool isVisible();
        bool isActive();

    signals:
        void somethingHappened();
        void activeChanged();
        void labelChanged();
        void visibilityChanged();

    protected:
        QString m_label;
        QString m_description;
        QBrush m_brush;
        QBrush m_activeBrush;
        QBrush m_selectedBrush;
        QBrush m_selectionHandleBrush;
        QPen m_pen;
        QPen m_activePen;
        QPen m_selectedPen;
        QPen m_selectionHandlePen;
        int m_resizeHandleWidth = 6;
        bool m_active = false;
        bool m_visible = true;
        bool m_movable = true;
        void drawResizeHandle(QPainter *painter, QPoint point) const;
    };
}

#endif // SLIPPYMAPLAYEROBJECT_H
