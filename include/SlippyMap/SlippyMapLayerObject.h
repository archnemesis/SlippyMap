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
        Q_PROPERTY(QString label MEMBER m_label NOTIFY labelChanged)
        Q_PROPERTY(QString description MEMBER m_description NOTIFY descriptionChanged)
        Q_PROPERTY(bool visible MEMBER m_visible READ isVisible NOTIFY visibilityChanged)
        Q_PROPERTY(bool movable MEMBER m_movable READ isMovable NOTIFY movabilityChanged)
        Q_PROPERTY(bool active MEMBER m_active READ isActive NOTIFY activeChanged)
    public:
        enum ObjectState {
            NormalState,
            SelectedState,
            DisabledState
        };

        explicit SlippyMapLayerObject(QObject *parent = nullptr);

        /* Virtual Methods ----- */

        /**
         * @brief draw
         * @param painter
         * @param transform
         * @param state
         */
        virtual void draw(QPainter *painter, const QTransform &transform, int zoom) = 0;

        /**
         * @brief Does this object contain points within the given rect (geo coordinates)?
         * @param rect
         * @return
         */
        virtual bool isIntersectedBy(const QRectF &rect) const = 0;

        /**
         * @brief Is the given point within the visible boundaries of the object (screen coordinates)?
         * @param point the point to test for
         * @param zoom the current zoom level
         * @return true if the given point falls within the visible boundaries of the object, false otherwise
         */
        virtual bool contains(const QPointF &point, int zoom) const = 0;

        virtual const QString statusBarText() const = 0;

        /* Property Getters ----- */
        const QString &label() const;
        const QString &description() const;
        bool isVisible() const;
        bool isMovable() const;
        bool isActive() const;

        /* Property Setters ----- */
        void setLabel(const QString& label);
        void setDescription(const QString& description);
        void setVisible(bool visible);
        void setMovable(bool movable);
        void setActive(bool active);

        signals:
        void labelChanged();
        void descriptionChanged();
        void visibilityChanged();
        void movabilityChanged();
        void activeChanged();

    protected:
        QString m_label;
        QString m_description;
        bool m_visible = true;
        bool m_movable = false;
        bool m_active = false;
    };
}

#endif // SLIPPYMAPLAYEROBJECT_H
