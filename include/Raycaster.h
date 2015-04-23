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
#include <algorithm>
#include "SMLine.h"


namespace smtech1 {
    typedef struct RaycastHit {
        SMVector vec;
        SMLine line;
        SMLine ray;
        double dist;
        int x;
        
        inline bool operator< (const RaycastHit& rhs) const {
            if (x != rhs.x){
                return x < rhs.x;
            }
            return dist > rhs.dist;
        }
        
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
            
            SMVector player;
            
            
        public:
            Raycaster();
            Raycaster(uint32_t width, uint32_t height, uint32_t viewHeight, double fov, uint32_t wallsz, SMVector& player, uint32_t gap);
            std::vector<RaycastHit> castLines(SMVector& position, double angle, std::vector<SMLine>& lines);
            
            struct debugLines {
                SMLine projectionPlane;
                SMLine leftPlane;
                SMLine rightPlane;
            } debugLines;

            // bad but I want things to work :>
            // more bad more good
            uint32_t castGap;
            double planeDist;

            // Do some fancy vector math
            SMVector project(const SMVector& vecta, const double angle, const SMVector& position);
    };

    
}

#endif
