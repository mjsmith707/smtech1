//
//  Raycaster.h
//  smtech1
//
//  Created by Matt on 4/13/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#ifndef smtech1_Raycaster_h
#define smtech1_Raycaster_h

#include <cstdint>
#include <cmath>
#include <vector>
#include "SMLine.h"

namespace smtech1 {
    class Raycaster {
        private:
            uint32_t width;
            uint32_t height;
            uint32_t viewHeight;
            uint32_t fov;
            uint32_t planeDistance;
            uint32_t columnSize;
            uint32_t gridSize;
            uint32_t gridHeight;
            uint32_t wallSize;
        
        public:
            Raycaster(uint32_t width, uint32_t height, uint32_t viewHeight, uint32_t fov, uint32_t wallsz);
            std::vector<SMVector> castLines(SMVector& position, double angle, std::vector<SMLine>& lines);
    };
}

#endif
