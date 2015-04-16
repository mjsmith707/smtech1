//
//  Minimap.h
//  smtech1
//

#include "Minimap.h"
using namespace smtech1;

SMVector Minimap::project(const SMVector& vect, const double angle, const SMVector& position, const SMVector& player){
    SMVector result;

    // offset position
    double x = vect.x - position.x;
    double y = vect.y - position.y;

    // scale based on minimap size / actual size
    x *= (tr.x - bl.x) / (player.x * 2.0);
    y *= (tr.y - bl.y) / (player.y * 2.0);

    // camera transform
    result.x = x * sin(angle) - y * cos(angle);
    result.y = x * cos(angle) + y * sin(angle);

    // center on minimap center
    result.x += bl.x + 0.5 * tr.x;
    result.y += bl.y + 0.5 * tr.y;

    if (result.x < 0){
        result.x = 0;
    }
    if (result.x > player.x * 2){
        result.x = player.x * 2 - 2;
    }
    if (result.y < 0){
        result.y = 0;
    }
    if (result.y > player.y * 2){
        result.y = player.y * 2 - 2;
    }

    return result;
}