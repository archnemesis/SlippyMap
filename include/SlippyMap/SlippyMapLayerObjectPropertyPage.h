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
    explicit SlippyMapLayerObjectPropertyPage(const SlippyMapLayerObject::Ptr& object, QWidget *parent = nullptr);
    ~SlippyMapLayerObjectPropertyPage() override;
    virtual QString tabTitle() = 0;
    virtual void save() = 0;
    virtual void setupUi() = 0;

public slots:
    virtual void updateUi() = 0;

protected:

    SlippyMapLayerObject::Ptr m_object;
};

#endif // SLIPPYMAPSHAPEPROPERTYPAGE_H
