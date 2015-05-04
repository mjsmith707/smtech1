//
//  main.cpp
//  smtech1
//

#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include "SMGame.h"

using namespace smtech1;

int main(int argc, char* argv[]) {
    // Spawn and start game
    SMGame game(1024, 768);
    //SMGame game(1280, 1024);
    game.run();

    // IO Block until we want to quit
    std::string whatever;
    std::cin >> whatever;
    return 0;
}