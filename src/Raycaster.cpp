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

Raycaster::Raycaster(uint32_t width, uint32_t height, uint32_t viewHeight, double fov, uint32_t wallsz, SMVector& player, uint32_t gap) : width(width), height(height), viewHeight(viewHeight), fov(fov), planeDistance((width/2)/tan(fov/2)), columnSize(fov/width), gridSize(width/wallsz), gridHeight(height/wallsz), wallSize(wallsz), player(player) {
    planeDist = 30;
    debugLines.leftPlane = { { player.x + planeDist * -1.0 * sin(fov / 2.0), player.y - planeDist }, { player.x + planeDist * -1.0 * sin(fov / 2.0) * 100.0, player.y - planeDist * 100.0 }, 0xFFFF00 };
    debugLines.rightPlane = { { player.x + planeDist *  1.0 * sin(fov / 2.0), player.y - planeDist }, { player.x + planeDist *  1.0 * sin(fov / 2.0) * 100.0, player.y - planeDist * 100.0 }, 0xFF00FF };
    debugLines.projectionPlane = { { player.x + planeDist * -1.0 * sin(fov / 2.0), player.y - planeDist }, { player.x + planeDist * 1.0 * sin(fov / 2.0), player.y - planeDist }, 0x00FFFF };
    castGap = gap;
}

std::vector<RaycastHit> Raycaster::castLines(SMVector& position, double angle, std::vector<SMLine>& lines) {
    std::vector<RaycastHit> projectedLines;
    // iterate through all the rays to cast
    // castgap defines the distance between raycasts
    for (int a = 0; a < width / castGap; a++){
        // go through each line on the map (this is where BSP would come in)
        for (auto i : lines){
            // the line to intersect; project the line so it's relative to our orientation
            SMLine toIntersect = { project(i.pt1, angle, position), project(i.pt2, angle, position), i.color };
            
            // intersection point gets put in here, if any intersection happens
            SMVector intersect;

            // the ray to cast
            // start casting from player's position outwards, so pt1 = {player.x, player.y}
            // pt2.x: 
            // transform player's x outwards based on current angle
            //                        -0.5 -> 0.5
            //                                       ratio of current position:total 
            //                                       positions so, a % of progress
            //                                                                                 trig stuff,
            //                                                                                 * 2 earlier so 
            //                                                                                 / 2 now
            //                                                                                                 "ray", just sent really far basically
            //                                                                                                 this multiplier doesn't really matter
            //                                                                                                 since smline::intersect checking broken
            // player.x + planeDist * (-1.0 + 2.0 * ((double)(a * castGap) / (double)width)) * sin(fov / 2.0) * 100.0
            //
            // pt2.y is just simple extension of the ray
            SMLine ray = { { player.x, player.y }, { player.x + planeDist * (-1.0 + 2.0 * ((double)(a * castGap) / (double)width)) * sin(fov / 2.0) * 100.0, player.y - planeDist * 100.0 }, 0xFFFF00 };

            // if there was an intersection
            if (toIntersect.intersect(ray, intersect)){
                SMVector vec = intersect;
                // don't count itersections behind the player
                if (vec.y < player.y - planeDist){

                    // the line we actually intersected
                    SMLine line = toIntersect;
                    
                    // dist to intersection point (this doesn't suffer from the 'fisheye' effect because casting 
                    // happens from the plane (planeDist) instead of from player, basically have a 2d frustum
                    double dist = std::sqrt(std::pow((player.y - intersect.y), 2.0) + std::pow((player.x - intersect.x), 2.0));

                    projectedLines.push_back(RaycastHit{ vec, line, ray, dist, static_cast<int>((a * castGap)) });
                    std::push_heap(projectedLines.begin(), projectedLines.end());
                }
            }
        }
    }

    return projectedLines;
}

SMVector Raycaster::project(const SMVector& vecta, const double angle, const SMVector& position){
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