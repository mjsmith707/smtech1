//
//  SMRenderer.h
//  smtech1
//

#ifndef __smtech1__SMRenderer__
#define __smtech1__SMRenderer__

#include <cstdint>
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
#include "Minimap.h"
#include "Trashcaster.h"

namespace smtech1 {
    class SMRenderer {
        private:
            // Chrono Millisecond typedef
            typedef std::chrono::duration<long, std::milli> msec;
        
            //typedef enum ClipCode {
            //    INSIDE = 0, LEFT = 1, RIGHT = 2, BOTTOM = 4, TOP = 8
            //} ClipCode;

            // Render thread and control
            std::thread renderThread;
            std::atomic<bool> renderRunning;

            // SDL pointers
            SDL_Window* window;
            SDL_Surface* screen;

            // Screen width/height
            const uint32_t width;
            const uint32_t height;
        
            // Some useful pi constants;
            const double pi = 3.14159265359;
            const double pi2 = 6.28318530718;
        
            // Fixed Player
            SMVector player;
        
            // Position
            SMVector position;
            double angle = 4.78319;

            // Bool to indicate whether the window has mouse grab
            bool mousemode = false;

            // Meshes to render
            std::vector<SMLine> lines;

            // Minimap
            Minimap minimap;
            bool mapactive;
            bool mapfullscreen;

            // Thread functions
            void threadinit();
            void render();
            void initMeshes();

            // Render functions
            inline void drawBlank();
            inline void drawPixel(int x, int y, uint32_t color);
            inline void drawLine(int x1, int y1, int x2, int y2, uint32_t color);
            inline void drawHud();
            inline void drawMap();

            // Input handler
            inline void getInput(SDL_Event& event);
        
            // Line clipping helper
            //ClipCode getClipping(double x, double y);

            // Vector functions
            inline double dotProduct(const SMVector& vecta, const SMVector& vectb);
            SMVector intersection(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);
            inline double crossProduct(double x1, double y1, double x2, double y2);
            SMVector normalize(const SMVector& vecta);
            SMVector project(const SMVector& vecta);

            Trashcaster trashcaster;

        public:
            SMRenderer(uint32_t width, uint32_t height);
            ~SMRenderer();

            void run();
    };
}

#endif /* defined(__smtech1__SMRenderer__) */
