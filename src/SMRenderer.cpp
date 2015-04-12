//
//  SMRenderer.cpp
//  smtech1
//

#include "SMRenderer.h"
using namespace smtech1;

// Initialize thread controls, width, height
SMRenderer::SMRenderer(Uint32 width, Uint32 height) : renderThread(), renderRunning(false), width(width), height(height) {

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

    // Enter render loop
    render();

    // Cleanup
    SDL_Quit();
}

// Loads meshes into SMMesh objects for rendering
// Will be replaced with a file format and more concrete Mesh structure
void SMRenderer::initMeshes() {
    SMVector pt1 { static_cast<int>((width/2)-50), static_cast<int>((height/2)-50), 0 };
    SMVector pt2 { static_cast<int>((width/2)+50), static_cast<int>((height/2)-50), 0 };
    SMLine line { pt1, pt2, 0xFFFF99 };
    lines.push_back(line);
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
        for (auto i : lines) {
            SMVector pt1 = project(i.pt1);
            SMVector pt2 = project(i.pt2);
            
            // 2D Player view
            //drawLine(player.x, player.y, player.x+10*cos(angle), player.y+10*sin(angle), 0x00FF00);
            //drawLine(pt1.x, pt1.z, pt2.x, pt2.z, i.color);
            
            // 3D Projection
            
            pt1.z = pt1.z == 0 ? 1 : pt1.z;
            pt2.z = pt2.z == 0 ? 1 : pt2.z;
            
            int x1 = (-pt1.x)*16/pt1.z;
            int y1a = (-player.y)/pt1.z;
            int y1b = player.y/pt1.z;
            int x2 = (-pt2.x)*16/pt2.z;
            int y2a = (-player.y)/pt2.z;
            int y2b = player.y/pt2.z;
            for (int x=x1; x<x2; x++) {
                int ya = y1a + (x-x1) * (y2a-y1a)/(x2-x1);
                int yb = y1b + (x-x1) * (y2b-y1b)/(x2-x1);
                
                drawLine(player.x+x, 0, player.x+x, player.y + (-ya), 0xFFFFFF);
                drawLine(player.x+x, player.y+yb, player.x+x, height, 0x6699FF);
                drawLine(player.x+x, player.y+ya, player.x+x, player.y+yb, i.color);
            }
            /*
            drawLine(player.x+x1, player.y+y1a, player.x+x2, player.y+y2a, i.color);
            drawLine(player.x+x1, player.y+y1b, player.x+x2, player.y+y2b, i.color);
            drawLine(player.x+x1, player.y+y1a, player.x+x1, player.y+y1b, i.color);
            drawLine(player.x+x2, player.y+y2a, player.x+x2, player.y+y2b, i.color);
            */
        }

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
                        position.y += 10;
                        break;
                    case SDLK_a:
                        position.x += 10;
                        break;
                    case SDLK_s:
                        position.y -= 10;
                        break;
                    case SDLK_d:
                        position.x -= 10;
                        break;
                    case SDLK_ESCAPE:
                        if (mousemode) {
                            mousemode = SDL_SetRelativeMouseMode(SDL_FALSE) == 0 ? false : true;
                        }
                        else {
                            mousemode = SDL_SetRelativeMouseMode(SDL_TRUE) == 0 ? true : false;
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
inline void SMRenderer::drawLine(int x1, int y1, int x2, int y2, Uint32 color) {
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

// Draw pixel to x,y coordinate
inline void SMRenderer::drawPixel(int x, int y, Uint32 pixel) {
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
            *(Uint32*)p = pixel;
            break;
    }
}

// Vector intersection
SMVector SMRenderer::intersection(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    int x = crossProduct(x1, y1, x2, y2);
    int y = crossProduct(x3, y3, x4, y4);
    int det = crossProduct(x1-x2, y1-y2, x3-x4, y3-y4);
    x = crossProduct(x, x1-x2, y, x3-x4);
    y = crossProduct(x, y1-y2, y, y3-y4);
    
    SMVector result;
    result.x = x;
    result.y = y;
    result.z = 0;
    return result;
}

// Some useless vector math functions
int SMRenderer::dotProduct(const SMVector& vecta, const SMVector& vectb) {
    double dot = vecta.x * vectb.x;
    dot += vecta.y * vectb.y;
    return dot;
}

// Black magic
int SMRenderer::crossProduct(int x1, int y1, int x2, int y2) {
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
    // Camera transform
    result.x = vect.x-player.x;
    result.y = vect.y-player.y;
    
    // Rotation
    result.z = result.x*cos(angle) + result.y*sin(angle);
    result.x = result.x*sin(angle) - result.y*cos(angle);

    // Scaling about fixed player
    result.z = position.y - result.z;
    result.x = position.x - result.x;
    
    return result;
}
