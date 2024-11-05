//
// Created by robin on 10/30/2024.
//

#ifndef SLIPPYMAPLAYERMARKERPROPERTYPAGE_H
#define SLIPPYMAPLAYERMARKERPROPERTYPAGE_H

#include "SlippyMapLayerObjectPropertyPage.h"
#include <QtColorWidgets/ColorSelector>

class QLineEdit;
class QTabWidget;
class QLabel;
class QPlainTextEdit;
class QCheckBox;

using namespace color_widgets;


class SlippyMapLayerMarkerPropertyPage : public SlippyMapLayerObjectPropertyPage {
    Q_OBJECT
public:
    explicit SlippyMapLayerMarkerPropertyPage(const SlippyMap::SlippyMapLayerObject::Ptr& object);
    void save() override;
    QString tabTitle() override;
    void setupUi() override;

public slots:
    void updateUi() override;

protected:

    QLineEdit *m_name;
    QLineEdit *m_latitude;
    QLineEdit *m_longitude;
    QLineEdit *m_radius;
    ColorSelector *m_color;
    QCheckBox *m_visibility;
    QPlainTextEdit *m_description;
    QTabWidget *m_tabWidget;
};



#endif //SLIPPYMAPLAYERMARKERPROPERTYPAGE_H
