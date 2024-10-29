#include "SlippyMap/SlippyMapWidget.h"
#include "SlippyMap/SlippyMapWidgetLayer.h"
#include "SlippyMap/SlippyMapLayer.h"
#include "SlippyMap/SlippyMapLayerManager.h"
#include "SlippyMap/SlippyMapLayerObject.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <cmath>

#include <QDebug>
#include <QGuiApplication>
#include <QPalette>

#include <QPainter>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QUrl>
#include <QDir>
#include <QRegularExpressionMatch>
#include <QMenu>
#include <QAction>
#include <QCompleter>
#include <QMessageBox>
#include <QPixmap>
#include <QPoint>
#include <QPushButton>
#include <QLineEdit>
#include <QPainterPath>
#include <QFontMetrics>
#include <QClipboard>
#include <QApplication>
#include <QCryptographicHash>
#include <QListWidget>
#include <QCursor>
#include <QTransform>
#include <QVBoxLayout>
#include <QGesture>

using namespace SlippyMap;

#ifndef DEFAULT_LATITUDE
#define DEFAULT_LATITUDE 40.0
#endif

#ifndef DEFAULT_LONGITUDE
#define DEFAULT_LONGITUDE 100.0
#endif

#ifndef DEFAULT_ZOOM
#define DEFAULT_ZOOM 5
#endif

SlippyMapWidget::SlippyMapWidget(QWidget *parent)
    : QWidget(parent)
    , m_legendAlignment(Qt::AlignLeft | Qt::AlignBottom)
    , m_legendMargins(10, 10)
    , m_legendSpacing(5)
    , m_userAgentString("SlippyMap/1.0.0")
    , m_dragButton(Qt::NoButton)
{
    setMouseTracking(true);

    m_net = new QNetworkAccessManager(this);
    m_zoomLevel = DEFAULT_ZOOM;
    m_clipboard = QApplication::clipboard();
    m_lat = DEFAULT_LATITUDE;
    m_lon = DEFAULT_LONGITUDE;

    m_zoomInButton = new QPushButton(this);
    m_zoomInButton->setText("");
    m_zoomInButton->setIcon(QIcon(":/icons/resources/light/plus-64.png"));
    m_zoomInButton->setIconSize(QSize(64,64));

    m_zoomOutButton = new QPushButton(this);
    m_zoomOutButton->setText("");
    m_zoomOutButton->setIcon(QIcon(":/icons/resources/light/minus-64.png"));
    m_zoomOutButton->setIconSize(QSize(64,64));

    m_currentLocationButton = new QPushButton(this);
    m_currentLocationButton->setText("");
    m_currentLocationButton->setIcon(QIcon(":/icons/resources/light/location-64.png"));
    m_currentLocationButton->setIconSize(QSize(64, 64));

    auto *vLayout = new QVBoxLayout();
    vLayout->addWidget(m_zoomInButton);
    vLayout->addWidget(m_zoomOutButton);
    vLayout->addWidget(m_currentLocationButton);
    vLayout->addStretch();

    auto *hLayout = new QHBoxLayout();
    hLayout->addLayout(vLayout);
    hLayout->addStretch();

    connect(m_zoomInButton,
            &QPushButton::pressed,
            this,
            &SlippyMapWidget::increaseZoomLevel);
    connect(m_zoomOutButton,
            &QPushButton::pressed,
            this,
            &SlippyMapWidget::decreaseZoomLevel);
    connect(m_currentLocationButton,
            &QPushButton::clicked,
            [this]() {
                emit currentLocationButtonClicked();
    });

    m_scaleBrush.setStyle(Qt::SolidPattern);
    m_scaleBrush.setColor(QColor(0,0,0,128));
    m_scalePen.setStyle(Qt::NoPen);

    m_scaleTextBrush.setStyle(Qt::SolidPattern);
    m_scaleTextBrush.setColor(Qt::black);
    m_scaleTextPen.setStyle(Qt::SolidLine);
    m_scaleTextPen.setColor(Qt::white);
    m_scaleTextPen.setWidth(1);
    m_scaleTextFont.setPixelSize(12);
    m_scaleTextFont.setBold(true);

    m_drawBrush.setStyle(Qt::SolidPattern);
    m_drawBrush.setColor(QColor(255, 255, 255, 128));
    m_drawPen.setStyle(Qt::SolidLine);
    m_drawPen.setColor(Qt::white);

    setLayout(hLayout);
}

SlippyMapWidget::~SlippyMapWidget()
{
    m_activeObject = nullptr;
    m_dragObject = nullptr;
    m_layerManager = nullptr;
    m_layers.clear();
    m_expiredLayers.clear();

    for (auto key : m_layerTileMaps.keys()) {
        while (!m_layerTileMaps[key].empty()) {
            Tile *tile = m_layerTileMaps[key].takeFirst();
            if (tile->pendingReply() != nullptr && tile->pendingReply()->isRunning()) {
                tile->discard();
                tile->pendingReply()->abort();
                tile->pendingReply()->deleteLater();
            }
            delete tile;
        }
    }
}

QString SlippyMapWidget::latLonToString(double lat, double lon)
{
    char dir_lat;
    char dir_lon;

    if (lat > 0) {
        dir_lat = 'N';
    }
    else {
        dir_lat = 'S';
    }

    if (lon > 0) {
        dir_lon = 'E';
    }
    else {
        dir_lon = 'W';
    }

    QString ret = QString("%1 %2 %3 %4")
            .arg(fabs(lat), 8, 'f', 4, '0')
            .arg(dir_lat)
            .arg(fabs(lon), 8, 'f', 4, '0')
            .arg(dir_lon);

    return ret;
}

QString SlippyMapWidget::geoCoordinatesToString(QPointF coords)
{
    return latLonToString(coords.y(), coords.x());
}

void SlippyMapWidget::addLayer(SlippyMapWidgetLayer *layer)
{
    m_layers.append(layer);
    m_layerTileMaps[layer] = QList<Tile*>();

    connect(
            layer,
            &SlippyMapWidgetLayer::updated,
            [this, layer]() {
                if (!m_expiredLayers.contains(layer))
                    m_expiredLayers.append(layer);
                remap();
            });

    remap();
}

QList<SlippyMapWidgetLayer *> SlippyMapWidget::layers()
{
    return m_layers;
}

void SlippyMapWidget::takeLayer(SlippyMapWidgetLayer *layer)
{
    m_layers.removeOne(layer);
    remap();
}

void SlippyMapWidget::setCenterOnCursorWhileZooming(bool enable)
{
    m_centerOnCursorWhileZooming = enable;
}

bool SlippyMapWidget::centerOnCursorWhileZooming() const
{
    return m_centerOnCursorWhileZooming;
}

void SlippyMapWidget::setZoomButtonsVisible(bool visible)
{
    m_zoomButtonsVisible = visible;
    m_zoomInButton->setVisible(false);
    m_zoomOutButton->setVisible(false);
    update();
}

void SlippyMapWidget::setLocationButtonVisible(bool visible)
{
    m_locationButtonVisible = visible;
    m_currentLocationButton->setVisible(visible);
    update();
}

void SlippyMapWidget::setZoomSliderVisible(bool visible)
{
    m_zoomSliderVisible = visible;
    update();
}

void SlippyMapWidget::setTileCachingEnabled(bool enabled)
{
    m_cacheTiles = enabled;
}

void SlippyMapWidget::setTileCacheDir(QString dir)
{
    m_cacheTileDir = dir;
    QDir cacheDir(dir);
    cacheDir.mkpath(".");
}

void SlippyMapWidget::setLayerManager(SlippyMapLayerManager *manager)
{
    m_layerManager = manager;
    update();
}

void SlippyMapWidget::setCenter(double latitude, double longitude)
{
    m_lat = latitude;
    m_lon = longitude;

    remap();
}

void SlippyMapWidget::setCenter(QPointF position)
{
    m_lat = position.y();
    m_lon = position.x();
    remap();
}

void SlippyMapWidget::setZoomLevel(int zoom)
{
    if (zoom >= m_minZoom && zoom <= m_maxZoom) {
        m_zoomLevel = zoom;
        remap();
    }
}

void SlippyMapWidget::increaseZoomLevel()
{
    if (m_zoomLevel < m_maxZoom) {
        m_zoomLevel++;
        m_expiredLayers = m_layers;
        remap();
    }
}

void SlippyMapWidget::decreaseZoomLevel()
{
    if (m_zoomLevel > m_minZoom) {
        m_zoomLevel--;
        m_expiredLayers = m_layers;
        remap();
    }
}

void SlippyMapWidget::setTextLocation(const QString& location)
{
    QRegularExpressionMatch match = m_locationParser.match(location);
    if (!match.hasMatch()) {
        QMessageBox::critical(
                this,
                tr("Location Error"),
                tr("Unable to parse location. Please try again."));
        return;
    }

    QString match_lat = match.captured(1);
    QString match_lat_card = match.captured(2);
    QString match_lon = match.captured(3);
    QString match_lon_card = match.captured(4);

    double lat;
    double lon;

    if (match_lat.length() > 0) {
        bool ok;
        lat = match_lat.toDouble(&ok);

        if (!ok) {
            goto parseError;
        }

        if (match_lat_card.length() == 1) {
            if (match_lat_card == "N") {
                lat = fabs(lat);
            }
            else {
                lat = -1 * fabs(lat);
            }
        }
    }
    else {
        goto parseError;
    }

    if (match_lon.length() > 0) {
        bool ok;
        lon = match_lon.toDouble(&ok);

        if (!ok) {
            goto parseError;
        }

        if (match_lon_card.length() == 1) {
            if (match_lon_card == "E") {
                lon = fabs(lon);
            }
            else {
                lon = -1 * fabs(lon);
            }
        }
    }
    else {
        goto parseError;
    }

    setCenter(lat, lon);

    return;

    parseError:
    QMessageBox::critical(
            this,
            tr("Location Error"),
            tr("Unable to parse location. Please try again."));
}

void SlippyMapWidget::centerMapActionTriggered()
{
    double lat = widgetY2lat(m_contextMenuLocation.y());
    double lon = widgetX2long(m_contextMenuLocation.x());
    setCenter(lat, lon);
}

void SlippyMapWidget::zoomInHereActionTriggered()
{
    double lat = widgetY2lat(m_contextMenuLocation.y());
    double lon = widgetX2long(m_contextMenuLocation.x());
    setCenter(lat, lon);
    increaseZoomLevel();
}

void SlippyMapWidget::zoomOutHereActionTriggered()
{
    double lat = widgetY2lat(m_contextMenuLocation.y());
    double lon = widgetX2long(m_contextMenuLocation.x());
    setCenter(lat, lon);
    decreaseZoomLevel();
}

void SlippyMapWidget::copyCoordinatesActionTriggered()
{
    double lat = widgetY2lat(m_contextMenuLocation.y());
    double lon = widgetX2long(m_contextMenuLocation.x());
    QString str = latLonToString(lat, lon);
    m_clipboard->setText(str);
}

void SlippyMapWidget::copyLatitudeActionTriggered()
{
    double lat = widgetY2lat(m_contextMenuLocation.y());
    QString str = QString("%1").arg(lat, 8, 'f', 4, '0');
    m_clipboard->setText(str);
}

void SlippyMapWidget::copyLongitudeActionTriggered()
{
    double lon = widgetX2long(m_contextMenuLocation.x());
    QString str = QString("%1").arg(lon, 8, 'f', 4, '0');
    m_clipboard->setText(str);
}

void SlippyMapWidget::onMarkerChanged()
{
    update();
}

void SlippyMapWidget::paintEvent(QPaintEvent *event)
{
    (void)event;

    QPainter painter(this);
    painter.setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);

    for (SlippyMapWidgetLayer *layer : m_layers) {
        for (int i = 0; i < m_layerTileMaps[layer].size(); i++) {
            Tile *t = m_layerTileMaps[layer].at(i);
            if (t->isLoaded()) {
                painter.drawPixmap(t->point(), t->pixmap());
            }
        }
    }

    /* ----- Scale Bar ----- */

    double C = 40075016.686;
    double S = C * cos(m_lat * (M_PI/180.0)) / pow(2.0, m_zoomLevel + 8);
    double len = S * 100;

    if (len < 100.0) {
        len = floor((len + 5) / 10) * 10;
    }
    else if (len < 1000.0) {
        len = floor((len + 50) / 100) * 100;
    }
    else if (len < 10000.0) {
        len = floor((len + 500) / 1000) * 1000;
    }
    else if (len < 100000.0) {
        len = floor((len + 5000) / 10000) * 10000;
    }
    else if (len < 1000000.0) {
        len = floor((len + 50000) / 100000) * 100000;
    }

    qint32 pixlen = static_cast<qint32>(len / S);
    int left = width() - m_scaleBarMarginRight - pixlen;
    int top = height() - m_scaleBarMarginBottom - m_scaleBarHeight;

    painter.setPen(m_scalePen);
    painter.setBrush(m_scaleBrush);
    painter.drawRect(left, top, pixlen, m_scaleBarHeight);

    /* ----- Search Bar ----- */

    QPainterPath textPath;
    textPath.addText(QPoint(left, (top - 10)), m_scaleTextFont, tr("%1 m").arg(len));

    painter.setBrush(m_scaleTextBrush);
    painter.setPen(m_scaleTextPen);
    painter.drawPath(textPath);

    /* ----- Draw Layers and Objects ----- */

    QRectF bbox = boundingBoxLatLon();

    QTransform m1;
    QTransform m2;
    m1.translate(-bbox.x(), -bbox.y());
    m2.scale(1.0/degPerPixelX(), -(1.0/degPerPixelY()));
    QTransform m3(m1 * m2);

    if (m_layerManager != nullptr) {
        for (SlippyMapLayer *layer : m_layerManager->layers()) {
            if (layer->isVisible()) {
                for (SlippyMapLayerObject *obj : layer->objects()) {
                    if (obj->isVisible() && !obj->isActive()) {
                        if (obj->isIntersectedBy(bbox)) {
                            obj->draw(&painter, m3, SlippyMapLayerObject::NormalState);
                        }
                    }
                }

                // draw active things on top
                for (SlippyMapLayerObject *obj : layer->objects()) {
                    if (obj->isVisible() && obj->isActive()) {
                        if (obj->isIntersectedBy(bbox)) {
                            obj->draw(&painter, m3, SlippyMapLayerObject::SelectedState);
                        }
                    }
                }
            }
        }
    }

    /* ----- Dragging and Drawing ----- */

    if (m_dragging && m_drawMode != NoDrawing) {
        switch (m_drawMode) {
            case RectDrawing:
                painter.setBrush(m_drawBrush);
                painter.setPen(m_drawPen);
                painter.drawRect(QRect(m_drawModeRect_topLeft, m_drawModeRect_bottomRight));
                break;
            case EllipseDrawing:
                painter.setBrush(m_drawBrush);
                painter.setPen(m_drawPen);
                painter.drawEllipse(QRect(m_drawModeRect_topLeft, m_drawModeRect_bottomRight));
                break;
            default:
                break;
        }
    }

    /* ----- Layer Legends ----- */

    const int margin_x = m_legendMargins.width();
    const int margin_y = m_legendMargins.height();
    int x_start = 0;

    for (auto *layer: m_layers) {
        if (layer->isVisible()) {
            if (!layer->legendColors().empty()) {
                QPixmap pixmap;

                if (m_layerLegends.contains(layer))
                    pixmap = m_layerLegends[layer];
                else {
                    pixmap = drawLegend(layer, 20, 200);
                    m_layerLegends[layer] = pixmap;
                }

                int x = 0;
                int y = 0;
                int w = pixmap.width();
                int h = pixmap.height();

                // center not supported
                if (m_legendAlignment & Qt::AlignRight)
                    x = rect().width() - margin_x - w - x_start;
                else
                    x = margin_x + x_start;

                // center not supported
                if (m_legendAlignment & Qt::AlignTop)
                    y = margin_y;
                else
                    y = rect().height() - margin_y - h;

                painter.drawPixmap(x, y, pixmap);
                x_start += pixmap.width() + m_legendSpacing;
            }

            layer->drawOverlay(painter, rect());
        }
    }
}

bool SlippyMapWidget::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture) {
        auto *gestureEvent = dynamic_cast<QGestureEvent*>(event);
        if (QGesture *gesture = gestureEvent->gesture(Qt::TapAndHoldGesture)) {
            auto *tapAndHold = qobject_cast<QTapAndHoldGesture*>(gesture);

            if (gesture->state() == Qt::GestureFinished)
                emit contextMenuRequested(QPoint(
                    (int)tapAndHold->position().x(),
                    (int)tapAndHold->position().y()
                ));
        }
        return true;
    }
    if (event->type() == QEvent::MouseButtonDblClick) {
        if (m_activeObject != nullptr) {
            emit objectDoubleClicked(m_activeObject);
        }
        return true;
    }
    return QWidget::event(event);
}

void SlippyMapWidget::mousePressEvent(QMouseEvent *event)
{
    setFocus(Qt::MouseFocusReason);
    m_dragging = true;
    m_dragStart = event->pos();
    m_dragRealStart = event->pos();
    m_dragButton = event->button();

    if (event->button() == Qt::LeftButton) {
        if (m_drawMode == NoDrawing) {
        }
        else {
            switch (m_drawMode) {
                case RectDrawing:
                case EllipseDrawing:
                    m_drawModeRect_topLeft = m_dragStart;
                    break;
                default:
                    break;
            }
        }
    }
}

void SlippyMapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_dragging = false;

    if (event->button() == Qt::LeftButton && event->pos() == m_dragRealStart) {
        if (m_drawMode == NoDrawing) {
            QPointF geoPos = widgetCoordsToGeoCoords(event->pos());
            if (m_layerManager != nullptr) {
                for (SlippyMapLayer *layer : m_layerManager->layers()) {
                    for (SlippyMapLayerObject *object : layer->objects()) {
                        if (layer->isVisible() && object->contains(geoPos, m_zoomLevel)) {
                            m_activeObject = object;
                            m_layerManager->setActiveLayer(layer);
                            layer->deactivateAll();
                            object->setActive(true);
                            emit objectActivated(object);
                            update();
                            return;
                        }
                    }
                }

                // user clicked outside of any object, deselect currently
                // selected object (if set)
                if (m_activeObject != nullptr) {
                    for (auto *layer : m_layerManager->layers()) {
                        if (layer->contains(m_activeObject)) {
                            layer->deactivateAll();
                            m_activeObject->setActive(false);
                            emit objectDeactivated(m_activeObject);
                            m_activeObject = nullptr;
                            update();
                            return;
                        }
                    }
                }
            }

            m_activeObject = nullptr;
            update();
            return;
        }
        else {
            m_drawMode = NoDrawing;
            setCursor(Qt::ArrowCursor);
            update();
        }
    }
    else if (event->button() == Qt::LeftButton && m_drawMode != NoDrawing) {
        switch (m_drawMode) {
            case RectDrawing:
                emit rectSelected(QRect(m_drawModeRect_topLeft, m_drawModeRect_bottomRight));
                break;
            case EllipseDrawing:
                emit ellipseSelected(QRect(m_drawModeRect_topLeft, m_drawModeRect_bottomRight));
                break;
            default:
                break;
        }

        m_drawMode = NoDrawing;
        setCursor(Qt::ArrowCursor);
        emit drawModeChanged(NoDrawing);
        update();
    }
}

void SlippyMapWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_drawMode == NoDrawing) {
        if (m_activeObject != nullptr) {
            QPoint mousePos = event->pos();
            double mouseLat = widgetY2lat(mousePos.y());
            double mouseLon = widgetX2long(mousePos.x());
            QPointF mouseCoords = QPointF(mouseLon, mouseLat);

            if (m_activeObject->contains(mouseCoords, m_zoomLevel)) {
                setCursor(Qt::SizeAllCursor);
                return;
            }

            setCursor(Qt::ArrowCursor);
        }
    }

    if (m_dragButton != Qt::LeftButton) return;

    double scale_factor = 1 / cos(m_lat * (M_PI / 180.0));
    double deg_per_pixel = (360.0 / pow(2.0, m_zoomLevel)) / 256.0;
    double deg_per_pixel_y = deg_per_pixel / scale_factor;
    QPoint pos = event->pos();
    QPoint diff = pos - m_dragStart;

    //qreal distance = sqrt(pow(diff.x(), 2) + pow(diff.y(), 2));

    if (m_drawMode != NoDrawing) {
        switch (m_drawMode) {
            case RectDrawing:
            case EllipseDrawing: {
                m_drawModeRect_bottomRight = pos;
                break;
            }
            default:
                break;
        }
        update();
    }
    else {
        if (m_dragging && m_dragObject != nullptr && m_dragObject->isMovable()) {
            //QPointF markerPoint = m_dragMarker->position();
            m_dragStart = pos;
            //markerPoint.setY(markerPoint.y() - (deg_per_pixel_y * diff.y()));
            //markerPoint.setX(markerPoint.x() + (deg_per_pixel * diff.x()));
            //m_dragMarker->setPosition(markerPoint);
            update();
        }
        else if (m_dragging) {
            m_dragStart = pos;
            m_lat = m_lat + (deg_per_pixel_y * diff.y());
            m_lon = m_lon - (deg_per_pixel * diff.x());

            emit centerChanged(m_lat, m_lon);
            remap();
        }
    }

    emit cursorPositionChanged(widgetY2lat(event->pos().y()), widgetX2long(event->pos().x()));
}

void SlippyMapWidget::enterEvent(QEvent *event)
{
    (void)event;
    emit cursorEntered();
}

void SlippyMapWidget::leaveEvent(QEvent *event)
{
    (void)event;
    emit cursorLeft();
}

void SlippyMapWidget::wheelEvent(QWheelEvent *event)
{
    QPoint deg = event->angleDelta();

    if (deg.y() > 0) {
        if (m_zoomLevel < m_maxZoom) {
            m_zoomLevel++;

            if (m_centerOnCursorWhileZooming) {
                setCenter(
                        widgetY2lat(event->position().y()),
                        widgetX2long(event->position().x()));
            }

            remap();
            emit zoomLevelChanged(m_zoomLevel);
        }
    }
    else if (deg.y() < 0) {
        if (m_zoomLevel > m_minZoom) {
            m_zoomLevel--;
            remap();
            emit zoomLevelChanged(m_zoomLevel);
        }
    }
}

void SlippyMapWidget::resizeEvent(QResizeEvent *event)
{
    (void)event;
    remap();
}

void SlippyMapWidget::contextMenuEvent(QContextMenuEvent *event)
{
    emit contextMenuRequested(event->pos());
//    m_coordAction->setText(latLonToString(widgetY2lat(event->y()), widgetX2long(event->x())));
//    m_contextMenuLocation = event->pos();
//    m_addMarkerAction->setVisible(true);
//    m_deleteMarkerAction->setVisible(false);
//    m_setMarkerLabelAction->setVisible(false);
//    m_activeMarker = nullptr;

//    if (m_markerModel != nullptr) {
//        QList<SlippyMapWidgetMarker *> markers = m_markerModel->markersForRect(boundingBoxLatLon());

//        for (SlippyMapWidgetMarker *marker : markers) {
//            qint32 marker_x = long2widgetX(marker->longitude());
//            qint32 marker_y = lat2widgety(marker->latitude());
//            QRect marker_box(
//                        (marker_x - 5),
//                        (marker_y - 5),
//                        10, 10);
//            if (marker_box.contains(event->pos())) {
//                m_addMarkerAction->setVisible(false);
//                m_setMarkerLabelAction->setVisible(true);
//                m_deleteMarkerAction->setVisible(true);
//                m_activeMarker = marker;
//                break;
//            }
//        }
//    }

//    for (SlippyMapWidgetMarker *marker : m_markers) {
//        qint32 marker_x = long2widgetX(marker->longitude());
//        qint32 marker_y = lat2widgety(marker->latitude());
//        if (event->x() > (marker_x - 5) && event->x() < (marker_x + 5)) {
//            if (event->y() > (marker_y - 5) && event->y() < (marker_y + 5)) {
//                m_addMarkerAction->setVisible(false);
//                m_setMarkerLabelAction->setVisible(true);
//                m_deleteMarkerAction->setVisible(true);
//                m_activeMarker = marker;
//                break;
//            }
//        }
//    }

//    update();
//    emit contextMenuActivated(widgetY2lat(event->y()), widgetX2long(event->x()));
//    m_contextMenu->exec(event->globalPos());
}

/**
 * @brief SlippyMapWidget::long2tilex returns the tile X coordinate for the given longitude and zoom level
 * @param lon
 * @param z
 * @return X coordinate of the tile for lon and zoom level z
 */
qint32 SlippyMapWidget::long2tilex(double lon, int z)
{
    return static_cast<qint32>(floor((lon + 180.0) / 360.0 * pow(2.0, z)));
}

/**
 * @brief SlippyMapWidget::lat2tiley returns the tile Y coordinate for the given latitude and zoom level
 * @param lat
 * @param z
 * @return Y coordinate of the tile for lat and zoom level z
 */
qint32 SlippyMapWidget::lat2tiley(double lat, int z)
{
    return static_cast<qint32>(floor((1.0 - log( tan(lat * M_PI/180.0) + 1.0 / cos(lat * M_PI/180.0)) / M_PI) / 2.0 * pow(2.0, z)));
}

double SlippyMapWidget::tilex2long(qint32 x, qint32 z)
{
    return x / pow(2.0, z) * 360.0 - 180;
}

double SlippyMapWidget::tiley2lat(qint32 y, qint32 z)
{
    double n = M_PI - 2.0 * M_PI * y / pow(2.0, z);
    return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}

qint32 SlippyMapWidget::long2widgetX(double lon)
{
    double deg_per_pixel = (360.0 / pow(2.0, m_zoomLevel)) / 256.0;
    double width_deg = deg_per_pixel * width();
    double left_deg = m_lon - (width_deg / 2);
    return static_cast<qint32>((lon - left_deg) / deg_per_pixel);
}

qint32 SlippyMapWidget::lat2widgety(double lat)
{
    double scale_factor = 1 / cos(m_lat * (M_PI / 180.0));
    double deg_per_pixel = (360.0 / pow(2.0, m_zoomLevel)) / 256.0;
    double deg_per_pixel_y = deg_per_pixel / scale_factor;
    double height_deg = deg_per_pixel_y * height();
    double top_deg = m_lat - (height_deg / 2);
    return height() - static_cast<qint32>((lat - top_deg) / deg_per_pixel_y);
}

QPointF SlippyMapWidget::widgetCoordsToGeoCoords(QPoint point)
{
    return {widgetX2long(point.x()), widgetY2lat(point.y())};
}

void SlippyMapWidget::setDrawMode(SlippyMapWidget::DrawMode mode)
{
    setCursor(Qt::CrossCursor);
    m_drawMode = mode;
}

SlippyMapWidget::DrawMode SlippyMapWidget::drawMode()
{
    return m_drawMode;
}

int SlippyMapWidget::zoomLevel() const
{
    return m_zoomLevel;
}

double SlippyMapWidget::widgetX2long(qint32 x)
{
    double deg_per_pixel = (360.0 / pow(2.0, m_zoomLevel)) / 256.0;
    double width_deg = deg_per_pixel * width();
    double left_deg = m_lon - (width_deg / 2);
    double xpos = left_deg + (deg_per_pixel * x);
    return xpos;
}

double SlippyMapWidget::widgetY2lat(qint32 y)
{
    double scale_factor = 1 / cos(m_lat * (M_PI / 180.0));  // 1 / cos(lat in radians)
    double deg_per_pixel = (360.0 / pow(2.0, m_zoomLevel)) / 256.0;
    double deg_per_pixel_y = deg_per_pixel / scale_factor;
    double height_deg = deg_per_pixel_y * height();
    double top_deg = m_lat + (height_deg / 2);
    double ypos = top_deg - (deg_per_pixel_y * y);
    return ypos;
}

double SlippyMapWidget::degPerPixelX() const
{
    return (360.0 / pow(2.0, m_zoomLevel)) / 256.0;
}

double SlippyMapWidget::degPerPixelY() const
{
    double scale_factor = 1 / cos(m_lat * (M_PI / 180.0));
    double deg_per_pixel = (360.0 / pow(2.0, m_zoomLevel)) / 256.0;
    return deg_per_pixel / scale_factor;
}

QRectF SlippyMapWidget::boundingBoxLatLon()
{
    double width_deg = degPerPixelX() * width();
    double left_deg = m_lon - (width_deg / 2);

    double height_deg = degPerPixelY() * height();
    double top_deg = m_lat + (height_deg / 2);

    return {left_deg, top_deg, width_deg, -height_deg};
}

void SlippyMapWidget::remap()
{
    auto tiles_wide = static_cast<qint32>(ceil(static_cast<double>(width()) / 256.0));
    if ((tiles_wide % 2) == 0) tiles_wide++;

    auto tiles_high = static_cast<qint32>(ceil(static_cast<double>(height()) / 256.0));
    if ((tiles_high % 2) == 0) tiles_high++;

    // always include what's immediately off-screen
    tiles_wide += 2;
    tiles_high += 2;

    qint32 tile_max = qRound(pow(2.0, m_zoomLevel));

    // tile_x and tile_y are the tile coords for the center tile
    qint32 tile_x = long2tilex(m_lon, m_zoomLevel);
    qint32 tile_y = lat2tiley(m_lat, m_zoomLevel);

    // tile coords for top left tile
    qint32 tile_x_start = tile_x - (tiles_wide - 1) / 2;
    qint32 tile_y_start = tile_y - (tiles_high - 1) / 2;

//    if (tile_x_start < 0)
//        tile_x_start += static_cast<qint32>(pow(2.0, m_zoomLevel));
//
//    if (tile_y_start < 0)
//        tile_y_start += static_cast<qint32>(pow(2.0, m_zoomLevel));

    m_tileX = tile_x;
    m_tileY = tile_y;

    double scale_factor = 1 / cos(m_lat * (M_PI / 180.0));
    double deg_per_pixel = (360.0 / pow(2.0, m_zoomLevel)) / 256.0;
    double deg_per_pixel_y = deg_per_pixel / scale_factor;

    // calculate offset between widget center and center tile center
    double snapped_lon = tilex2long(long2tilex(m_lon, m_zoomLevel), m_zoomLevel);
    double snapped_lat = tiley2lat(lat2tiley(m_lat, m_zoomLevel), m_zoomLevel);
    double diff_lon = snapped_lon - m_lon;
    double diff_lat = snapped_lat - m_lat;

    qint32 diff_pix_x = 255 + static_cast<qint32>(diff_lon / deg_per_pixel) - 128;
    qint32 diff_pix_y = static_cast<qint32>(diff_lat / deg_per_pixel_y) - 128;

    qint32 centerX = width() / 2;
    qint32 centerY = height() / 2;

    qint32 startX = centerX - 128 - ((tiles_wide / 2) * 256) + diff_pix_x;
    qint32 startY = centerY - 128 - ((tiles_high / 2) * 256) - diff_pix_y;

    for (SlippyMapWidgetLayer *layer : m_layers) {
        QList<Tile*> deleteList(m_layerTileMaps[layer]);

        if (layer->isVisible()) {
            for (int y = 0; y < tiles_high; y++) {
                for (int x = 0; x < tiles_wide; x++) {
                    qint32 this_x = tile_x_start + x;
                    qint32 this_y = tile_y_start + y;
                    QPoint point(startX + (x * 256), startY + (y * 256));

                    // make sure the tile x and y coordinates are constrained
                    // to the min of 0 and max of 2^zoom_level - 1
                    while (this_x < 0)
                        this_x += tile_max;
                    while (this_x >= tile_max)
                        this_x -= tile_max;
                    while (this_y < 0)
                        this_y += tile_max;
                    while (this_y >= tile_max)
                        this_y -= tile_max;

                    Tile *tile = nullptr;
                    for (int i = 0; i < m_layerTileMaps[layer].length(); i++) {
                        if (m_layerTileMaps[layer].at(i)->x() == this_x && m_layerTileMaps[layer].at(i)->y() == this_y) {
                            tile = m_layerTileMaps[layer].at(i);
                        }
                    }

                    // if this layer has been marked expired, we leave it in
                    // the deleteList and create a new one
                    if (m_expiredLayers.contains(layer)) {
                        tile = nullptr;
                    }

                    if (tile == nullptr) {
                        tile = new Tile(this_x, this_y, point);
                        m_layerTileMaps[layer].append(tile);

                        // tile URL should have something like %1/%2/%3.png
                        // %1 = z, %2 = x, %3 = y
                        QString tileUrl = layer->tileUrl()
                                .arg(m_zoomLevel)
                                .arg(this_x)
                                .arg(this_y);
                        QString fileExt = QUrl(tileUrl).fileName().split(".").last();
                        QString fileName = QString("%1/%2/%3.%4")
                                .arg(m_zoomLevel)
                                .arg(this_x)
                                .arg(this_y)
                                .arg(fileExt);
                        QString cachedFileName = QString("%1/%2/%3")
                                .arg(m_cacheTileDir)
                                .arg(layer->tileUrlHash())
                                .arg(fileName);

                        bool cacheHit = false;

                        // is caching enabled?
                        if (layer->cacheDuration() > 0) {
                            // does a cache file exist?
                            if (QFile::exists(cachedFileName)) {
                                const QFileInfo info(cachedFileName);
                                const QDateTime lastModified = info.lastModified();
                                const qint64 diff = lastModified.msecsTo(QDateTime::currentDateTime()) / 1000;

                                // is it fresh?
                                if (diff <= layer->cacheDuration())
                                    cacheHit = true;
                            }
                        }

                        if (cacheHit) {
                            QPixmap pixmap(cachedFileName);
                            tile->setPixmap(pixmap);
                            update();
                        }
                        else {
                            QNetworkRequest req(tileUrl);
                            req.setRawHeader(QByteArray("User-Agent"), m_userAgentString.toLocal8Bit());
                            req.setSslConfiguration(QSslConfiguration::defaultConfiguration());
                            emit tileRequestInitiated();
                            QNetworkReply *reply = m_net->get(req);
                            tile->setPendingReply(reply);
                            connect(reply,
                                    &QNetworkReply::errorOccurred,
                                    [=](QNetworkReply::NetworkError code){
                                        qWarning() << "Got a network error:" << code;
                                        if (tile->isDiscarded()) {
                                            reply->deleteLater();
                                            delete tile;
                                        }
                            });
                            connect(reply,
                                    &QNetworkReply::finished,
                                    [this, tile, layer, cachedFileName, reply]() {
                                emit tileRequestFinished();

                                if (reply->error() != QNetworkReply::NoError) {
                                    qWarning() << "Got a network error:" << reply->errorString();
                                    if (tile->isDiscarded()) {
                                        reply->deleteLater();
                                        delete tile;
                                    }
                                    return;
                                }

                                QByteArray data = reply->readAll();

                                if (layer->cacheDuration() > 0) {
                                    QFile cacheFile(cachedFileName);
                                    QFileInfo cacheFileInfo(cacheFile);
                                    cacheFileInfo.dir().mkpath(".");
                                    if (cacheFile.open(QIODevice::ReadWrite)) {
                                        cacheFile.write(data);
                                        cacheFile.close();
                                    }
                                }

                                if (tile->isDiscarded()) {
                                    delete tile;
                                }
                                else {
                                    QPixmap pixmap;
                                    pixmap.loadFromData(data);
                                    tile->setPixmap(pixmap);
                                    tile->setPendingReply(nullptr);
                                    update();

                                    reply->deleteLater();
                                }
                            });
                        }
                    }
                    else {
                        QPoint old = tile->point();
                        tile->setPoint(point);
                        deleteList.removeOne(tile);
                        if (old != point) update();
                    }
                }
            }
        }

        if (m_expiredLayers.contains(layer))
            m_expiredLayers.removeOne(layer);

        while (!deleteList.empty()) {
            Tile *todelete = deleteList.takeFirst();

            //
            // allow the pending request to finish so the tile can
            // be cached, and then delete the discarded tile
            //
            if (todelete->pendingReply() != nullptr && todelete->pendingReply()->isRunning()) {
                m_layerTileMaps[layer].removeOne(todelete);
                todelete->discard();
            }
            else {
                m_layerTileMaps[layer].removeOne(todelete);
                delete todelete;
            }
        }
    }
}

QPixmap SlippyMapWidget::drawLegend(SlippyMapWidgetLayer *layer, int width, int height)
{
    const QList<QColor>& legendColors = layer->legendColors();
    const QList<QString>& legendLabels = layer->legendLabels();
    const int labelSpacing = 5;

    QFont labelFont = qApp->font();
    labelFont.setPixelSize(height / (legendLabels.length() + 2));

    QFontMetrics fontMetrics(labelFont);
    int maxLabelWidth = 0;
    int labelHeight = fontMetrics.height();
    for (int i = 0; i < legendLabels.length(); i++) {
        int tw = fontMetrics.horizontalAdvance(legendLabels.at(i));
        if (tw > maxLabelWidth) maxLabelWidth = tw;
    }

    QPixmap canvas(width + (labelSpacing * 2) + maxLabelWidth, height);
    canvas.fill(Qt::transparent);

    QPainter painter(&canvas);
    QLinearGradient gradient(
            QPointF(0, 0),
            QPointF(0, height)
    );

    for (int i = 0; i < legendColors.length(); i++) {
        double stop = (1.0 / legendColors.length()) * i;
        gradient.setColorAt(stop, legendColors.at(i));
    }

    painter.save();
    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    QRect pos = QRect(
            0, 0, width, height);
    painter.drawRect(pos);
    painter.restore();

    painter.save();
    painter.setBrush(qApp->palette().base());
    painter.setPen(Qt::NoPen);
    painter.drawRect(width, 0, (labelSpacing * 2) + maxLabelWidth, height);
    painter.restore();

    painter.save();
    painter.setBrush(Qt::SolidPattern);
    painter.setPen(QPen(qApp->palette().text().color()));
    painter.setFont(labelFont);

    for (int i = 0; i < legendLabels.length(); i++) {
        int xpos = width + labelSpacing;
        int ypos = ((height / legendLabels.length()) * i);
        const QRect bbox(xpos, ypos, maxLabelWidth, labelHeight);
        painter.drawText(bbox, Qt::AlignRight | Qt::AlignVCenter, legendLabels.at(i));
    }

    painter.restore();
    painter.end();

    return canvas;
}

void SlippyMapWidget::setLegendAlignment(const Qt::Alignment &alignment)
{
    m_legendAlignment = alignment;
}

void SlippyMapWidget::setLegendSpacing(const int legendSpacing)
{
    m_legendSpacing = legendSpacing;
}

void SlippyMapWidget::setUserAgent(const QString &userAgent)
{
    m_userAgentString = userAgent;
}

const QString &SlippyMapWidget::userAgent() const
{
    return m_userAgentString;
}
