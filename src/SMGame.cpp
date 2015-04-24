#include "SMGame.h"
#include "SMRenderer.h"
using namespace smtech1;

// Initialize thread controls, width, height
SMGame::SMGame(uint32_t width, uint32_t height) : renderThread(), renderRunning(false), width(width), height(height), smRenderer(width, height){
    player.x = static_cast<double>(width / 2);
    player.y = static_cast<double>(height / 2);
    player.z = 0.0;
    position.x = static_cast<double>(width / 2);
    position.y = static_cast<double>(height / 2);
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

    // Create some render data
    AssetLoader loader;
    loader.loadAssets("test.txt", spriteManager, map);

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

    // enter game loop
    game();

    // Cleanup
    SDL_Quit();
}

void SMGame::game(){
    // consistent FPS logic

    // process input

    // do raycasting
    std::vector<RaycastHit> intersections = raycaster.castLines(position, angle, map.lines);

    // render
    smRenderer.render(intersections, raycaster.castGap, angle, position, raycaster, map.floor, map.ceiling);
}
