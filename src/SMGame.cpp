#include "SMGame.h"
#include "SMRenderer.h"
using namespace smtech1;

// Initialize thread controls, width, height
SMGame::SMGame(uint32_t width, uint32_t height) : renderThread(), renderRunning(false), width(width), height(height), smRenderer(width, height){
    player.x = static_cast<double>(width / 2);
    player.y = static_cast<double>(height / 2);
    player.z = 0.0;
    position.x = 0.0;
    position.y = 0.0;
    position.z = 0.0;
}

// Destructor shuts down thread before rejoining
// Beautiful RAII at work
SMGame::~SMGame() {
    renderRunning = false;
    if (renderThread.joinable()) {
        renderThread.join();
    }
}

// Public run call for render thread
void SMGame::run() {
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
void SMGame::threadinit() {
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

    // load all the assets based on the map's requirements
    // static function maybe?
    AssetLoader loader;
    loader.loadAssets("thingloadertest.txt", spriteManager, map);

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

    // initialize the renderer with SDL things
    smRenderer.init(window, screen, renderer);

    // enter game loop
    game();

    // Cleanup
    SDL_Quit();
}

void SMGame::game(){
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
    while (renderRunning){
        // TODO consistent FPS logic
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
        
        // process input
        SDL_Event event;
        getInput(event);

        // do raycasting
        std::vector<RaycastHit> intersections = raycaster.castLines(position, angle, map.lines);

        // render results
        smRenderer.render(intersections, raycaster.castGap, angle, position, raycaster, map.floor, map.ceiling, map.things);
        
        #if defined( __MACH__)
            // This probably isn't correct.
            while (frametime < framerate{1}) {
                std::this_thread::sleep_for(frametime);
                frametime += frametime;
            }
        #endif
    }
}

inline void SMGame::getInput(SDL_Event& event) {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN: {
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
                // REFACTORTODO fix sprites
                /*
                // I didn't really think this part though yet
                if (sprites.size() > 0) {
                    sprites.at(0).playAnimation();
                }
                */
            }
        }
                                  //std::cout << "position: <" << player.x << ", " << player.y << ">" << std::endl
                                  //<< "angle: " << angle << std::endl;
        }
    }
}
