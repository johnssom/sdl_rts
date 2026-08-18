#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <string>
#include <unordered_map>
#include <iostream>
#include <SDL2/SDL.h> 

class ResourceManager {
    private:
        ResourceManager();
        ResourceManager(ResourceManager const&);
        ResourceManager operator = (ResourceManager const&);
        
        std::unordered_map<std::string, SDL_Surface*> surfaces;
    
    public:
        static ResourceManager& getInstance();

        SDL_Surface* getSurface(std::string filePath);
};

#endif