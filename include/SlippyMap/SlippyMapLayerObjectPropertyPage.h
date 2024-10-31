#ifndef SLIPPYMAPSHAPEPROPERTYPAGE_H
#define SLIPPYMAPSHAPEPROPERTYPAGE_H

#include <QWidget>
#include <SlippyMap/SlippyMapLayerObject.h>
#include <SlippyMap/SlippyMapLayerManager.h>

class QLineEdit;
class QLabel;
class QPlainTextEdit;
class QCheckBox;
class QComboBox;

using namespace SlippyMap;

class SlippyMapLayerObjectPropertyPage : public QWidget
{
    Q_OBJECT
public:
    explicit SlippyMapLayerObjectPropertyPage(SlippyMapLayerObject *object);
    SlippyMapLayerObjectPropertyPage(SlippyMapLayerObject *object, SlippyMapLayerManager *layerManager);
    ~SlippyMapLayerObjectPropertyPage() override;
    virtual QString tabTitle();
    virtual void save();
    virtual void setupUi();

protected:

    SlippyMapLayerObject *m_object;
    SlippyMapLayerManager *m_layerManager;
    QLineEdit *m_label;
    QLineEdit *m_latitude;
    QLineEdit *m_longitude;
    QPlainTextEdit *m_description;
    QCheckBox *m_visibility;
    QComboBox *m_layerComboBox;
};

#endif // SLIPPYMAPSHAPEPROPERTYPAGE_H
