//
//  Trashcaster.h
//  smtech1
//
//  Created by Matt on 4/13/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#ifndef smtech1_Trashcaster_h
#define smtech1_Trashcaster_h

#include <cstdint>
#include <cmath>
#include <vector>
#include <iostream>
#include "SMLine.h"

namespace smtech1 {
    class Trashcaster {
        private:
        SMVector direction;
        SMVector plane;
        
        uint32_t width;
        uint32_t height;
        int world[640][480];
        void drawLineToWorld(int x1, int y1, int x2, int y2, uint32_t color);
        public:
            Trashcaster(uint32_t width, uint32_t height);
            void loadMap(std::vector<SMLine>& lines);
            std::vector<SMLine> raycast(SMVector pos, double angle);
    };
}

#endif
