//
//  Trashcaster.cpp
//  smtech1
//

#include "Trashcaster.h"
using namespace smtech1;

// tan(30) or tan(fov/2)? Need to brush up on my trig
// 64 is wall size, 32 is player viewheight

Trashcaster::Trashcaster(uint32_t width, uint32_t height) : width(width), height(height) {
    
}

// Load lines into a flat 2d array where -1 is a boundary otherwise a color
void Trashcaster::loadMap(std::vector<SMLine>& lines) {
    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            world[x][y] = 0;
        }
    }
    
    for (int x=0; x<width; x++) {
        world[x][0] = 1;
        world[x][height-1] = 1;
    }
    
    for (int y=0; y<height; y++) {
        world[0][y] = 1;
        world[width-1][y] = 1;
    }
    
    for (auto i : lines) {
        drawLineToWorld(i.pt1.x, i.pt1.y, i.pt2.x, i.pt2.y, i.color);
    }
}

void Trashcaster::drawLineToWorld(int x1, int y1, int x2, int y2, uint32_t color) {
    // More debug bounds checking
    // Stop that long ass line draw
    if ((abs(x1) > 10000000) || (abs(y1) > 10000000) || (abs(x2) > 10000000) || (abs(y2) > 10000000)) {
        return;
    }
    
    const bool steep = (abs(y2 - y1) > abs(x2 - x1));
    if (steep) {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }
    
    if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    
    const float dx = x2 - x1;
    const float dy = abs(y2 - y1);
    
    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;
    
    const int maxX = (int)x2;
    
    for (int x=(int)x1; x<maxX; x++) {
        if(steep) {
            world[(int)y][(int)x] = color;
        }
        else {
            world[(int)x][(int)y] = color;
        }
        
        error -= dy;
        if (error < 0) {
            y += ystep;
            error += dx;
        }
    }
}

std::vector<SMLine> Trashcaster::raycast(SMVector pos, double angle) {
    std::vector<SMLine> lines;
    // Compute direction vector
    direction.x = -cos(angle);
    direction.y = -sin(angle);
    
    plane.x = 0.0;  //
    plane.y = 1.1519173063; // 2*atan(0.66/1.0) = 66 deg;
    
    for (int x=0; x<width; x++) {
        double camerax = 2*x/double(width) -1;
        double rayposx = pos.x;
        double rayposy = pos.y;
        double raydirx = direction.x + plane.x * camerax;
        double raydiry = direction.y + plane.y * camerax;
        
        int mapx = int(rayposx);
        int mapy = int(rayposy);
        
        double sidedistx;
        double sidedisty;
        
        double deltadistx = sqrt(1 + (raydiry*raydiry) / (raydirx*raydirx));
        double deltadisty = sqrt(1 + (raydirx*raydirx) / (raydiry*raydiry));
        
        double perpwalldist;
        
        int stepx;
        int stepy;
        
        int hit = 0;
        int side = 1;
        
        if (raydirx < 0) {
            stepx = -1;
            sidedistx = (rayposx - mapx) * deltadistx;
        }
        else {
            stepx = 1;
            sidedistx = (mapx + 1.0 - rayposx) * deltadistx;
        }
        if (raydiry < 0) {
            stepy = -1;
            sidedisty = (rayposy - mapy) * deltadisty;
        } else {
            stepy = 1;
            sidedisty = (mapy + 1.0 - rayposy) * deltadisty;
        }
        
        while (hit == 0) {
            if (sidedistx < sidedisty) {
                sidedistx += deltadistx;
                mapx += stepx;
                side = 0;
            }
            else {
                sidedisty += deltadisty;
                mapy += stepy;
                side = 1;
            }
            
            if (world[mapx][mapy] > 0) {
                hit = 1;
            }
        }
        
        if (side == 0) {
            perpwalldist = fabs((mapx - rayposx + (1 - stepx) / 2) / raydirx);
        }
        else {
            perpwalldist = fabs((mapy - rayposy + (1 - stepy) / 2) / raydiry);
        }
        
        int lineheight = abs(int(height / perpwalldist));
        
        int drawstart = -lineheight / 2 + height / 2;
        if (drawstart < 0) {
            drawstart = 0;
        }
        int drawend = lineheight / 2 + height / 2;
        if (drawend >= height) {
            drawend = height -1;
        }
        
        SMLine line;
        SMVector pt1;
        SMVector pt2;
        if (side == 1) {
            line.color = world[mapx][mapy]/2;
        }
        
        pt1.x = x;
        pt1.y = drawstart;
        pt2.x = x;
        pt2.y = drawend;
        
        line.pt1 = pt1;
        line.pt2 = pt2;
        
        lines.push_back(line);
    }
    
    return lines;
}