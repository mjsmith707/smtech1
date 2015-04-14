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
#include <iostream>
#include "SMLine.h"

namespace smtech1 {
    class Raycaster {
        private:
        SMVector direction;
        SMVector plane;
        
        uint32_t width = 320;
        uint32_t height = 240;
        int world[640][480];
        void drawLineToWorld(int x1, int y1, int x2, int y2, uint32_t color);
        public:
            Raycaster();
            void loadMap(std::vector<SMLine>& lines);
            std::vector<SMLine> raycast(SMVector pos, double angle);
    };
}

#endif
