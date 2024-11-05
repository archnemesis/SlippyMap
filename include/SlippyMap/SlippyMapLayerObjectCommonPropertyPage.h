//
// Created by robin on 10/31/2024.
//

#ifndef SLIPPYMAPLAYEROBJECTCOMMONPROPERTYPAGE_H
#define SLIPPYMAPLAYEROBJECTCOMMONPROPERTYPAGE_H
#include "SlippyMap/SlippyMapLayerObjectPropertyPage.h"


namespace SlippyMap {
    class SlippyMapLayerObject;
    class SlippyMapLayerPolygon;
}

class SlippyMapLayerObjectCommonPropertyPage : public SlippyMapLayerObjectPropertyPage
{
public:
    explicit SlippyMapLayerObjectCommonPropertyPage(SlippyMap::SlippyMapLayerObject::Ptr object);
    SlippyMapLayerObjectCommonPropertyPage(SlippyMap::SlippyMapLayerObject::Ptr object, SlippyMapLayerManager *layerManager);

    QString tabTitle() override;
    void setupUi() override;
    void save() override;

public slots:
    void updateUi() override;

protected:
    SlippyMapLayerManager *m_layerManager;
    QLineEdit *m_label;
    QLineEdit *m_latitude;
    QLineEdit *m_longitude;
    QPlainTextEdit *m_description;
    QCheckBox *m_visibility;
    QComboBox *m_layerComboBox;
};



#endif //SLIPPYMAPLAYEROBJECTCOMMONPROPERTYPAGE_H
