#include "Texture.h"

using namespace smtech1;

Texture::Texture(){
    staticView = new BMP();
}

Texture::Texture(std::string textureName){
    try {
        staticView = new BMP();
        staticView->ReadFromFile((textureName + ".bmp").c_str());
    }
    catch (std::exception e) {
        throw e;
    }
}