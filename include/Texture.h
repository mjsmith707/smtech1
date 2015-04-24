#ifndef __smtech1__Texture__
#define __smtech1__Texture__

#include "EasyBMP.h"
#include "EasyBMP_BMP.h"

namespace smtech1{
    class Texture{
        public:
            BMP* staticView;
            Texture();
            Texture(std::string);
    };
}

#endif;