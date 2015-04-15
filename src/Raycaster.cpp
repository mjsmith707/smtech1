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

Raycaster::Raycaster(uint32_t width, uint32_t height, uint32_t viewHeight, double fov, uint32_t wallsz, SMVector& player) : width(width), height(height), viewHeight(viewHeight), fov(fov), planeDistance((width/2)/tan(fov/2)), columnSize(fov/width), gridSize(width/wallsz), gridHeight(height/wallsz), wallSize(wallsz), player(player) {
    planeDist = 30;
    debugLines.leftPlane = { { player.x + planeDist * -1.0 * sin(fov / 2.0), player.y - planeDist }, { player.x + planeDist * -1.0 * sin(fov / 2.0) * 100.0, player.y - planeDist * 100.0 }, 0xFFFF00 };
    debugLines.rightPlane = { { player.x + planeDist *  1.0 * sin(fov / 2.0), player.y - planeDist }, { player.x + planeDist *  1.0 * sin(fov / 2.0) * 100.0, player.y - planeDist * 100.0 }, 0xFF00FF };
    debugLines.projectionPlane = { { player.x + planeDist * -1.0 * sin(fov / 2.0), player.y - planeDist }, { player.x + planeDist * 1.0 * sin(fov / 2.0), player.y - planeDist }, 0x00FFFF };
}

std::vector<RaycastHit> Raycaster::castLines(SMVector& position, double angle, std::vector<SMLine>& lines) {
    std::vector<RaycastHit> projectedLines;

    for (int a = 0; a < width; a++){
        for (auto i : lines){
            SMLine toIntersect = { project(i.pt1, angle, position), project(i.pt2, angle, position), i.color };
            SMVector intersect;
            SMLine ray = { { player.x, player.y}, { player.x + planeDist * (-1.0 + 2.0 * ((double)a / (double)width)) * sin(fov / 2.0) * 100.0, player.y - planeDist * 100.0 }, 0xFFFF00 };

            if (toIntersect.intersect(ray, intersect)){

                SMVector vec = intersect;

                if (vec.y < player.y - planeDist){
                    SMLine line = toIntersect;
                    double dist = std::sqrt(std::pow((player.y - intersect.y), 2.0) + std::pow((player.x - intersect.x), 2.0));
                    projectedLines.push_back(RaycastHit{ vec, line, ray, dist, a });
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