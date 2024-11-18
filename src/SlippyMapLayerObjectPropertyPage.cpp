#include <SlippyMap/SlippyMapLayerObjectPropertyPage.h>
#include <SlippyMap/SlippyMapWidgetMarker.h>
#include <SlippyMap/SlippyMapLayer.h>

#include <QComboBox>


SlippyMapLayerObjectPropertyPage::SlippyMapLayerObjectPropertyPage(const SlippyMapLayerObject::Ptr& object, QWidget *parent) :
    QWidget(parent)
{
    m_object = object;
}

SlippyMapLayerObjectPropertyPage::~SlippyMapLayerObjectPropertyPage()
{

}