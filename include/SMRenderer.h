//
//  SMRenderer.h
//  smtech1
//

#ifndef __smtech1__SMRenderer__
#define __smtech1__SMRenderer__

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <cmath>
#include <numeric>
#include <SDL2/SDL.h>
#include "SMLine.h"
#include "SMVector.h"

namespace smtech1 {
    class SMRenderer {
        private:
            // Probably useless
            typedef std::chrono::duration<long, std::milli> msec;

            // Render thread and control
            std::thread renderThread;
            std::atomic<bool> renderRunning;

            // SDL pointers
            SDL_Window* window;
            SDL_Surface* screen;

            // Screen width/height
            const Uint32 width;
            const Uint32 height;
        
            // Some useful pi constants;
            static constexpr double pi = 3.14159265359;
            static constexpr double pi2 = 6.28318530718;
        
            // Fixed Player
            SMVector player { static_cast<int>(width/2), static_cast<int>(height/2), 0 };
        
            // Position
            SMVector position { static_cast<int>(width/2), static_cast<int>(height/2), 0 };
            double angle = 0;

            // Bool to indicate whether the window has mouse grab
            bool mousemode = false;

            // Meshes to render
            std::vector<SMLine> lines;

            // Thread functions
            void threadinit();
            void render();
            void initMeshes();

            // Render functions
            inline void drawBlank();
            inline void drawPixel(int x, int y, Uint32 color);
            inline void drawLine(int x1, int y1, int x2, int y2, Uint32 color);

            // Input handler
            inline void getInput(SDL_Event& event);

            // Vector functions
            int dotProduct(const SMVector& vecta, const SMVector& vectb);
        SMVector intersection(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
            int crossProduct(int x1, int y1, int x2, int y2);
            SMVector normalize(const SMVector& vecta);
            SMVector project(const SMVector& vecta);

        public:
            SMRenderer(Uint32 width, Uint32 height);
            ~SMRenderer();
            void run();
    };
}

#endif /* defined(__smtech1__SMRenderer__) */
