#include <SlippyMap/SlippyMapLayerObjectPropertyPage.h>
#include <SlippyMap/SlippyMapWidgetMarker.h>
#include <SlippyMap/SlippyMapLayer.h>

#include <QComboBox>


SlippyMapLayerObjectPropertyPage::SlippyMapLayerObjectPropertyPage(SlippyMapLayerObject *object) :
    QWidget()
{
    m_object = object;
}

SlippyMapLayerObjectPropertyPage::~SlippyMapLayerObjectPropertyPage()
{

}