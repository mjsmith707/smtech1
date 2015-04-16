//
//  MapLoader.h
//  smtech1
//
//  Created by Matt on 4/15/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#ifndef smtech1_MapLoader_h
#define smtech1_MapLoader_h

#include <cstdint>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <string>
#include "SMLine.h"
#include "SMVector.h"

namespace smtech1 {
    class MapLoader {
        private:
        
    public:
        MapLoader();
        std::vector<SMLine> loadMap(std::string& filename);
    };
}

#endif
