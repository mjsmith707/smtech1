#include <iostream>
#include <fstream>
#include <map>
#include "AssetLoader.h"

using namespace smtech1;

//TODO load sprites here somehow, not sure how is best
void AssetLoader::loadAssets(std::string mapFile, SpriteManager spriteManager, Map& mapObj){
    std::vector<SMLine> mapLines;
    std::vector<SMThing> things;
    std::filebuf buffer;
    std::map<std::string, Texture*> textureMap;

    std::ifstream input(mapFile);
    if (input){
        double x1, y1, x2, y2;
        // if our texture name is more than 256 chars we have a problem anyways
        char texture[256];
        char peeked;
        
        input >> x1 >> y1 >> texture;

        for (; input; input >> x1 >> y1 >> texture){
            // create the texture if it's not in the map
            if (textureMap.find(std::string(texture)) == textureMap.end()){
                Texture* textureObject = new Texture(std::string(texture));
                textureMap[texture] = textureObject;
            }
	        
            // add the thing to the map's things
            things.push_back(SMThing{ SMVector{x1, y1, 0}, textureMap[texture] });
            
            // check if done with section
            // consume whitespace
            input >> std::ws;
            
            // peek char
            peeked = (char)input.peek();
            
            // if a comma (map format section delimiter), move to processing linesegs
            if (peeked == ','){
                input >> peeked;
                break;
            }
        }

        input >> x1 >> y1 >> x2 >> y2 >> texture;
        // parse our space-delimited input (no broken commas!)
        for (; input; input >> x1 >> y1 >> x2 >> y2 >> texture){
            SMLine newSeg = SMLine{ SMVector{ x1, y1, 0 }, SMVector{ x2, y2, 0 } };

            // create the texture if it's not in the map
            if (textureMap.find(std::string(texture)) == textureMap.end()){
                Texture* textureObject = new Texture(std::string(texture));
                textureMap[texture] = textureObject;
            }
            
            // attach the texture to the lineseg
            newSeg.texture = textureMap[texture];
            mapLines.push_back(newSeg);
        }

        // populate our map with linesegs
        mapObj.lines = mapLines;
        mapObj.things = things;

        // init floor and ceiling
        try {
            mapObj.floor = new Texture("floor");
            mapObj.ceiling = new Texture("ceiling");
        }
        catch (std::exception e) {
            throw e;
        }
    }
    else {
        std::cout << "Problem opening file " << mapFile.c_str() << std::endl;
    }
    
}
