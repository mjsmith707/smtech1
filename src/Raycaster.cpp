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

Raycaster::Raycaster(){

}

Raycaster::Raycaster(uint32_t width, uint32_t height, uint32_t viewHeight, double fov, uint32_t wallsz) : width(width), height(height), viewHeight(viewHeight), fov(fov), planeDistance((width/2)/tan(fov/2)), columnSize(fov/width), gridSize(width/wallsz), gridHeight(height/wallsz), wallSize(wallsz) {
    

}

std::vector<RaycastHit> Raycaster::castLines(SMVector& player, SMVector& position, double angle, std::vector<SMLine>& lines) {
    std::vector<RaycastHit> projectedLines;
    
    for (int a = 0; a < width; a++){
        for (auto i : lines){
            SMLine toIntersect = {  project(i.pt1, angle, position, player) , project(i.pt2, angle, position, player)};
            SMVector intersect;
            double dist = (double)(width / 2.0) * tan(fov);
            
            SMLine ray = { { player.x, player.y }, { player.x - 0.5 * width + width * a * (fov / (double)width), player.y - dist}, 0x00FF00 + a  * 10 % 255};
            if (toIntersect.intersect(ray, intersect)){
                SMVector vec = intersect;
                if (vec.y < player.y){
                    SMLine line = ray;
                    projectedLines.push_back(RaycastHit{ vec, line });
                }
            }
        }
    }
    
    return projectedLines;
}

SMVector Raycaster::project(const SMVector& vecta, const double angle, const SMVector& position, const SMVector& player){
    // Do some fancy vector math
   
        SMVector result;

        // offset position
        double x = vecta.x - position.x;
        double y = vecta.y - position.y;

        // camera transform
        result.x = x * sin(angle) - y * cos(angle);
        result.y = x * cos(angle) + y * sin(angle);

        result.x += player.x;
        result.y += player.y;

        return result;
}


