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


SlippyMapLayerMarkerPropertyPage::SlippyMapLayerMarkerPropertyPage(SlippyMapLayerObject *object) :
    SlippyMapLayerObjectPropertyPage(object) {
}

void SlippyMapLayerMarkerPropertyPage::setupUi()
{
    qDebug() << "Setting up Marker property page UI";
    m_radius = new QLineEdit;
    m_color = new QLineEdit;

    if (!m_object->isEditable()) {
        m_radius->setEnabled(false);
        m_color->setEnabled(false);
    }

    auto *marker = qobject_cast<SlippyMapWidgetMarker*>(m_object);

    m_color->setText(marker->color().name(QColor::HexRgb));
    m_radius->setText(QString("%1").arg(marker->radius()));

    QColor markerColor = marker->color();
    m_color->setText(markerColor.name(QColor::HexArgb));
    m_radius->setText(QString("%1").arg(marker->radius()));

    auto *stylePropertiesLayout = new QFormLayout();
    stylePropertiesLayout->addRow(tr("Radius"), m_radius);
    stylePropertiesLayout->addRow(tr("Color"), m_color);

    setLayout(stylePropertiesLayout);
}

void SlippyMapLayerMarkerPropertyPage::save()
{
    auto *obj = qobject_cast<SlippyMapWidgetMarker*>(m_object);
    QColor newColor = QColor(m_color->text());
    obj->setColor(newColor);
    obj->setRadius(m_radius->text().toInt());
}

QString SlippyMapLayerMarkerPropertyPage::tabTitle()
{
    return tr("Style");
}
