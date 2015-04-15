//
//  SMVector.h
//  smtech1
//

#ifndef __smtech1__SMVector__
#define __smtech1__SMVector__

// Vector struct
namespace smtech1 {
    typedef struct SMVector {
        double x;
        double y;
        double z;
        
        // Comparator overload for heap creation
        inline bool operator< (const SMVector& rhs) {
            return (z < rhs.z);
        }
    } SMVector;
    
}

#endif /* defined(__smtech1__SMVector__) */
