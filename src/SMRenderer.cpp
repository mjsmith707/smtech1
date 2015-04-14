//
//  SMRenderer.cpp
//  smtech1
//

#include "SMRenderer.h"
using namespace smtech1;

// Initialize thread controls, width, height
SMRenderer::SMRenderer(uint32_t width, uint32_t height) : renderThread(), renderRunning(false), width(width), height(height) {
    player.x = static_cast<double>(width/2);
    player.y = static_cast<double>(height/2);
    player.z = 0.0;
    position.x = static_cast<double>(width/2);
    position.y = static_cast<double>(height/2);
    position.z = 0.0;
    // raycaster
    //1.047197551196597746154 /* 60deg in rad */
    //1.57079632679489661923132 /* 90deg in rad */
    raycaster = Raycaster(width, height, 32, 1.047197551196597746154 /* 60deg in rad */, 64);
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
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0x0, 0x0, 0x0));
        SDL_UpdateWindowSurface(window);
    }

    // Grab Mouse
    mousemode = SDL_SetRelativeMouseMode(SDL_TRUE) == 0 ? true : false;

    // Create some render data
    initMeshes();

    // minimap init
    SMVector mapbl = { 10.0, 10.0, 0 };
    SMVector maptr = { 100.0, 100.0, 0 };
    minimap = { mapbl, maptr, 0xFFFF66, 0x00ff00 };
    mapactive = true;
    mapfullscreen = false;

    

    // Enter render loop
    render();

    // Cleanup
    SDL_Quit();
}

// Loads meshes into SMMesh objects for rendering
// Will be replaced with a file format and more concrete Mesh structure
void SMRenderer::initMeshes() {
    SMVector a {315.0,35.0};
    SMVector b {210.0,70.0};
    SMVector c {105.0,175.0};
    SMVector d {105.0,290.0};
    SMVector e {315.0,370.0};
    SMVector f {420.0,315.0};
    SMVector g {525.0,185.0};
    SMVector h {525.0,35.0};
    
    SMLine ab {a,b,0x003366}; // darkblue
    SMLine bc {b,c,0x669999}; // turquoise
    SMLine cd {c,d,0x339933}; // green
    SMLine de {d,e,0x333300}; // olive
    SMLine ef {e,f,0xCCCC00}; // yellow
    SMLine fg {f,g,0xCC6600}; // orange
    SMLine gh {g,h,0xCC0000}; // red
    SMLine ha {h,a,0xCC0099}; // pink
    
    lines.push_back(ab);
    lines.push_back(bc);
    lines.push_back(cd);
    lines.push_back(de);
    lines.push_back(ef);
    lines.push_back(fg);
    lines.push_back(gh);
    lines.push_back(ha);

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
        
        // Erase framebuffer
        drawBlank();
            
        drawPixel(player.x, player.y, 0xFFFF66);
        /*
        for (auto i : lines) {
			SMVector pt1 = project(i.pt1);
		    SMVector pt2 = project(i.pt2);
	        drawLine(pt1.x, pt1.y, pt2.x, pt2.y, i.color);
        }
        */

        std::vector<RaycastHit> intersections = raycaster.castLines(player, position, angle, lines);
        minimap.intersections = intersections;
        int x = 0;
        for (auto i : intersections){
            // pretty ghetto (and inefficient ! ! !), but it's something to show
            
            if (i.dist < 1.0) i.dist = 1.0;
            double sliceHeight = player.y - (0.5 * player.y) + (32.0 / i.dist) * raycaster.planeDist;
            double roofHeight = player.y - (0.75 * player.y);
            // roof
            //drawLine(x, 0, x, player.y - 0.5 * sliceHeight, 0x8c8c8c);

            // slice
            drawLine(x, player.y - 0.5 * sliceHeight, x, sliceHeight, i.line.color);

            // floor
            //drawLine(x, sliceHeight, x, player.y * 2.0, 0x191919);
            x++;
        }
        
        
        // Draw HUD Elements
        drawHud();

        // Flip buffer
        SDL_UpdateWindowSurface(window);

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
                std::cout << "keydown" << std::endl;
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        position.y -= sin(angle);
						position.x -= cos(angle);
                        break;
                    case SDLK_a:
						position.y += sin(angle + (pi / 2.0));
						position.x += cos(angle + (pi / 2.0));
                        break;
                    case SDLK_s:
						position.y += sin(angle);
						position.x += cos(angle);
                        break;
                    case SDLK_d:
						position.y += sin(angle - (pi / 2.0));
						position.x += cos(angle - (pi / 2.0));
                        break;
					case SDLK_LEFT:
						if (angle <= 0) {
							angle = pi2;
						}
						else if (angle >= pi2) {
							angle = 0;
						}
						angle -= 0.05;
						break;
					case SDLK_RIGHT:
						if (angle <= 0) {
							angle = pi2;
						}
						else if (angle >= pi2) {
							angle = 0;
						}
						angle += 0.05;
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
                    case SDLK_q:
                        renderRunning = false;
                        break;
                }
                std::cout << "position: <" << position.x << ", " << position.y << ">" << std::endl
                << "angle: " << angle << std::endl;
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
                        angle = 0;
                    }
                    angle -= 0.1;
                }
                else if (event.motion.xrel > 0) {
                    if (angle <= 0) {
                        angle = pi2;
                    }
                    else if (angle >= pi2) {
                        angle = 0;
                    }
                    angle += 0.1;
                }
            }
            std::cout << "position: <" << player.x << ", " << player.y << ">" << std::endl
            << "angle: " << angle << std::endl;
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

// Bresenham line algorithm for _all_ octants
// http://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C.2B.2B
// Wasn't for lack of trying..
inline void SMRenderer::drawLine(int x1, int y1, int x2, int y2, uint32_t color) {
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
}

// convenience overload
inline void SMRenderer::drawLine(SMLine line) {
    drawLine(line.pt1.x, line.pt1.y, line.pt2.x, line.pt2.y, line.color);
}

// Draw pixel to x,y coordinate
inline void SMRenderer::drawPixel(int x, int y, uint32_t pixel) {
    // Debugging bounds checking
    if ((x >= width) || (x < 0) || (y >= height) || (y < 0)) {
        return;
    }

    // http://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html
    int bpp = screen->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;

    // Could probably do away with all of this and use 4
    switch(bpp) {
        case 1:
            *p = pixel;
            break;
        case 2:
            *(Uint16 *)p = pixel;
            break;
        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;
        case 4:
            *(uint32_t*)p = pixel;
            break;
    }
}

// Draw 2D elements on top of rendered scene
inline void SMRenderer::drawHud() {
    // Draw Minimap
    drawMap();
    
    // Draw Gun model, etc.
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
        for (auto i : minimap.intersections){
            SMVector p1 = i.line.pt1;
            SMVector p2 = i.line.pt2;

            drawLine(p1.x, p1.y, p2.x, p2.y, i.line.color);
            drawPixel(i.vec.x, i.vec.y, 0xffffff);
        }

        drawLine(raycaster.debugLines.projectionPlane);
        drawLine(raycaster.debugLines.leftPlane);
        drawLine(raycaster.debugLines.rightPlane);

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
