//
//  Raycaster.cpp
//  smtech1
//
//  Created by Matt on 4/13/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#include "Raycaster.h"
using namespace smtech1;

// tan(30) or tan(fov/2)? Need to brush up on my trig
// 64 is wall size, 32 is player viewheight

Raycaster::Raycaster(uint32_t width, uint32_t height, uint32_t viewHeight, uint32_t fov, uint32_t wallsz) : width(width), height(height), viewHeight(viewHeight), fov(fov), planeDistance((width/2)/tan(fov/2)), columnSize(fov/width), gridSize(width/wallsz), gridHeight(height/wallsz), wallSize(wallsz) {
    
}

std::vector<SMLine> Raycaster::castLines(SMVector& position, double angle, std::vector<SMLine>& lines) {
    std::vector<SMLine> projectedLines;
    
    for (uint32_t ray=0; ray<width; ray+=columnSize) {
        for (uint32_t grid=0; grid<width; grid+=gridSize) {
            int Xa = wallSize/tan(angle);// x coordinate of ray intersection point ?? :->
            
            
        }
    }
    
    return projectedLines;
}