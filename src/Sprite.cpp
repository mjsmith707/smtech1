//
//  Sprite.cpp
//  smtech1
//
//  Created by Matt on 4/14/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#include "Sprite.h"
using namespace smtech1;

Sprite::Sprite(std::vector<std::string> filenames, uint32_t delay, uint32_t xrel, uint32_t yrel, bool isTexture, bool loop) : playing(loop), staticview(true), cycle(0), delaycount(0), cycledelay(delay), xrel(xrel), yrel(yrel), loop(loop) {
    for (auto i : filenames) {
        try {
            BMP* image = new BMP();
            image->ReadFromFile(i.c_str());
            images.push_back(std::ref(image));
        }
        catch (std::exception e) {
            throw e;
        }
    }
    
    texture = isTexture;

    if (images.size() == 0) {
        throw std::runtime_error("Unable to load any sprite files");
    }
    
}

void Sprite::playAnimation() {
    playing = true;
    staticview = false;
    cycle = 0;
    delaycount = 0;
}

bool Sprite::isTexture(){
    return texture;
}

bool Sprite::isPlaying() {
    return playing;
}

bool Sprite::isStatic() {
    return staticview;
}

uint32_t Sprite::getWidth() {
    return images.at(cycle)->TellWidth();
}

uint32_t Sprite::getHeight() {
    return images.at(cycle)->TellHeight();
}

BMP* Sprite::cycleAnimation() {
    if (delaycount%cycledelay != 0) {
        delaycount++;
        return images.at(cycle);
    }
    
    BMP* image = images.at(cycle);
    cycle++;
    delaycount++;
    
    if (cycle >= images.size()) {
        cycle = 0;
        delaycount = 0;
        if (!loop) {
            playing = false;
            staticview = true;
        }
    }
    
    return image;
}

BMP* Sprite::staticView() {
    BMP* image = images.at(0);
    return image;
}
