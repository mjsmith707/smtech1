//
//  MapLoader.cpp
//  smtech1
//
//  Created by Matt on 4/15/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#include "MapLoader.h"
using namespace smtech1;

MapLoader::MapLoader() {
}

std::vector<SMLine> MapLoader::loadMap(std::string& filename) {
    std::vector<SMLine> map;
    size_t count = 0;
    std::ifstream fhandle;
    fhandle.open(filename);
    if (!fhandle.is_open()) {
        throw std::runtime_error("Failed to load map: " + filename);
    }
    else {
        std::string line = "";
        std::string temp = "";
        char i;
        double vector[4] {0.0,0.0,0.0,0.0};
        uint32_t color = 0;
        try {
            while (fhandle.good() && (fhandle >> i)) {
                if (count > 3) {
                    if (i != ',') {
                        temp += i;
                        continue;
                    }
                    else {
                        color = (uint32_t)std::stoi(temp);
                        
                        SMVector vecta {vector[0],vector[1],0.0};
                        SMVector vectb {vector[2],vector[3],0.0};
                        SMLine lineseg {vecta, vectb, color};
                        map.push_back(lineseg);
                        
                        temp = "";
                        vector[0] = 0.0;
                        vector[1] = 0.0;
                        vector[2] = 0.0;
                        vector[3] = 0.0;
                        color = 0;
                        count = 0;
                    }
                }
                else if (i == ',') {
                    vector[count] = std::stod(temp);
                    temp = "";
                    count++;
                }
                else {
                    temp += i;
                }
            }
        }
        catch (std::exception e) {
            throw e;
        }
    }
    return map;
}
