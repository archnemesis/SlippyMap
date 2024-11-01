//
// Created by robin on 9/23/24.
//

#ifndef SLIPPYMAPLAYEROBJECT_H
#define SLIPPYMAPLAYEROBJECT_H

#include <SlippyMap/SlippyMap.h>
#include <QPainter>
#include <QBrush>

class SlippyMapLayerObjectPropertyPage;

namespace SlippyMap
{
    class SLIPPYMAPSHARED_EXPORT SlippyMapLayerObject : public QObject
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
        virtual QList<SlippyMapLayerObjectPropertyPage*> propertyPages() const = 0;
        virtual QDataStream& serialize(QDataStream& stream) const = 0;
        virtual void unserialize(QDataStream& stream) = 0;
        virtual bool contains(const QPointF& point, int zoom) const = 0;
        virtual bool isIntersectedBy(const QRectF& rect) const = 0;
        virtual bool isMovable();
        virtual bool isEditable();
        virtual const QPointF position() const = 0;
        virtual void setPosition(const QPointF& position) = 0;
        virtual const QSizeF size() const = 0;
        virtual QString statusBarText();
        QString label() const;
        QString description() const;
        void setLabel(const QString& name);
        void setDescription(const QString& description);
        void setVisible(bool visible);
        void setEditable(bool editable);
        bool isVisible();
        bool isActive();

        /*
         * Drawing/Editing Support Methods
         */

        /**
         * Used to determine if the object has resize
         * handles that need to be shown when selected.
         * @return true if the object has resize handles
         */
        bool hasResizeHandles() const { return false; }

        /**
         * Returns the bounding rectangles for the objects
         * resize handles, used for hit detection.
         * @return
         */
        QList<QRectF> resizeHandles() const { return {}; }

        /**
         * Return the cursor shape for when the mouse is over
         * a particular area.
         * @param point
         * @return
         */
        Qt::CursorShape cursorShape(const QPointF& point) const { return Qt::PointingHandCursor; }

        /**
         * Return the cursor shape for when the mouse is over
         * a particular area and it is active.
         * @param point
         * @return
         */
        Qt::CursorShape activeCursorShape(const QPointF& point) const { return Qt::DragMoveCursor; }


    signals:
        void updated();
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
        bool m_editable = true;
        void drawResizeHandle(QPainter *painter, QPoint point) const;
    };
}

#endif // SLIPPYMAPLAYEROBJECT_H
