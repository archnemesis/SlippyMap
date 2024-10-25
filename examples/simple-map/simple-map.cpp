//
// Simple SlippyMapWidget usage example. This example creates
// the bare minimum configuration to show a map from a tile
// server.
//

#include <QApplication>
#include <SlippyMap/SlippyMapWidget.h>
#include <SlippyMap/SlippyMapWidgetLayer.h>

using namespace SlippyMap;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //
    // Create and configure map widget.
    //
    SlippyMapWidget map;

    //
    // Set up the map tile layer.
    //
    SlippyMapWidgetLayer layer;
    layer.setName("OpenStreetMaps");
    layer.setDescription("Example OpenStreetMaps layer");
    layer.setTileUrl("https://tile.openstreetmap.org/%1/%2/%3.png");
    map.addLayer(&layer);

    //
    // Show the map.
    //
    map.show();

    return QApplication::exec();
}