#include <SMVector.h>

using namespace smtech1;

double SMVector::dist(SMVector other){
    return sqrt(((x - other.x) * (x - other.x)) + ((y - other.y) * (y - other.y)));
}

double SMVector::fastDist(SMVector other){
    return ((x - other.x) * (x - other.x)) + ((y - other.y) * (y - other.y));
}