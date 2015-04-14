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

    // init this somewhere else
    planeDist = (double)(width / 2.0) * tan(fov);
    
    for (int a = 0; a < width; a++){
        for (auto i : lines){

            // for removing view distortion
            // almost not quite there
            //double beta = ( -1.0 * 1.57079632679 / 2.0 ) + ((double)a) * (fov / (double)width);

            SMLine toIntersect = {  project(i.pt1, angle, position, player) , project(i.pt2, angle, position, player), i.color};
            SMVector intersect;
            
            SMLine ray = { { player.x, player.y }, { player.x - 0.5 * width + width * a * (fov / (double)width), player.y - planeDist}, 0x0f0f0f + a  * 10 % 255};
            if (toIntersect.intersect(ray, intersect)){
                SMVector vec = intersect;
                if (vec.y < player.y){
                    SMLine line = toIntersect;
                    double dist = std::sqrt(std::pow((position.y - intersect.y), 2.0) + std::pow((position.x - intersect.x), 2.0));

                    // removing view distortion
                    //dist *= cos(beta);
                    projectedLines.push_back(RaycastHit{ vec, line, dist});
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


