#ifndef __smtech1__Map__
#define __smtech1__Map__

#include <vector>

#include "SMLine.h"
#include "SMThing.h"


namespace smtech1{
    class Map{
        private:
            
        public:
            std::vector<SMLine> lines;
            std::vector<SMThing> things;
            Texture* floor;
            Texture* ceiling;
    };
}

#endif