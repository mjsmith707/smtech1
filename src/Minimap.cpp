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

    // make sure we don't draw outside of minimap bounds
    if (result.x < bl.x){
        result.x = bl.x;
    }
    else if (result.x > tr.x){
        result.x = tr.x;
    }

    if (result.y < bl.y){
        result.y = bl.y;
    }
    else if (result.y > tr.y){
        result.y = tr.y;
    }

    return result;
}