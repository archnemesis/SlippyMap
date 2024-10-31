#ifndef SLIPPYMAPPOLYGONPROPERTYPAGE_H
#define SLIPPYMAPPOLYGONPROPERTYPAGE_H

#include <SlippyMap/SlippyMapLayerObjectPropertyPage.h>
#include <SlippyMap/SlippyMapLayerPolygon.h>

using namespace SlippyMap;

class QLineEdit;

class SlippyMapLayerPolygonPropertyPage : public SlippyMapLayerObjectPropertyPage
{
public:
    explicit SlippyMapLayerPolygonPropertyPage(SlippyMapLayerObject *object);
    virtual QString tabTitle();
    void setupUi() override;

protected:

    SlippyMapLayerPolygon *m_polygon;
    QLineEdit *m_lneX;
    QLineEdit *m_lneY;
    QLineEdit *m_lneWidth;
    QLineEdit *m_lneHeight;
};

#endif // SLIPPYMAPPOLYGONPROPERTYPAGE_H
