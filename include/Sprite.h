//
//  Sprite.h
//  smtech1
//
//  Created by Matt on 4/14/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#ifndef smtech1_Sprite_h
#define smtech1_Sprite_h

#include <vector>
#include <string>
#include <cstdint>
#include <fstream>
#include "EasyBMP.h"

namespace smtech1 {
    class Sprite {
    private:
        std::vector<BMP*> images;
        uint32_t cycle;
        uint32_t cycledelay;
        uint32_t delaycount;
        bool playing;
        bool staticview;
        bool loop;
    public:
        uint32_t xrel;
        uint32_t yrel;
        Sprite(std::vector<std::string> filenames, uint32_t delay, uint32_t xrel, uint32_t yrel, bool loop);
        void playAnimation();
        bool isPlaying();
        bool isStatic();
        uint32_t getWidth();
        uint32_t getHeight();
        BMP* cycleAnimation();
        BMP* staticView();
    };
}

#endif
