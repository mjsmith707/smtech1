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
#include "EasyBMP_BMP.h"
#include "EasyBMP_DataStructures.h"
#include "Sprite.h"
#include "SMThing.h"

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
            const uint32_t texHeight = 64;
            const uint32_t texWidth = 64;
        
            // Raycaster Stuff
            uint32_t viewHeight;
            double fov;
            uint32_t planeDistance;
            uint32_t columnSize;
            uint32_t gridSize;
            uint32_t gridHeight;
            uint32_t wallSize;
            SMVector player;
            
            typedef struct RaycastHit {
                SMVector vec;
                SMLine line;
                SMLine ray;
                double dist;
                int x;
                inline bool operator< (const RaycastHit& rhs) const {
                    if (x != rhs.x){
                        return x < rhs.x;
                    }
                    return dist > rhs.dist;
                }
            } RaycastHit;
        
            struct debugLines {
                SMLine projectionPlane;
                SMLine leftPlane;
                SMLine rightPlane;
            } debugLines;

            // bad but I want things to work :>
            // more bad more good
            uint32_t castGap;
            double planeDist;

            // Render functions
            inline void drawBlank();
            inline void drawPixel(int x, int y, uint32_t color);
            inline void drawLine(int x1, int y1, int x2, int y2, uint32_t color);
            inline void drawLine(SMLine line);
            inline void vLine(int x, int y1, int y2, uint32_t  color);
            inline void texVLine(int x, int y1, int y2, double dist, double fract, double len, double ssconst, int w, int h, BMP* texture);
            std::vector<RaycastHit> castLines(SMVector& position, double angle, std::vector<SMLine>& lines);
            inline SMVector project(const SMVector& vecta, const double angle, const SMVector& position);
            inline void drawHud();
            inline void drawMap();
            inline void drawSprites();
            inline void drawBMP(const uint32_t swidth, const uint32_t sheight, const uint32_t xrel, const uint32_t yrel, BMP* bmp);

            // graphics things
            inline double smoothstep(double min, double max, double val);
            inline uint32_t dim(uint32_t color, uint32_t amount);
            inline uint32_t brighten(uint32_t color, uint32_t amount);
            float Q_rsqrt(float number);

        public:
            SMRenderer(uint32_t width, uint32_t height);
            void init(SDL_Window*& window, SDL_Surface*& screen, SDL_Renderer*& renderer);
            // REFACTORTODO move most of these args into fields
            void render(double angle, SMVector& position, std::vector<SMLine>& lines, Texture*& ceiling, Texture*& floor, std::vector<SMThing>& things);
    };
}

#endif /* defined(__smtech1__SMRenderer__) */
