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
#include "Raycaster.h"
#include "EasyBMP_BMP.h"
#include "EasyBMP_DataStructures.h"
#include "Sprite.h"
#include "MapLoader.h"

namespace smtech1 {
    class SMRenderer {
        private:
            // Chrono Millisecond typedef
            typedef std::chrono::duration<long, std::milli> msec;
            
            // hacky transparency in bmps, who needs color 0x0c0f08
            // color code 0x0c0f08
            const uint32_t fakealpha = 0x000b0b0b;    // CBA to fuck with pngs

            // Render thread and control
            std::thread renderThread;
            std::atomic<bool> renderRunning;

            // SDL pointers
            SDL_Window* window;
            SDL_Surface* screen;
            SDL_Renderer* renderer;
            
            // Screen width/height
            const uint32_t width;
            const uint32_t height;

            // texture w/h
            const uint32_t texHeight = 312;
            const uint32_t texWidth = 312;

            // player screen position, misnomer... always in the middle of the screen!
            SMVector playerPos;

            // Render functions
            inline void drawBlank();
            inline void drawPixel(int x, int y, uint32_t color);
            inline void drawLine(int x1, int y1, int x2, int y2, uint32_t color);
            inline void drawLine(SMLine line);
            inline void vLine(int x, int y1, int y2, uint32_t  color);
            inline void texVLine(int x, int y1, int y2, double dist, double fract, double len, double ssconst, int w, int h, BMP* texture);
            inline void drawHud();
            inline void drawMap();
            inline void drawSprites();
            inline void drawBMP(const uint32_t swidth, const uint32_t sheight, const uint32_t xrel, const uint32_t yrel, BMP* bmp);

            // these don't belong here
            /*
            // Vector functions
            inline double dotProduct(const SMVector& vecta, const SMVector& vectb);
            SMVector intersection(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);
            inline double crossProduct(double x1, double y1, double x2, double y2);
            SMVector normalize(const SMVector& vecta);
            SMVector project(const SMVector& vecta);
            */

            // graphics things
            inline double smoothstep(double min, double max, double val);
            inline uint32_t dim(uint32_t color, uint32_t amount);
            inline uint32_t brighten(uint32_t color, uint32_t amount);
            float Q_rsqrt(float number);

        public:
            SMRenderer(uint32_t width, uint32_t height);
            ~SMRenderer();
            // REFACTORTODO move most of these args into fields
            void render(std::vector<RaycastHit> intersections, uint32_t castgap, double angle, SMVector position, Raycaster raycaster, Texture* ceiling, Texture* floor);
    };
}

#endif /* defined(__smtech1__SMRenderer__) */
