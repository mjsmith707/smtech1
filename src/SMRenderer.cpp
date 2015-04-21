//
//  SMRenderer.cpp
//  smtech1


#include "SMRenderer.h"

#if defined (__WINDOWS__) 
    //#define __SNAIL__
#endif

using namespace smtech1;

// Initialize thread controls, width, height
SMRenderer::SMRenderer(uint32_t width, uint32_t height) : renderThread(), renderRunning(false), width(width), height(height), trashcaster(width, height) {
    player.x = static_cast<double>(width/2);
    player.y = static_cast<double>(height/2);
    player.z = 0.0;
    position.x = static_cast<double>(width/2);
    position.y = static_cast<double>(height/2);
    position.z = 0.0;
    r_mode = DOOMCASTER;
    
}

// Destructor shuts down thread before rejoining
// Beautiful RAII at work
SMRenderer::~SMRenderer() {
    renderRunning = false;
    if (renderThread.joinable()) {
        renderThread.join();
    }
}

// Public run call for render thread
void SMRenderer::run() {
    renderRunning = true;
// If OSX or Windows run in calling thread
// If Linux/Unix run in new thread
#if defined( __MACH__)
    threadinit();
#elif defined(__WINDOWS__)
    threadinit();
#else
    renderThread = std::thread(&SMRenderer::threadinit, this);
#endif
}

// Render thread entry point
// Initialize SDL window and surface
void SMRenderer::threadinit() {
    // 100 years from now we'll return these errors to a Game class or something
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cout << "SDL Init Failed" << std::endl;
        return;
    }
    else {
        std::cout << "SDL Init Successful" << std::endl;
    }
    
    window = NULL;
    screen = NULL;

    // Initialize the SDL Window
    window = SDL_CreateWindow("smtech1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cout << "Window not created: " << SDL_GetError() << std::endl;
        return;
    }
    else {
        // Grab SDL_Surface, draw initial blank screen
        screen = SDL_GetWindowSurface(window);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0x0, 0x0, 0x0));
        SDL_UpdateWindowSurface(window);
       
    }

    // Grab Mouse
    mousemode = SDL_SetRelativeMouseMode(SDL_TRUE) == 0 ? true : false;

    // Create some render data
    initMeshes();
    
    // Load 2D Sprites
    initSprites();

    // minimap init
    SMVector mapbl = { 10.0, 10.0, 0 };
    SMVector maptr = { 100.0, 100.0, 0 };
    minimap = { mapbl, maptr, 0xFFFF66, 0x00ff00 };
    mapactive = true;
    mapfullscreen = false;

    // trashcaster
    //it's broken so forget it
    //trashcaster.loadMap(lines);
    
    // raycaster
    //1.047197551196597746154 /* 60deg in rad */
    //1.57079632679489661923132 /* 90deg in rad */
    raycaster = Raycaster(width, height, 32, 1.047197551196597746154 /* 60deg in rad */, 64, player, 1);

    // Enter render loop
    render();

    // Cleanup
    SDL_Quit();
}

// Loads meshes into SMMesh objects for rendering
// Will be replaced with a file format and more concrete Mesh structure
void SMRenderer::initMeshes() {
    std::string filename = "test.txt";
    MapLoader maploader;
    lines = maploader.loadMap(filename);
}

void SMRenderer::initSprites() {
    // Sprites
    // Slot 0 in sprites is gun uhhehherr
    // Slot 1 is doomguy
    /*
    // Chainsaw
    std::string saw1 = "SAWGA0.bmp";
    std::string saw2 = "SAWGB0.bmp";
    std::string saw3 = "SAWGC0.bmp";
    std::string saw4 = "SAWGD0.bmp";
    std::vector<std::string> sawfiles {saw1,saw2,saw3,saw4};
    Sprite saw { sawfiles, 30, (width/2)-30, height-100, false};
    sprites.push_back(saw);
    */
    std::string sg1 = "SHTGA0.bmp";
    std::string sg2 = "SHTGB0.bmp";
    std::string sg3 = "SHTGC0.bmp";
    std::string sg4 = "SHTGD0.bmp";
    std::vector<std::string> sgfiles {sg1,sg2,sg3,sg4};
    Sprite shotgun { sgfiles, 10, (width/2)-40, height-100, false, false};
    sprites.push_back(shotgun);
    
    // Doomguy
    std::string dg1 = "STFST30.bmp";
    std::string dg2 = "STFST31.bmp";
    std::string dg3 = "STFST32.bmp";
    std::string dg4 = "STFTL30.bmp";
    std::string dg5 = "STFTR30.bmp";
    std::vector<std::string> doomguyfiles {dg1,dg2,dg3,dg4,dg5};
    Sprite doomguy { doomguyfiles, 75, (width/2)-30, height-40, false, true };
    sprites.push_back(doomguy);

    std::vector<std::string> wallFile{"dankwall.bmp"};
    Sprite wall{wallFile, 0, 0, 0, true, false};
    sprites.push_back(wall);
}

// Main render thread loop function
void SMRenderer::render() {
    #if defined( __MACH__)
        // Frame capping still WIP
        // std::chrono is wack to work with
        const int r_fps = 60;
        // Framerate as 1/60th of a second
        using framerate = std::chrono::duration<uint64_t, std::ratio<1, r_fps>>;
        uint64_t lastfpstime = 0;
        uint64_t fps = 0;
        auto lasttime = std::chrono::high_resolution_clock::now();
    #endif
    
    SDL_Event event;
    
    // Main render loop
    // Checks atomic renderRunning bool to see if we should quit
    while (renderRunning) {
        #if defined( __MACH__)
            // Frame capping still TODO
            auto currtime = std::chrono::high_resolution_clock::now();
            auto frametime = currtime - lasttime;
            lasttime = currtime;

            lastfpstime += frametime.count();
            fps++;

            // Take this with a grain of salt
            // Needs to be converted to std::chrono somehow
            if (lastfpstime > 1000000000) {
                std::cout << "fps: " << fps << std::endl;
                lastfpstime = 0;
                fps = 0;
            }
        #endif
        #if defined(__SNAIL__)
            SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xff);
            SDL_RenderClear(renderer);
        #else
            // Erase framebuffer
            //drawBlank();
        #endif
        
        switch (r_mode) {
            case DOOMCASTER: {
                std::vector<RaycastHit> intersections = raycaster.castLines(position, angle, lines);
                minimap.intersections = intersections;
                uint32_t drawn = 0;
                uint32_t skipped = 0;
                uint32_t lastDrawnX = -1;
                uint32_t size = intersections.size();
                
                while (!intersections.empty()) {
                    RaycastHit i = intersections.front();
                    std::pop_heap(intersections.begin(), intersections.end());
                    intersections.pop_back();

                    if (i.x == lastDrawnX){
                        skipped++;
                        continue;
                    }
                    else {
                        lastDrawnX = i.x;
                    }

                    drawn++;

                    double lineh = std::abs(((double)height / (double)i.dist));
                    //std::cout << lineh << std::endl;
                    lineh *= sprites[2].getHeight();
                    
                    double drawStart = -lineh / 2.0 + (double)height / 2.0;
                    double drawEnd = lineh / 2.0 + (double)height / 2.0;                    

                    if (drawStart < 0) drawStart = 0;
                    if (drawEnd >= height) drawEnd = height-1;

                    uint32_t pxWidth = lineh / sprites[2].getHeight();
                    uint32_t numDrawn = 0;
                    uint32_t currPx = i.x % sprites[2].getWidth();

                    // for texture mapping
                    double fract = i.line.pt1.dist(i.vec) / (i.line.pt1.dist(i.line.pt2));
                    // this works, but it loses a lot of accuracy along the edges
                    //double fract = i.line.pt1.fastDist(i.vec) / (i.line.pt1.fastDist(i.line.pt2));
    
                    // smoothstep constant to dim by
                    // pretty arbitrary, have to play around with the values a lot to get things that look good
                    // especially depending on the current texture darkness
                    double ssconst = 200.0 * smoothstep(150.0, 800.0, i.dist);

                    for (int d = 0; d < raycaster.castGap; d++){
                        // monocolored, shaded walls
                        //vLine(i.x + d, drawStart, drawEnd, dim(i.line.color, 200.0 * smoothstep(0.0, 500.0, i.dist)));
                        
                        // textured walls
                        //TODO fix shading factor
                        texVLine(i.x + d, drawStart, drawEnd, i.dist, fract, ssconst, sprites[2].getWidth(), sprites[2].getHeight(), sprites[2].staticView());
                        
                        // roof
                        vLine(i.x + d, 0, drawStart, 0x111111);

                        //floor
                        vLine(i.x + d, drawEnd, height, 0x020202);
                    }
                }
                //std::cout << drawn << " " << skipped << " " << size << std::endl;
                break;
            }
            case TRASHCASTER: {
                /*
                std::vector<SMLine> projectedLines = trashcaster.raycast(position, angle);
                
                for (auto i : projectedLines) {
                    drawLine(i.pt1.x, i.pt1.y, i.pt2.x, i.pt2.y, i.color);
                }
                 */
                break;
            }
        }
        
        // Draw HUD Elements
        drawHud();

        #if defined (__SNAIL__)
            SDL_RenderPresent(renderer);
        #else 
            // Flip buffer
            SDL_UpdateWindowSurface(window);
        #endif

        // Check for input
        // Will be spun off into a different class/thread someday
        getInput(event);
        
        #if defined( __MACH__)
            // This probably isn't correct.
            while (frametime < framerate{1}) {
                std::this_thread::sleep_for(frametime);
                frametime += frametime;
            }
        #endif
    }
}

inline void SMRenderer::getInput(SDL_Event& event) {
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_KEYDOWN: {
                //std::cout << "keydown" << std::endl;
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        position.y -= speed * sin(angle);
						position.x -= speed * cos(angle);
                        break;
                    case SDLK_a:
                        position.y += speed * sin(angle + (pi / 2.0));
                        position.x += speed * cos(angle + (pi / 2.0));
                        break;
                    case SDLK_s:
                        position.y += speed * sin(angle);
                        position.x += speed * cos(angle);
                        break;
                    case SDLK_d:
                        position.y += speed * sin(angle - (pi / 2.0));
                        position.x += speed * cos(angle - (pi / 2.0));
                        break;
					case SDLK_LEFT:
						if (angle <= 0) {
							angle = pi2;
						}
						else if (angle >= pi2) {
							angle = 0.01;
						}
						angle -= 0.01;
						break;
					case SDLK_RIGHT:
						if (angle <= 0) {
							angle = pi2;
						}
						else if (angle >= pi2) {
							angle = 0.01;
						}
						angle += 0.01;
						break;
                    case SDLK_ESCAPE:
                        if (mousemode) {
                            mousemode = SDL_SetRelativeMouseMode(SDL_FALSE) == 0 ? false : true;
                        }
                        else {
                            mousemode = SDL_SetRelativeMouseMode(SDL_TRUE) == 0 ? true : false;
                        }
                        break;
                    case SDLK_TAB:
                        if (!mapactive) {
                            mapactive = true;
                            minimap.bl = SMVector{ 10.0, 10.0 };
                            minimap.tr = SMVector{ 100.0, 100.0 };
                        }
                        else if (mapactive && !mapfullscreen) {
                            mapfullscreen = true;
                            minimap.bl = SMVector{ 0.0, 0.0 };
                            minimap.tr = SMVector{ player.x * 2.0, player.y * 2.0 };
                        }
                        else if (mapactive && mapfullscreen) {
                            mapactive = false;
                            mapfullscreen = false;
                        }
                        break;
                    case SDLK_1:
                        r_mode = TRASHCASTER;
                        break;
                    case SDLK_2:
                        r_mode = DOOMCASTER;
                        break;
                    case SDLK_q:
                        renderRunning = false;
                        break;
                }
                //std::cout << "position: <" << position.x << ", " << position.y << ">" << std::endl
                //<< "angle: " << angle << std::endl;
                return;
            }
            case SDL_MOUSEMOTION: {
                if (!mousemode) {
                    break;
                }
                // Clamping camera angle to 0<2pi with wraparound;
                if (event.motion.xrel < 0) {
                    if (angle <= 0) {
                        angle = pi2;
                    }
                    else if (angle >= pi2) {
                        angle = 0.01;
                    }
                    angle -= 0.04;
                }
                else if (event.motion.xrel > 0) {
                    if (angle <= 0) {
                        angle = pi2;
                    }
                    else if (angle >= pi2) {
                        angle = 0.01;
                    }
                    angle += 0.04;
                }
            }
            case SDL_MOUSEBUTTONDOWN: {
                if (!mousemode) {
                    break;
                }
                else if (event.button.button == SDL_BUTTON_LEFT) {
                    // I didn't really think this part though yet
                    if (sprites.size() > 0) {
                        sprites.at(0).playAnimation();
                    }
                }
            }
            //std::cout << "position: <" << player.x << ", " << player.y << ">" << std::endl
            //<< "angle: " << angle << std::endl;
        }
    }
}

// Blanks surface
inline void SMRenderer::drawBlank() {
    for (unsigned int y=0; y<height; y++) {
        for (unsigned int x=0; x<width; x++) {
            drawPixel(x,y,0x0);
        }
    }
}

// Textured vline
inline void SMRenderer::texVLine(int x1, int y1, int y2, int dist, double fract, double ssconst, int w, int h, BMP* texture) {
#if defined(__SNAIL__)
    drawLine(x1, y1, x1, y2, color);
#else
    int wallh = y2 - y1;

    for (int i = 0; i < wallh; i++){
        // do something about these casts
        
        // TODO tiling check, vs constant texture
        RGBApixel pix = texture->GetPixel((uint32_t)(fract * w), (int)(((double)i / (double)wallh) * h));

        // consolidate into oneliner
        uint32_t pxc = (unsigned char)pix.Alpha << 24;
        pxc += ((unsigned char)pix.Red << 16);
        pxc += ((unsigned char)pix.Green << 8);
        pxc += ((unsigned char)pix.Blue);
        
        pxc = dim(pxc, ssconst);

        drawPixel(x1, y1 + i, pxc);
    }
#endif
}


// Vertical Line
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

inline void SMRenderer::drawSprites() {
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
}

inline void SMRenderer::drawBMP(const uint32_t bmpwidth, const uint32_t bmpheight, const uint32_t xrel, const uint32_t yrel, BMP* bmp) {
    for (int bmpy=0; bmpy<bmpheight; bmpy++) {
        for (int bmpx=0; bmpx<bmpwidth; bmpx++) {
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
inline void SMRenderer::drawMap() {
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
        /*
        for (auto i : minimap.intersections){
            if (i.x % 30 == 0){
                SMVector p1 = i.ray.pt1;
                SMVector p2 = i.ray.pt2;
                
                
                
                drawLine(p1.x, p1.y, p2.x, p2.y, i.line.color);
                drawPixel(i.vec.x, i.vec.y, 0xffffff);
            }
        }
         */

        //drawLine(raycaster.debugLines.projectionPlane);
        //drawLine(raycaster.debugLines.leftPlane);
        //drawLine(raycaster.debugLines.rightPlane);

    }
}

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