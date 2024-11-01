#include <SlippyMap/SlippyMapLayerPolygonPropertyPage.h>
#include <SlippyMap/SlippyMap.h>

#include <QDebug>
#include <QApplication>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPointF>
#include <QListWidget>

SlippyMapLayerPolygonPropertyPage::SlippyMapLayerPolygonPropertyPage(SlippyMapLayerObject *object):
    SlippyMapLayerObjectPropertyPage(object),
    m_strokeWidth(nullptr),
    m_strokeColor(nullptr),
    m_fillColor(nullptr)
{
    m_polygon = qobject_cast<SlippyMapLayerPolygon*>(object);
}


QString SlippyMapLayerPolygonPropertyPage::tabTitle()
{
    return tr("Polygon");
}

void SlippyMapLayerPolygonPropertyPage::setupUi()
{
    m_strokeColor = new QLineEdit();
    m_strokeWidth = new QLineEdit();
    m_fillColor = new QLineEdit();

    m_strokeColor->setText(m_polygon->strokeColor().name(QColor::HexArgb));
    m_strokeWidth->setText(QString("%1").arg(m_polygon->strokeWidth()));
    m_fillColor->setText(m_polygon->fillColor().name(QColor::HexArgb));

    auto *formLayout = new QFormLayout();
    formLayout->addRow(tr("Stroke Width"), m_strokeWidth);
    formLayout->addRow(tr("Stroke Color"), m_strokeColor);
    formLayout->addRow(tr("Fill Color"), m_fillColor);

    m_pointList = new QListWidget();
    for (const auto& point : m_polygon->points()) {
        QString label = latLonToString(point);
        m_pointList->addItem(label);
    }

    auto *vLayout = new QVBoxLayout();
    vLayout->addLayout(formLayout);
    vLayout->addWidget(m_pointList);

    setLayout(vLayout);
}

void SlippyMapLayerPolygonPropertyPage::updateUi()
{
}

void SlippyMapLayerPolygonPropertyPage::save()
{
    auto *obj = qobject_cast<SlippyMapLayerPolygon*>(m_object);
    obj->setFillColor(QColor(m_fillColor->text()));
    obj->setStrokeColor(QColor(m_strokeColor->text()));
    obj->setStrokeWidth(m_strokeWidth->text().toInt());
}
