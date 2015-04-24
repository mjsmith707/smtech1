#ifndef __smtech1__AssetLoader__
#define __smtech1__AssetLoader__

#include "SpriteManager.h"
#include "Map.h"

namespace smtech1{
    class AssetLoader{
        private:
        public:
            void loadAssets(std::string mapFile, SpriteManager spriteManager, Map& map);
    };
}

#endif