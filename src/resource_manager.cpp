#include "resource_manager.h"

ResourceManager::ResourceManager() {

}

ResourceManager::ResourceManager(ResourceManager  const&) {
    
}

ResourceManager ResourceManager::operator = (ResourceManager const& rhs) {
    return *this;
}

ResourceManager& ResourceManager::getInstance() {
    static ResourceManager* instance = new ResourceManager;
    return *instance;
}

SDL_Surface* ResourceManager::getSurface(std::string filePath) {
    auto search = surfaces.find(filePath);
    if (search == surfaces.end()) {
        std::cout << "loaded image in memory: (" << filePath.c_str() << ")\n";
        SDL_Surface* surface = SDL_LoadBMP(filePath.c_str());
        surfaces.insert(std::make_pair(filePath, surface));
    }
    return surfaces[filePath];
}