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
    typedef struct RaycastHit {
        SMVector vec;
        SMLine line;
        double dist;
    } RaycastHit;

    class Raycaster {
        private:
            uint32_t width;
            uint32_t height;
            uint32_t viewHeight;
            double fov;
            uint32_t planeDistance;
            uint32_t columnSize;
            uint32_t gridSize;
            uint32_t gridHeight;
            uint32_t wallSize;
            
            // Do some fancy vector math
            SMVector project(const SMVector& vecta, const double angle, const SMVector& position, const SMVector& player);
                
        public:
            Raycaster();
            Raycaster(uint32_t width, uint32_t height, uint32_t viewHeight, double fov, uint32_t wallsz);
            std::vector<RaycastHit> castLines(SMVector& player, SMVector& position, double angle, std::vector<SMLine>& lines);
            
            // bad but I want things to work :>
            double planeDist;
    };

    
}

#endif
