#include "SMLine.h"

using namespace smtech1;

bool SMLine::intersect(SMLine other, SMVector& intersect){
    double x1 = pt1.x;
    double x2 = pt2.x;
    double x3 = other.pt1.x;
    double x4 = other.pt2.x;
    double y1 = pt1.y;
    double y2 = pt2.y;
    double y3 = other.pt1.y;
    double y4 = other.pt2.y;

    double x12 = x1 - x2;
    double x34 = x3 - x4;
    double y12 = y1 - y2;
    double y34 = y3 - y4;

    double c = x12 * y34 - y12 * x34;

    if (fabs(c) < 0.01)
    {
        // No intersection
        return false;
    }
    else
    {
        // Intersection
        double a = x1 * y2 - y1 * x2;
        double b = x3 * y4 - y3 * x4;

        double x = (a * x34 - b * x12) / c;
        double y = (a * y34 - b * y12) / c;

        intersect = {x, y};

        if ((x < x1 && x < x2) || (x > x2 && x > x1)){
   
            return false;
        }

        if ((y < y1 && y < y2) || (y > y2 && y > y1)){

            return false;
        }

        return true;
    }
    return true;
}

// Comparator overload for heap creation
inline bool SMLine::operator< (const SMLine& rhs) {
    return std::tie(pt1.z, pt2.z) < std::tie(rhs.pt1.z, rhs.pt2.z);
}
