#ifndef SLIPPYMAPWIDGETLAYER_H
#define SLIPPYMAPWIDGETLAYER_H

#include <QObject>
#include <QPixmap>

namespace SlippyMap
{
    class SlippyMapWidgetLayer : public QObject
    {
        Q_OBJECT
    public:
        enum LegendStyle {
            None,
            Discrete,
            Gradient
        };

        explicit SlippyMapWidgetLayer(QObject *parent = nullptr);
        explicit SlippyMapWidgetLayer(const QString& tileUrl, QObject *parent = nullptr);
        void setTileUrl(const QString& tileUrl);
        void setName(const QString& name);
        void setDescription(const QString& description);
        void setZOrder(int zOrder);
        void setVisible(bool visible);
        void setBaseLayer(bool baseLayer);
        void setLegendStyle(LegendStyle style);
        void addLegendColor(const QColor& color, const QString& label);
        void setCacheDuration(int duration);
        virtual void drawOverlay(QPainter& painter, const QRect& rect) {}

        virtual const QString& name();
        virtual const QString& description();
        virtual const QString& tileUrl();
        virtual QString tileUrlHash();
        int zOrder();
        int cacheDuration();
        virtual bool isVisible();
        bool isBaseLayer();
        QPixmap legendPixmap(int width, int height);
        const QList<QColor>& legendColors();
        const QList<QString>& legendLabels();

    private:
        QString m_name;
        QString m_description;
        QString m_tileUrl;
        int m_zOrder;
        int m_cacheDuration;
        bool m_visible;
        bool m_baseLayer;
        QList<QColor> m_legendColors;
        QList<QString> m_legendLabels;
        QPixmap m_legendPixmap;

        signals:
            void zOrderChanged(int zOrder);
        void visibilityChanged(bool visible);
        void nameChanged(const QString& name);
        void descriptionChanged(const QString& description);
        void updated();
    };
}

#endif // SLIPPYMAPWIDGETLAYER_H
