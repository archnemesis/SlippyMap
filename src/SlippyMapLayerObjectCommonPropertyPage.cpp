//
// Created by robin on 10/31/2024.
//

#include "SlippyMap/SlippyMapLayerObjectCommonPropertyPage.h"
#include <SlippyMap/SlippyMapWidgetMarker.h>
#include <SlippyMap/SlippyMapLayer.h>

#include <QDebug>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QCheckBox>
#include <QFormLayout>
#include <QComboBox>

SlippyMapLayerObjectCommonPropertyPage::SlippyMapLayerObjectCommonPropertyPage(
    SlippyMap::SlippyMapLayerObject::Ptr object) :
    SlippyMapLayerObjectPropertyPage(object)
{
}

SlippyMapLayerObjectCommonPropertyPage::SlippyMapLayerObjectCommonPropertyPage(
        SlippyMap::SlippyMapLayerObject::Ptr object,
        SlippyMapLayerManager *layerManager) :
        SlippyMapLayerObjectPropertyPage(object),
        m_layerManager(layerManager)
{
}

QString SlippyMapLayerObjectCommonPropertyPage::tabTitle()
{
    return tr("Properties");
}

void SlippyMapLayerObjectCommonPropertyPage::save()
{
    m_object->setLabel(m_label->text());
    m_object->setDescription(m_description->toPlainText());
    m_object->setVisible(m_visibility->isChecked());

    QPointF position;
    position.setY(m_latitude->text().toDouble());
    position.setX(m_longitude->text().toDouble());
    m_object->setPosition(position);

    int selectedLayerIndex = m_layerComboBox->currentIndex();
    int layerIndex = 0;
    SlippyMapLayer::Ptr parentLayer;
    for (const auto& layer: m_layerManager->layers()) {
        if (layer->isEditable()) {
            if (layer->contains(m_object)) {
                parentLayer = layer;
                break;
            }
            layerIndex++;
        }
    }

    Q_ASSERT(parentLayer != nullptr);

    if (selectedLayerIndex != layerIndex) {
        m_layerManager->removeLayerObject(parentLayer, m_object);
        int newLayerIndex = 0;
        for (const auto& layer: m_layerManager->layers()) {
            if (layer->isEditable()) {
                if (newLayerIndex == selectedLayerIndex) {
                    m_layerManager->addLayerObject(layer, m_object);
                    break;
                }
                newLayerIndex++;
            }
        }
    }
}

void SlippyMapLayerObjectCommonPropertyPage::setupUi()
{
    m_label = new QLineEdit();
    m_description = new QPlainTextEdit();
    m_latitude = new QLineEdit();
    m_longitude = new QLineEdit();
    m_visibility = new QCheckBox();
    m_layerComboBox = new QComboBox();

    if (m_layerManager) {
        for (const auto& layer : m_layerManager->layers()) {
            if (layer->isEditable())
                m_layerComboBox->addItem(layer->name());
        }
    }

    int layerIndex = 0;
    for (const auto& layer : m_layerManager->layers()) {
        if (layer->isEditable()) {
            if (layer->contains(m_object)) {
                m_layerComboBox->setCurrentIndex(layerIndex);
                break;
            }
            layerIndex++;
        }
    }

    if (!m_object->isEditable()) {
        m_label->setEnabled(false);
        m_description->setEnabled(false);
        m_latitude->setEnabled(false);
        m_longitude->setEnabled(false);
        m_visibility->setEnabled(false);
    }

    m_label->setText(m_object->label());
    m_description->setPlainText(m_object->description());
    m_latitude->setText(QString("%1")
        .arg(m_object->position().y(), 0, 'f', 6));
    m_longitude->setText(QString("%1")
        .arg(m_object->position().x(), 0, 'f', 6));
    m_visibility->setText(tr("Visible"));
    m_visibility->setChecked(m_object->isVisible());

    auto *mainPropertiesLayout = new QFormLayout();

    mainPropertiesLayout->addRow(tr("Label"), m_label);
    mainPropertiesLayout->addRow(tr("Description"), m_description);
    mainPropertiesLayout->addRow(tr("Latitude"), m_latitude);
    mainPropertiesLayout->addRow(tr("Longitude"), m_longitude);
    mainPropertiesLayout->addRow(tr("Layer"), m_layerComboBox);
    mainPropertiesLayout->addRow("", m_visibility);

    setLayout(mainPropertiesLayout);
}

void SlippyMapLayerObjectCommonPropertyPage::updateUi()
{
    m_label->setText(m_object->label());
    m_description->setPlainText(m_object->description());
    m_latitude->setText(QString("%1")
        .arg(m_object->position().y(), 0, 'f', 6));
    m_longitude->setText(QString("%1")
        .arg(m_object->position().x(), 0, 'f', 6));
    m_visibility->setText(tr("Visible"));
    m_visibility->setChecked(m_object->isVisible());
}
