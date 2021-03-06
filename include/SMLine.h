//
//  SMLine.h
//  smtech1
//

#ifndef __smtech1__SMLine__
#define __smtech1__SMLine__

#include <cstdint>
#include <cmath>
#include <iostream>
#include "SMVector.h"
#include "Texture.h"


// LineDef
namespace smtech1 {
    typedef struct SMLine {
            SMVector pt1;
            SMVector pt2;
            uint32_t color;

            // for wallsegs
            Texture* texture;
            bool intersect(SMLine& other, SMVector& intersect);
            SMVector mid();
    } SMLine;
}

#endif /* defined(__smtech1__SMLine__) */
