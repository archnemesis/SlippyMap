//
// Created by Robin on 10/31/2024.
//

#ifndef OSMEXPLORER_SLIPPYMAPANIMATEDLAYER_H
#define OSMEXPLORER_SLIPPYMAPANIMATEDLAYER_H

#include "SlippyMapWidgetLayer.h"

namespace SlippyMap
{
    class SlippyMapAnimatedLayer : public SlippyMapWidgetLayer
    {
        Q_OBJECT
    public:
        explicit SlippyMapAnimatedLayer(QObject *parent = nullptr);
        virtual void update() = 0;
        virtual void nextFrame() = 0;
        virtual void previousFrame() = 0;
    };
}

#endif //OSMEXPLORER_SLIPPYMAPANIMATEDLAYER_H
