//
//  SMVector.h
//  smtech1
//

#ifndef __smtech1__SMVector__
#define __smtech1__SMVector__

#include <cmath>

// Vector struct
namespace smtech1 {
    typedef struct SMVector {
        double x;
        double y;
        double z;
        double dist(SMVector other);
        double fastDist(SMVector other);
    } SMVector;
    
}

#endif /* defined(__smtech1__SMVector__) */
