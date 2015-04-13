#ifndef __smtech1__Minimap__
#define __smtech1__Minimap__

#include <cmath>
#include <vector>
#include <cstdint>
#include "SMVector.h"

namespace smtech1 {
    typedef struct Minimap{

        // bottom left corner of minimap
        SMVector bl;

        // bottom right corner of minimap
        SMVector tr;
        
        // Player Dot Color
        uint32_t playercolor;
        
        // Bounding Box Color
        uint32_t boxcolor;
      
        SMVector project(const SMVector& vecta, const double angle, const SMVector& position, const SMVector& player);
    } Minimap;
}
#endif