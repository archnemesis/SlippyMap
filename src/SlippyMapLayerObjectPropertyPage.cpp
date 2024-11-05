#include <SlippyMap/SlippyMapLayerObjectPropertyPage.h>
#include <SlippyMap/SlippyMapWidgetMarker.h>
#include <SlippyMap/SlippyMapLayer.h>

#include <QComboBox>


SlippyMapLayerObjectPropertyPage::SlippyMapLayerObjectPropertyPage(const SlippyMapLayerObject::Ptr& object) :
    QWidget()
{
    m_object = object;
}

SlippyMapLayerObjectPropertyPage::~SlippyMapLayerObjectPropertyPage()
{

}