#include "SlippyMap/SlippyMapWidgetLayer.h"

#include <QByteArray>
#include <QCryptographicHash>
#include <QPainter>
#include <QPixmap>
#include <QLinearGradient>

using namespace SlippyMap;

SlippyMapWidgetLayer::SlippyMapWidgetLayer(const QString& tileUrl, QObject *parent) :
        QObject(parent)
{
    m_tileUrl = tileUrl;
    m_visible = true;
}

SlippyMapWidgetLayer::SlippyMapWidgetLayer(QObject *parent) :
        QObject(parent)
{

}

void SlippyMapWidgetLayer::setTileUrl(const QString& tileUrl)
{
    m_tileUrl = tileUrl;
    emit updated();
}

void SlippyMapWidgetLayer::setName(const QString& name)
{
    m_name = name;
    emit updated();
}

void SlippyMapWidgetLayer::setDescription(const QString& description)
{
    m_description = description;
    emit updated();
}

void SlippyMapWidgetLayer::setZOrder(int zOrder)
{
    m_zOrder = zOrder;
    emit updated();
}

void SlippyMapWidgetLayer::setVisible(bool visible)
{
    m_visible = visible;
    emit updated();
}

const QString& SlippyMapWidgetLayer::name()
{
    return m_name;
}

const QString& SlippyMapWidgetLayer::description()
{
    return m_description;
}

const QString& SlippyMapWidgetLayer::tileUrl()
{
    return m_tileUrl;
}

QString SlippyMapWidgetLayer::tileUrlHash()
{
    QByteArray hash = QCryptographicHash::hash(
            tileUrl().toLocal8Bit(),
            QCryptographicHash::Md5).toHex();

    return QString::fromLocal8Bit(hash);
}

int SlippyMapWidgetLayer::zOrder()
{
    return m_zOrder;
}

bool SlippyMapWidgetLayer::isVisible()
{
    return m_visible;
}

void SlippyMapWidgetLayer::setLegendStyle(SlippyMapWidgetLayer::LegendStyle style)
{

}

bool SlippyMapWidgetLayer::isBaseLayer()
{
    return m_baseLayer;
}

void SlippyMapWidgetLayer::setBaseLayer(bool baseLayer)
{
    m_baseLayer = baseLayer;
}

void SlippyMapWidgetLayer::addLegendColor(const QColor &color, const QString &label)
{
    m_legendColors.append(color);
    m_legendLabels.append(label);
}

void SlippyMapWidgetLayer::setCacheDuration(int duration)
{
    m_cacheDuration = duration;
}

int SlippyMapWidgetLayer::cacheDuration()
{
    return m_cacheDuration;
}

QPixmap SlippyMapWidgetLayer::legendPixmap(int width, int height)
{
    QPixmap canvas(width, height);
    QPainter painter(&canvas);
    QLinearGradient gradient(
            QPointF(0, 0),
            QPointF(width, height)
            );

    for (int i = 0; i < m_legendColors.length(); i++) {
        double stop = (1.0 / m_legendColors.length()) * i;
        gradient.setColorAt(stop, m_legendColors.at(i));
    }

    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    QRect pos = QRect(
            0, 0, width, height);
    painter.drawRect(pos);
    painter.end();

    return canvas;
}

const QList<QColor> &SlippyMapWidgetLayer::legendColors()
{
    return m_legendColors;
}

const QList<QString> &SlippyMapWidgetLayer::legendLabels()
{
    return m_legendLabels;
}