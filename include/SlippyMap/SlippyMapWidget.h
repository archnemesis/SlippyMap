#ifndef SLIPPYMAPWIDGET_H
#define SLIPPYMAPWIDGET_H

#include <SlippyMap/SlippyMap.h>

#include <QWidget>
#include <QPoint>
#include <QPointF>
#include <QMap>
#include <QMutex>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QRegularExpression>
#include <QCryptographicHash>
#include <QTimer>

class QNetworkAccessManager;
class QNetworkReply;
class QPaintEvent;
class QPixmap;
class QMouseEvent;
class QWheelEvent;
class QContextMenuEvent;
class QPushButton;
class QLineEdit;
class QMenu;
class QAction;
class QClipboard;

namespace SlippyMap {
    class SlippyMapLayer;
    class SlippyMapLayerObject;
}

namespace SlippyMap
{
    class SlippyMapWidgetLayer;
    class SlippyMapLayerManager;
    class SlippyMapLayerObject;

    class SlippyMapWidget : public QWidget
    {
        Q_OBJECT
        public:
        class LineSet {
        public:
            explicit LineSet(QVector<QPointF> *segments, int width = 1, QColor color = Qt::black) {
                m_segments = segments;
                m_width = width;
                m_color = color;
            }
            QVector<QPointF> *segments() { return m_segments; }
            int width() const { return m_width; }
            const QColor& color() const { return m_color; }
        private:
            QVector<QPointF> *m_segments;
            int m_width;
            QColor m_color;
        };

        enum DrawMode {
            PathDrawing,
            NoDrawing,
            RectDrawing,
            EllipseDrawing,
            PolygonDrawing,
        };

        explicit SlippyMapWidget(QWidget *parent = nullptr);
        ~SlippyMapWidget() override;
        DrawMode drawMode();
        QList<SlippyMapWidgetLayer*> layers();
        QPointF widgetCoordsToGeoCoords(QPoint point);
        QRectF boundingBoxLatLon();
        bool centerOnCursorWhileZooming() const;
        const QString& userAgent() const;
        double degPerPixelX() const;
        double degPerPixelY() const;
        double widgetX2long(qint32 x);
        double widgetY2lat(qint32 y);
        double latitude() const;
        double longitude() const;
        QPointF position() const;
        int zoomLevel() const;
        qint32 lat2widgety(double lat);
        qint32 long2widgetX(double lon);
        static QString geoCoordinatesToString(QPointF coords);
        static QString latLonToString(double lat, double lon);
        void addLayer(SlippyMapWidgetLayer *layer);
        void setCenterOnCursorWhileZooming(bool enable);
        void setDrawMode(DrawMode mode);
        void setLayerManager(SlippyMapLayerManager *manager);
        void setLegendAlignment(const Qt::Alignment& alignment);
        void setLegendSpacing(int legendSpacing);
        void setTileCacheDir(QString dir);
        void setTileCachingEnabled(bool enabled);
        void setDrawingStrokeWidth(int width);
        void setDrawingStrokeColor(const QColor& color);
        void setDrawingFillColor(const QColor& color);
        void setDrawingLineWidth(int width);
        void setUserAgent(const QString& userAgent);
        void takeLayer(SlippyMapWidgetLayer *layer);
        void setActiveObject(SlippyMapLayerObject *object);

    public slots:
        void decreaseZoomLevel();
        void increaseZoomLevel();
        void setCenter(QPointF position);
        void setCenter(double latitude, double longitude);
        void setZoomLevel(int zoom);
        void nextFrame();
        void previousFrame();

    protected slots:
        void remap();
        void layerManagerObjectAdded(SlippyMap::SlippyMapLayer *layer, SlippyMap::SlippyMapLayerObject *object);

    protected:
        void paintEvent(QPaintEvent *event) override;
        bool event(QEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void enterEvent(QEvent *event) override;
        void leaveEvent(QEvent *event) override;
        void wheelEvent(QWheelEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;
        void contextMenuEvent(QContextMenuEvent *event) override;
        void keyPressEvent(QKeyEvent *event) override;
        QPixmap drawLegend(SlippyMapWidgetLayer *layer, int width, int height);
        void initDrawingStyle();

    signals:
        void centerChanged(double latitude, double longitude);
        void dragFinished();
        void zoomLevelChanged(int zoom);
        void tileRequestInitiated();
        void tileRequestFinished();
        void cursorPositionChanged(double latitude, double longitude);
        void cursorLeft();
        void cursorEntered();
        void contextMenuRequested(const QPoint& point);
        void searchTextChanged(const QString &searchText);
        void rectSelected(const QRect &rect);
        void ellipseSelected(const QRect &rect);
        void polygonSelected(const QList<QPointF>& points);
        void pathSelected(const QList<QPointF>& points);
        void drawModeChanged(DrawMode mode);
        void objectActivated(SlippyMapLayerObject *object);
        void objectDeactivated(SlippyMapLayerObject *object);
        void objectDoubleClicked(SlippyMapLayerObject *object);
        void objectWasDragged(SlippyMapLayerObject *object);
        void currentLocationButtonClicked();

    private:
        class Tile {
        public:
            Tile(qint32 x, qint32 y, QPoint point) :
                    m_x(x),
                    m_y(y),
                    m_point(point) {

            }
            Tile(qint32 x, qint32 y, QPoint point, const QPixmap& pixmap) :
                    m_x(x),
                    m_y(y),
                    m_point(point),
                    m_pixmap(pixmap) {
                m_loaded = true;
            }
            QNetworkReply *pendingReply() { return m_pendingReply; }
            QPixmap pixmap() { return m_pixmap; }
            QPoint point() { return m_point; }
            bool isDiscarded() const { return m_discarded; }
            bool isLoaded() const { return m_loaded; }
            qint32 x() const { return m_x; }
            qint32 y() const { return m_y; }
            void discard() { m_discarded = true; }
            void setPendingReply(QNetworkReply *reply) { m_pendingReply = reply; }
            void setPixmap(const QPixmap& pixmap) { m_pixmap = pixmap; m_loaded = true; }
            void setPoint(QPoint point) { m_point = point; }
        private:
            qint32 m_x;
            qint32 m_y;
            bool m_loaded = false;
            bool m_discarded = false;
            QPoint m_point;
            QPixmap m_pixmap;
            QMutex m_pixmapMutex;
            QNetworkReply *m_pendingReply = nullptr;
        };

        qint32 long2tilex(double lon, int z);
        qint32 lat2tiley(double lat, int z);
        double tilex2long(qint32 x, qint32 z);
        double tiley2lat(qint32 y, qint32 z);

        QBrush m_markerBrush;
        QBrush m_markerLabelBrush;
        QBrush m_markerLabelTextBrush;
        QBrush m_scaleBrush;
        QBrush m_scaleTextBrush;
        QClipboard *m_clipboard;
        QFont m_scaleTextFont;
        QList<LineSet*> m_lineSets;
        QList<SlippyMapWidgetLayer*> m_expiredLayers;
        QList<SlippyMapWidgetLayer*> m_layers;
        QMap<LineSet*,QVector<QLineF>> m_lineSetPaths;
        QMap<SlippyMapWidgetLayer*,QList<Tile*>> m_layerTileMaps;
        QMap<SlippyMapWidgetLayer*,QPixmap> m_layerLegends;
        QMutex m_tileMutex;
        QNetworkAccessManager *m_net;
        QPen m_markerLabelPen;
        QPen m_markerLabelTextPen;
        QPen m_markerPen;
        QPen m_scalePen;
        QPen m_scaleTextPen;
        QPixmap m_loadingPixmap;
        QPoint m_contextMenuLocation;
        QPoint m_dragRealStart;
        QPoint m_dragStart;
        QPointF m_drawPolygonEndPosition;
        QRegularExpression m_locationParser;
        QSize m_legendMargins;
        Qt::Alignment m_legendAlignment;
        Qt::MouseButton m_dragButton;
        bool m_dragging = false;
        bool m_dragStarted = false;
        double m_lat;
        double m_lon;
        int m_legendSpacing;
        int m_maxZoom = 18;
        int m_minZoom = 0;
        int m_scaleBarHeight = 10;
        int m_scaleBarMarginBottom = 10;
        int m_scaleBarMarginRight = 10;
        int m_tileX = 0;
        int m_tileY = 0;
        int m_zoomLevel = 0;

        QList<QAction*> m_contextMenuActions;

        /* configurable items */
        bool m_centerOnCursorWhileZooming = true;
        bool m_searchBarVisible = true;
        bool m_zoomButtonsVisible = true;
        bool m_locationButtonVisible = true;
        bool m_zoomSliderVisible = true;
        bool m_cacheTiles = false;
        QString m_cacheTileDir;
        QString m_userAgentString;

        /* drawing */
        DrawMode m_drawMode = NoDrawing;
        QColor m_drawingStrokeColor;
        QColor m_drawingFillColor;
        QList<QPointF> m_drawPolygonPoints;
        QPoint m_drawModeRect_topLeft;
        QPoint m_drawModeRect_bottomRight;
        QBrush m_drawBrush;
        QPen m_drawPen;
        QPen m_linePen;
        QPen m_lineStrokePen;
        SlippyMapLayerManager *m_layerManager = nullptr;
        SlippyMapLayerObject *m_activeObject = nullptr;
        SlippyMapLayerObject *m_dragObject = nullptr;
        int m_drawingStrokeWidth = 2;
        int m_drawingLineWidth = 10;
    };

}

#endif // SLIPPYMAPWIDGET_H
