#include <SlippyMap/SlippyMapLayerObjectPropertyPage.h>
#include <SlippyMap/SlippyMapWidgetMarker.h>
#include <SlippyMap/SlippyMapLayer.h>

#include <QDebug>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QCheckBox>
#include <QFormLayout>
#include <QComboBox>


SlippyMapLayerObjectPropertyPage::SlippyMapLayerObjectPropertyPage(SlippyMapLayerObject *object) :
    QWidget()
{
    m_object = object;
    m_layerManager = nullptr;
}

SlippyMapLayerObjectPropertyPage::SlippyMapLayerObjectPropertyPage(
        SlippyMapLayerObject *object,
        SlippyMapLayerManager *layerManager) : QWidget()
{
    m_object = object;
    m_layerManager = layerManager;
}

SlippyMapLayerObjectPropertyPage::~SlippyMapLayerObjectPropertyPage()
{

}

QString SlippyMapLayerObjectPropertyPage::tabTitle()
{
    return tr("Properties");
}

void SlippyMapLayerObjectPropertyPage::save()
{
    auto *obj = qobject_cast<SlippyMapWidgetMarker*>(m_object);
    obj->setLabel(m_label->text());
    obj->setDescription(m_description->toPlainText());
    obj->setVisible(m_visibility->isChecked());

    QPointF position;
    position.setY(m_latitude->text().toDouble());
    position.setX(m_longitude->text().toDouble());
    obj->setPosition(position);

    int selectedLayerIndex = m_layerComboBox->currentIndex();
    int layerIndex = 0;
    SlippyMapLayer *parentLayer = nullptr;
    for (auto *layer : m_layerManager->layers()) {
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
        for (auto *layer : m_layerManager->layers()) {
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

void SlippyMapLayerObjectPropertyPage::setupUi()
{
    qDebug() << "Setting up Object property page UI";
    m_label = new QLineEdit();
    m_description = new QPlainTextEdit();
    m_latitude = new QLineEdit();
    m_longitude = new QLineEdit();
    m_visibility = new QCheckBox();
    m_layerComboBox = new QComboBox();

    if (m_layerManager) {
        for (auto *layer : m_layerManager->layers()) {
            if (layer->isEditable())
                m_layerComboBox->addItem(layer->name());
        }
    }

    int layerIndex = 0;
    for (auto *layer : m_layerManager->layers()) {
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
    m_latitude->setText(QString("%1").arg(m_object->position().y()));
    m_longitude->setText(QString("%1").arg(m_object->position().x()));
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
