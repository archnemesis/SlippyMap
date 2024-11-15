#ifndef SLIPPYMAPPOLYGONPROPERTYPAGE_H
#define SLIPPYMAPPOLYGONPROPERTYPAGE_H

#include <SlippyMap/SlippyMapLayerObjectPropertyPage.h>
#include <SlippyMap/SlippyMapLayerPolygon.h>
#include <QtColorWidgets/ColorSelector>

using namespace SlippyMap;
using namespace color_widgets;

class QLineEdit;
class QListWidget;

class SlippyMapLayerPolygonPropertyPage : public SlippyMapLayerObjectPropertyPage
{
public:
    explicit SlippyMapLayerPolygonPropertyPage(const SlippyMap::SlippyMapLayerObject::Ptr& object);
    virtual QString tabTitle();
    void setupUi() override;
    void save() override;

public slots:
    void updateUi() override;

protected:

    SlippyMapLayerPolygon *m_polygon;
    ColorSelector *m_strokeColor;
    QLineEdit *m_strokeWidth;
    ColorSelector *m_fillColor;
    QListWidget *m_pointList;
};

#endif // SLIPPYMAPPOLYGONPROPERTYPAGE_H
