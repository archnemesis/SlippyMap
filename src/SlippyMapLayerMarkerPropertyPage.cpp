//
// Created by robin on 10/30/2024.
//

#include <SlippyMap/SlippyMapLayerMarkerPropertyPage.h>
#include <SlippyMap/SlippyMapWidgetMarker.h>


#include <QDebug>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QFormLayout>
#include <QPushButton>
#include <QCheckBox>


SlippyMapLayerMarkerPropertyPage::SlippyMapLayerMarkerPropertyPage(const SlippyMap::SlippyMapLayerObject::Ptr& object) :
    SlippyMapLayerObjectPropertyPage(object) {
}

void SlippyMapLayerMarkerPropertyPage::setupUi()
{
    qDebug() << "Setting up Marker property page UI";
    m_radius = new QLineEdit;
    m_color = new ColorSelector;

    if (!m_object->isEditable()) {
        m_radius->setEnabled(false);
        m_color->setEnabled(false);
    }

    auto *marker = qobject_cast<SlippyMapWidgetMarker*>(m_object.get());

    m_color->setColor(marker->color());
    m_radius->setText(QString("%1").arg(marker->radius()));

    auto *stylePropertiesLayout = new QFormLayout();
    stylePropertiesLayout->addRow(tr("Radius"), m_radius);
    stylePropertiesLayout->addRow(tr("Color"), m_color);

    setLayout(stylePropertiesLayout);
}

void SlippyMapLayerMarkerPropertyPage::updateUi()
{
    auto *marker = qobject_cast<SlippyMapWidgetMarker*>(m_object.get());
    m_color->setColor(marker->color());
    m_radius->setText(QString("%1").arg(marker->radius()));
}

void SlippyMapLayerMarkerPropertyPage::save()
{
    auto *obj = qobject_cast<SlippyMapWidgetMarker*>(m_object.get());
    QColor newColor = m_color->color();
    obj->setColor(newColor);
    obj->setRadius(m_radius->text().toInt());
}

QString SlippyMapLayerMarkerPropertyPage::tabTitle()
{
    return tr("Style");
}
