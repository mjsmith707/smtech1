#ifndef __smtech1__Minimap__
#define __smtech1__Minimap__

#include <vector>
#include "SMVector.h"

namespace smtech1 {
    typedef struct Minimap{

        // bottom left corner of minimap
        SMVector bl;

        // bottom right corner of minimap
        SMVector tr;
      
        SMVector project(const SMVector& vecta, const double angle, const SMVector& position, const SMVector& player);
    } Minimap;
}
#endif