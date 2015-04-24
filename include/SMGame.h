#ifndef __smtech1__SMGame__
#define __smtech1__SMGame__

#include "SMRenderer.h"
#include "AssetLoader.h"
#include "MapLoader.h"
#include "Map.h"

namespace smtech1{
    class SMGame {
        private:
            // chrono ms typedef
            typedef std::chrono::duration<long, std::milli> msec;

            // Render thread and control
            std::thread renderThread;
            std::atomic<bool> renderRunning;

            // SDL pointers
            SDL_Window* window;
            SDL_Surface* screen;
            SDL_Renderer* renderer;

            // Some useful pi constants;
            const double pi = 3.14159265359;
            const double pi2 = 6.28318530718;

            // input handler
            inline void getInput(SDL_Event& event);

            // thread functions
            void threadinit();
            void game();
            void initMeshes();
            void initSprites();

            // screen width/height
            const uint32_t width;
            const uint32_t height;

            // player things
            SMVector player;
            SMVector position;
            double angle = 0.01;
            double speed = 6.0;
            
            // indicates whether window has mouse focus
            bool mousemode = false;

            // for displaying things
            SMRenderer smRenderer;
            Raycaster raycaster;
            
            // loading things
            AssetLoader assetLoader;
            MapLoader mapLoader;
            
            // sprite manager, for things that aren't textures
            SpriteManager spriteManager;

            // active map
            Map map;

            // minimap
            Minimap minimap;
            bool mapactive;
            bool mapfullscreen;

        public:
            SMGame(uint32_t width, uint32_t height);
            ~SMGame();

            void run();
    };
}

#endif