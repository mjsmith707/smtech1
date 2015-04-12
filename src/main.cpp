//
//  main.cpp
//  smtech1
//

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include "SMRenderer.h"

using namespace smtech1;

int main(void) {
    // Spawn and start renderer
    SMRenderer renderer(640, 480);;
    renderer.run();

    // IO Block until we want to quit
    std::string whatever;
    std::cin >> whatever;
    return 0;
}


