//
//  main.cpp
//  smtech1
//

#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include "SMRenderer.h"

using namespace smtech1;

int main(int argc, char* argv[]) {
    // Spawn and start renderer
    SMRenderer renderer(640, 480);
    //SMRenderer renderer(1280, 1024);
    renderer.run();

    // IO Block until we want to quit
    std::string whatever;
    std::cin >> whatever;
    return 0;
}