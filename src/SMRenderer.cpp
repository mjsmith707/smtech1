//
//  SMRenderer.cpp
//  smtech1
#include "SMRenderer.h"

using namespace smtech1;

// Initialize thread controls, width, height
SMRenderer::SMRenderer(uint32_t width, uint32_t height) : width(width), height(height) {
    playerPos = SMVector{ width / 2.0, height / 2.0, 0 };
}

SMRenderer::SMRenderer(): width(0), height(0){
    std::cout << "Shouldn't be called" << std::endl;
}

// grab SDL things from the game
void SMRenderer::init(SDL_Window* w, SDL_Surface* s, SDL_Renderer* r){
    window = w;
    screen = s;
    renderer = r;
}

// Main render thread loop function
void SMRenderer::render(std::vector<RaycastHit> intersections, uint32_t castgap, double angle, SMVector position, Raycaster raycaster, Texture* ceiling, Texture* floor, std::vector<SMThing> things) {

    int lastDrawnX = -1;

    // zbuffer for properly drawing things
    std::vector<double> zbuffer;
    zbuffer.resize(width);

    while (!intersections.empty()) {
        RaycastHit i = intersections.front();
        std::pop_heap(intersections.begin(), intersections.end());
        intersections.pop_back();

        if (i.x == lastDrawnX){
            continue;
        }
        else {
            lastDrawnX = i.x;
            zbuffer[i.x] = i.dist;
        }
        
        double lineh = std::abs(((double)height / (double)i.dist));

        lineh *= texHeight;

        double drawStart = -lineh / 2.0 + (double)height / 2.0;
        double drawEnd = lineh / 2.0 + (double)height / 2.0;

        if (drawStart < 0) drawStart = 0;
        if (drawEnd >= height) drawEnd = height - 1;

        uint32_t pxWidth = (uint32_t)lineh / texHeight;
        uint32_t numDrawn = 0;
        uint32_t currPx = i.x % texWidth;

        // for texture mapping
        double len = (i.line.pt1.dist(i.line.pt2));
        double fract = i.line.pt1.dist(i.vec) / len;

        // this works, but it loses a lot of accuracy along the edges
        //double fract = i.line.pt1.fastDist(i.vec) / (i.line.pt1.fastDist(i.line.pt2));

        // smoothstep constant to dim by
        // pretty arbitrary, have to play around with the values a lot to get things that look good
        // especially depending on the current texture darkness
        double ssconst = 500.0 * smoothstep(300.0, 800.0, i.dist);

        for (uint32_t d = 0; d < castgap; d++){
            // monocolored, shaded walls
            //vLine(i.x + d, drawStart, drawEnd, dim(i.line.color, 200.0 * smoothstep(0.0, 500.0, i.dist)));

            // textured walls
            // TODO fix shading factor
            texVLine(i.x + d, (uint32_t)drawStart, (uint32_t)drawEnd, i.dist, fract, len, ssconst, texWidth, texHeight, i.line.texture->staticView);

            // monocolor roof
            //vLine(i.x + d, 0, drawStart, 0x111111);

            // monocolor floor
            //vLine(i.x + d, drawEnd, height, 0x020202);

            // plane casting for ceil&floor
            double planeXWall, planeYWall;

            // 'unproject' so we're casting against the actual 'map axes' and not relative
            // to the player's position, which stays constant all the time
            SMVector transv = raycaster.project(SMVector{ i.vec.x, i.vec.y, 0 }, -angle, playerPos);

            planeXWall = transv.x;
            planeYWall = transv.y;

            double distWall, currentDist;

            distWall = i.dist;

            //draw the floor from drawEnd to the bottom of the screen
            for (uint32_t y = (uint32_t)drawEnd + 1; y < height; y++)
            {
                // TODO cache this
                currentDist = height / (2.0 * y - height);

                // TODO optimize away with some bitshifts
                double weight = currentDist / distWall;

                // player's position is always (player.x, player.y), aka (width/2, height/2) since the
                // world is transformed and moved around the player instead of the other way around
                double currentFloorX = weight * planeXWall + (1.0 - weight) * playerPos.x;
                double currentFloorY = weight * planeYWall + (1.0 - weight) * playerPos.y;

                int floorTexX, floorTexY;
                floorTexX = int(currentFloorX * texWidth - position.x) % texWidth;
                floorTexY = int(currentFloorY * texHeight - position.y) % texHeight;

                // from here, draw the pixels
                // REFACTORTODO floor/ceil texture references
                // REFACTORTODO get pixels in 'our format'
                RGBApixel pix = ceiling->staticView->GetPixel(floorTexX, floorTexY);

                // TODO sprite.pixel(x, y) returns uint32_t with all of these transforms applied
                uint32_t pxcfloor = (unsigned char)pix.Alpha << 24;
                pxcfloor += ((unsigned char)pix.Red << 16);
                pxcfloor += ((unsigned char)pix.Green << 8);
                pxcfloor += ((unsigned char)pix.Blue);

                pix = floor->staticView->GetPixel(floorTexX, floorTexY);
                uint32_t pxcroof = (unsigned char)pix.Alpha << 24;
                pxcroof += ((unsigned char)pix.Red << 16);
                pxcroof += ((unsigned char)pix.Green << 8);
                pxcroof += ((unsigned char)pix.Blue);
                
                // magic numbers
                // TODO optimize, looks nice for now though
                uint32_t dimfact = uint32_t(500.0 * (smoothstep(0.0, 12.0, currentDist)));
                pxcfloor = dim(pxcfloor, dimfact);
                pxcroof = dim(pxcroof, dimfact);

                drawPixel(i.x + d, y, pxcfloor);
                drawPixel(i.x + d, height - y, pxcroof);
            }
        }
    }

    for (auto t : things){
        //std::cout << t.pos.x << " " << t.pos.y << std::endl;
        SMVector projected = raycaster.project(t.pos, angle, position);
        double dist = projected.dist(position);
       
        double tx = projected.x;
        double ty = projected.y;
        if ((tx > 0 || tx < width - 2) && projected.y < height / 2)
        vLine(tx, 1, height - 1, 0xff0000);
        
    }

    // Draw HUD Elements
    drawHud();
    SDL_UpdateWindowSurface(window);

}


// Textured wall vline
inline void SMRenderer::texVLine(int x1, int y1, int y2, double dist, double fract, double len, double ssconst, int w, int h, BMP* texture) {
#if defined(__SNAIL__)
    drawLine(x1, y1, x1, y2, color);
#else
    int wallh = y2 - y1;

    for (int i = 0; i < wallh; i++){
        // do something about these casts

        // TODO tiling check, vs constant texture
        RGBApixel pix = texture->GetPixel((uint32_t)((uint32_t)(fract * len) % w), (int)(((double)i / (double)wallh) * h));

        // consolidate into oneliner
        uint32_t pxc = (unsigned char)pix.Alpha << 24;
        pxc += ((unsigned char)pix.Red << 16);
        pxc += ((unsigned char)pix.Green << 8);
        pxc += ((unsigned char)pix.Blue);

        pxc = dim(pxc, uint32_t(ssconst));

        drawPixel(x1, y1 + i, pxc);
    }
#endif
}

// vertical pixel line of monocolor
inline void SMRenderer::vLine(int x1, int y1, int y2, uint32_t color) {
    #if defined(__SNAIL__)
        drawLine(x1, y1, x1, y2, color);
    #else
        if (y1 > y2) {
            std::swap(y1, y2);
        }
    
        for (int y=y1; y<y2; y++) {
            drawPixel(x1, y, color);
        }
    #endif
}

// Bresenham line algorithm for _all_ octants
// http://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C.2B.2B
// Wasn't for lack of trying..
inline void SMRenderer::drawLine(int x1, int y1, int x2, int y2, uint32_t color) {
    #if defined(__SNAIL__)
        SDL_SetRenderDrawColor(renderer, (color & 0xff0000) >> 16, (color & 0x00ff00) >> 8 , color & 0x0000ff, 0xff);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    #else
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
                drawPixel(y,x, color);
            }
            else {
                drawPixel(x,y, color);
            }

            error -= dy;
            if (error < 0) {
                y += ystep;
                error += dx;
            }
        }
    #endif
}

// convenience overload
inline void SMRenderer::drawLine(SMLine line) {
    if (line.pt1.x < 0) line.pt1.x = 0;
    else if (line.pt1.x >= width) line.pt1.x = width-1;
    if (line.pt1.y < 0) line.pt1.y = 0;
    else if (line.pt1.y >= height) line.pt1.y = height-1;
    if (line.pt2.x < 0) line.pt2.x = 0;
    else if (line.pt2.x >= width) line.pt2.x = width-1;
    if (line.pt2.y < 0) line.pt2.y = 0;
    else if (line.pt2.y >= height) line.pt2.y = height-1;
    drawLine(line.pt1.x, line.pt1.y, line.pt2.x, line.pt2.y, line.color);
}

// Draw pixel to x,y coordinate
inline void SMRenderer::drawPixel(int x, int y, uint32_t pixel) {
    #if defined (__SNAIL__)
        
    # else 
    // http://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html
    int bpp = screen->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;

    *(uint32_t*)p = pixel;
    #endif
}

// Draw 2D elements on top of rendered scene
inline void SMRenderer::drawHud() {
    // Draw Minimap
    drawMap();
    
    // Draw Gun model, etc.
    drawSprites();
}

// REFACTORTODO figure out sprite rendering, probably pass in a map here
inline void SMRenderer::drawSprites() {
    /*
    for (auto& i : sprites) {
        if (!i.isTexture()){
            if (i.isPlaying()) {
                uint32_t bmpwidth = i.getWidth();
                uint32_t bmpheight = i.getHeight();
                // some kind of entity class for xrel/yrel

                BMP* bmp = i.cycleAnimation();

                drawBMP(bmpwidth, bmpheight, i.xrel, i.yrel, bmp);
            }
            else if (i.isStatic()) {
                uint32_t bmpwidth = i.getWidth();
                uint32_t bmpheight = i.getHeight();

                BMP* bmp = i.staticView();
                drawBMP(bmpwidth, bmpheight, i.xrel, i.yrel, bmp);
            }
        }
    }
    */
}

inline void SMRenderer::drawBMP(const uint32_t bmpwidth, const uint32_t bmpheight, const uint32_t xrel, const uint32_t yrel, BMP* bmp) {
    for (uint32_t bmpy=0; bmpy < bmpheight; bmpy++) {
        for (uint32_t bmpx=0; bmpx < bmpwidth; bmpx++) {
            RGBApixel bmppixel = bmp->GetPixel(bmpx, bmpy);
            uint32_t pixel = 0;
            pixel = ((unsigned char)bmppixel.Alpha << 24);
            pixel += ((unsigned char)bmppixel.Red << 16);
            pixel += ((unsigned char)bmppixel.Green << 8);
            pixel += ((unsigned char)bmppixel.Blue);
            
            if (pixel == fakealpha) {
                continue;
            }
            drawPixel(xrel+bmpx >= width ? width-2 : xrel+bmpx, yrel+bmpy >= height ? height-2 : yrel+bmpy, pixel);
        }
    }
}

// Will greatly simplify rendering order at the expensive of a second
// (forthcoming) pruned lines list traversal

// REFACTORTODO figure out how to fix this
inline void SMRenderer::drawMap() {
    /*
    if (!mapactive) {
        return;
    }
    for (auto i : lines) {
        // for minimap
        SMVector mmp1 = minimap.project(i.pt1, angle, position, player);
        SMVector mmp2 = minimap.project(i.pt2, angle, position, player);
        if (!mapfullscreen) {
            drawLine(mmp1.x, mmp1.y, mmp2.x, mmp2.y, i.color);
            drawPixel(minimap.bl.x + 0.5 * minimap.tr.x, minimap.bl.y + 0.5 * minimap.tr.y, 0xFFFF66);
        }
        else {
            drawLine(mmp1.x, mmp1.y, mmp2.x, mmp2.y, i.color);
            drawPixel(player.x, player.y, 0xFFFF66);
        }
    }
    
    // minimap box
    if (!mapfullscreen) {
        drawLine(minimap.bl.x, minimap.bl.y, minimap.tr.x, minimap.bl.y, minimap.boxcolor);
        drawLine(minimap.bl.x, minimap.tr.y, minimap.tr.x, minimap.tr.y, minimap.boxcolor);
        drawLine(minimap.bl.x, minimap.bl.y, minimap.bl.x, minimap.tr.y, minimap.boxcolor);
        drawLine(minimap.tr.x, minimap.bl.y, minimap.tr.x, minimap.tr.y, minimap.boxcolor);
    }
    else {
        
        //for (auto i : minimap.intersections){
        //    if (i.x % 30 == 0){
        //        SMVector p1 = i.ray.pt1;
        //        SMVector p2 = i.ray.pt2;        
        //        drawLine(p1.x, p1.y, p2.x, p2.y, i.line.color);
        //        drawPixel(i.vec.x, i.vec.y, 0xffffff);
        //    }
        //}
         

        //drawLine(raycaster.debugLines.projectionPlane);
        //drawLine(raycaster.debugLines.leftPlane);
        //drawLine(raycaster.debugLines.rightPlane);

    }
    */
}

// REFACTORTODO probably don't need these here
/*
// Vector intersection
SMVector SMRenderer::intersection(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
    int x = crossProduct(x1, y1, x2, y2);
    int y = crossProduct(x3, y3, x4, y4);
    //int det = crossProduct(x1-x2, y1-y2, x3-x4, y3-y4);
    x = crossProduct(x, x1-x2, y, x3-x4);
    y = crossProduct(x, y1-y2, y, y3-y4);
    
    SMVector result;
    result.x = x;
    result.y = y;
    result.z = 0;
    return result;
}

// Some useless vector math functions
inline double SMRenderer::dotProduct(const SMVector& vecta, const SMVector& vectb) {
    double dot = vecta.x * vectb.x;
    dot += vecta.y * vectb.y;
    return dot;
}

// Black magic
inline double SMRenderer::crossProduct(double x1, double y1, double x2, double y2) {
    int result = x1*y2 - y1*x2;
    return result;
}

// Convert to normal vector
SMVector SMRenderer::normalize(const SMVector& vecta) {
    double length = sqrt(vecta.x*vecta.x + vecta.y*vecta.y);
    SMVector result;
    result.x = vecta.x/length;
    result.y = vecta.y/length;
    result.z = 0;
    return result;
}

// Do some fancy vector math
SMVector SMRenderer::project(const SMVector& vect) {
    SMVector result;
	
    // offset position
	double x = vect.x - position.x ;
	double y = vect.y - position.y ;
	
	// camera transform
	result.x = x * sin(angle) - y * cos(angle);
	result.y = x * cos(angle) + y * sin(angle);

	result.x += player.x;
	result.y += player.y;

    result.x = result.x;
    result.y = result.x / result.y + player.y;

    return result;
}
*/

inline double SMRenderer::smoothstep(double min, double max, double val){
    // get 0 < smoothed < 1
    double smoothed = (val - min) / (max - min);

    // clamp
    if (smoothed < 0.0) smoothed = 0.0;
    if (smoothed > 1.0) smoothed = 1.0;

    // return smoothstep, f(x) = 3x^2 - 2x^3 
    return smoothed * smoothed * (3.0 - 2.0 * smoothed);
}

inline uint32_t SMRenderer::dim(uint32_t color, uint32_t amount){
    uint32_t red = (color & 0xff0000) >> 16;
    uint32_t green = (color & 0x00ff00) >> 8;
    uint32_t blue = color & 0x0000ff;

    if (red < amount){
        red = 0;
    }
    else {
        red -= amount;
    }

    if (green < amount){
        green = 0;
    }
    else {
        green -= amount;
    }

    if (blue < amount){
        blue = 0;
    }
    else {
        blue -= amount;
    }

    return red << 16 | green << 8 | blue;
}

inline uint32_t SMRenderer::brighten(uint32_t color, uint32_t amount){
    uint32_t red = (color & 0xff0000) >> 16;
    uint32_t green = (color & 0x00ff00) >> 8;
    uint32_t blue = color & 0x0000ff;

    red += amount;
    green += amount;
    blue += amount;

    red = red & 0x0000ff;
    green = green & 0x0000ff;
    blue = blue & 0x0000ff;

    return red << 16 | green << 8 | blue;
}

// For calculating collision normals later
// http://en.wikipedia.org/wiki/Fast_inverse_square_root
float SMRenderer::Q_rsqrt(float number)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long *)&y;                       // evil floating point bit level hacking
    i = 0x5f3759df - (i >> 1);               // what the fuck? 
    y = *(float *)&i;
    y = y * (threehalfs - (x2 * y * y));   // 1st iteration
    //      y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

    return y;
}