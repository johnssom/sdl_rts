/**
 * COMPILE: g++ main.cpp ./glad/src/glad.c -I./glad/include -o prog.exe -lmingw32 -lSDL2main -lSDL2 && ./prog.exe
 * TODO: 
 *  - Upgrade SDL2 to SDL3
 *  - Upgrade OpenGL to DirectX
 *  - Learn vectors
 *  - Learn smart pointers / memory
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <SDL2/SDL.h> 
#include <SDL2/SDL_ttf.h>
#include <glad/glad.h> // Header file for OpenGL API

#include "sdl_app.h"
#include "unit_entity.h"
#include "game_entity.h"
#include "texture_rectangle.h"
#include "animated_sprite.h"
#include "sound.h"
#include "dynamic_text.h"
#include "obstacle.h"
#include "path_grid.h"

SDLApp* app;

GameEntity* selectionBox;
std::vector<std::shared_ptr<GameEntity>> dummies;
SDL_Texture* textTexture;
SDL_Rect textBox;
Sound* collisionSound;
DynamicText* text;
DynamicText* text2;
std::vector<std::shared_ptr<UnitEntity>> units;
std::vector<std::shared_ptr<UnitEntity>> selectedUnits;
std::vector<Obstacle> obstacles;
PathGrid* pathGrid = nullptr;
const int CELL_SIZE = 32;

struct Selector {
    int startX = 0, startY = 0;
    int currX = 0, currY = 0;
    bool isDragging = false;
};
Selector selector;

void setPixel(SDL_Surface* surface, int mouseX, int mouseY, uint8_t r, uint8_t g, uint8_t b) {
    std::cout << "mouse coords:" << mouseX << "," << mouseY << "\n";
    SDL_LockSurface(surface);
    uint8_t* pixelArray = (uint8_t*) surface->pixels;
    pixelArray[mouseY* surface->pitch + mouseX * surface->format->BytesPerPixel + 0] = g;
    pixelArray[mouseY* surface->pitch + mouseX * surface->format->BytesPerPixel + 1] = b;
    pixelArray[mouseY* surface->pitch + mouseX * surface->format->BytesPerPixel + 2] = r;
    SDL_UnlockSurface(surface);
}

void spawnObstacles(SDL_Renderer* renderer, int count) {
    for (int i = 0; i < count; i++) {
        int w = 40 + std::rand() % 80;
        int h = 40 + std::rand() % 80;
        int x = std::rand() % (1600 - w);
        int y = std::rand() % (900 - h);
        obstacles.push_back(Obstacle(x, y, w, h));
    }
}

void handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        const Uint8* state = SDL_GetKeyboardState(NULL);
        // if (event.button.button == SDL_BUTTON_LEFT) {
        //     setPixel(screen, mouseX, mouseY, 255, 0 , 0);
        // }
        switch(event.type) {
            case SDL_QUIT:
                app->quitGame();
                break;
            case SDL_KEYDOWN:
                // std::cout << "keydown: " << event.key.keysym.sym << "\n";
                // if (state[SDL_SCANCODE_LCTRL] && state[SDL_SCANCODE_RIGHT]) {
                //     std::cout << "command pressed\n";
                // }
                // if (event.key.keysym.sym == SDLK_UP) {
                    
                // }
                break;
            case SDL_MOUSEMOTION:
                if (event.motion.state & SDL_BUTTON_LMASK) {
                    if (!selector.isDragging) {
                        selector.startX = event.motion.x;
                        selector.startY = event.motion.y;
                    }
                    selector.isDragging = true;
                    selector.currX = event.motion.x;
                    selector.currY = event.motion.y;
                }
                // charBox2.x = event.motion.x;
                // charBox2.y = event.motion.y;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    for (const auto& selectedUnit : selectedUnits) {
                        if (state[SDL_SCANCODE_LSHIFT]) {
                            selectedUnit->queueMoveCommand(app->getMouseX(), app->getMouseY());
                        } else {
                            selectedUnit->commandMove(app->getMouseX(), app->getMouseY());
                        }
                    }
                }
                else if (event.button.button == SDL_BUTTON_LEFT) {
                    selectedUnits.clear();
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    selector.isDragging = false;
                    for (const auto& unit : units) {
                        if (selectionBox->getCollider(0).isColliding(unit->getGameEntity().getCollider(0))) {
                            selectedUnits.push_back(unit);
                        }
                    }
                }
                break;
        }
    }
}

void handleUpdates() {
    for (const auto& unit : units) {
        unit->update();
    }
    int i = 0;
    for (const auto& dummy : dummies) {
        dummy->update();
        dummy->getCollider(0).setPosition(dummy->getSprite().getPositionX(), dummy->getSprite().getPositionY());
        dummy->getCollider(0).setDimensions(dummy->getSprite().getWidth(), dummy->getSprite().getHeight());
        // for (const auto& unit : units) {
        //     if (dummy->getCollider(0).isColliding(unit->getGameEntity().getCollider(0))) {
        //         std::cout << "HIT: (" << i << ")\n";
        //     }
        // }
        i++;
    }
    SDL_Delay(10);
}

void handleRendering() {
    SDL_SetRenderDrawColor(app->getRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(app->getRenderer(), &textBox);
    SDL_RenderCopy(app->getRenderer(), textTexture, NULL, &textBox);

    if (pathGrid) {
        int cs = pathGrid->getCellSize();
        for (int gy = 0; gy < pathGrid->getGridHeight(); gy++) {
            for (int gx = 0; gx < pathGrid->getGridWidth(); gx++) {
                SDL_Rect cell = {gx * cs, gy * cs, cs, cs};
                if (pathGrid->isWalkable(gx, gy)) {
                    SDL_SetRenderDrawColor(app->getRenderer(), 40, 40, 40, SDL_ALPHA_OPAQUE);
                    SDL_RenderDrawRect(app->getRenderer(), &cell);
                } else {
                    SDL_SetRenderDrawColor(app->getRenderer(), 180, 40, 40, 120);
                    SDL_RenderFillRect(app->getRenderer(), &cell);
                }
            }
        }
    }
    
    if (selector.isDragging) {
        int x = std::min(selector.startX, selector.currX);
        int y = std::min(selector.startY, selector.currY);
        int w = std::abs(selector.startX - selector.currX);
        int h = std::abs(selector.startY - selector.currY);
        selectionBox->setPosition(x, y);
        selectionBox->setDimensions(w, h);
        selectionBox->render();
    }

    for (const auto& unit : units) {
        AnimatedSprite* animSprite = dynamic_cast<AnimatedSprite*>(&unit->getGameEntity().getSprite());
        if (animSprite) {
            static int unitFrameNum = 0;
            animSprite->playFrame(0, 0, 32, 44, unitFrameNum);
            unitFrameNum++;
        }
        unit->getGameEntity().render();
    }

    for (const auto& unit : units) {
        Action* action = unit->getCurrentAction();
        PathAction* pathAction = dynamic_cast<PathAction*>(action);
        if (!pathAction) continue;
        const auto& path = pathAction->getPath();
        int wp = pathAction->getCurrentWaypoint();
        if (path.empty()) continue;

        SDL_Renderer* r = app->getRenderer();
        int ux = unit->getGameEntity().getSprite().getPositionX();
        int uy = unit->getGameEntity().getSprite().getPositionY();

        int startX = ux, startY = uy;
        for (size_t i = wp; i < path.size(); i++) {
            SDL_SetRenderDrawColor(r, 0, 200, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawLine(r, startX, startY, path[i].first, path[i].second);
            startX = path[i].first;
            startY = path[i].second;
        }
    }

    for (const auto& dummy : dummies) {
        dummy->render();
    }

    SDL_SetRenderDrawColor(app->getRenderer(), 100, 60, 20, SDL_ALPHA_OPAQUE);
    for (const auto& obs : obstacles) {
        SDL_Rect rect = {obs.x, obs.y, obs.w, obs.h};
        SDL_RenderFillRect(app->getRenderer(), &rect);
    }

    std::string textContent = "Martin";
    std::string counterText = std::to_string(app->getMouseX());
    
    text->render(app->getRenderer(), textContent, 400, 200, 150, 40);
    text2->render(app->getRenderer(), counterText, 400, 400, 150, 70);
}

int main(int argc, char* argv[]){
    app = new SDLApp(SDL_INIT_VIDEO | SDL_INIT_AUDIO, (char*) "Martin Game", 1600, 900);

    text = new DynamicText("./assets/fonts/font.ttf", 32);
    text2 = new DynamicText("./assets/fonts/font.ttf", 16);

    selectionBox = new GameEntity(app->getRenderer());
    selectionBox->addCollider();
    selectionBox->setDimensions(0, 0);
    selectionBox->setPosition(0, 0);

    for (int i = 0; i < 10; i++) {
        GameEntity* ge = new GameEntity(app->getRenderer());
        ge->setSprite(new AnimatedSprite(app->getRenderer(), "./assets/img/iso_char.bmp"));
        ge->addCollider();
        ge->setDimensions(40, 50);
        ge->setPosition(150 + 60 * (i % 3), 150 + 60 * (i / 3));
        units.push_back(std::make_unique<UnitEntity>(ge));
    }
    
    // Set units context for flocking with separation
    for (auto& unit : units) {
        unit->setUnitsContext(&units);
    }

    std::srand(42);
    spawnObstacles(app->getRenderer(), 8);

    pathGrid = new PathGrid(1600, 900, CELL_SIZE);
    for (const auto& obs : obstacles) {
        pathGrid->markObstacle(obs);
    }

    for (auto& unit : units) {
        unit->setObstaclesContext(&obstacles);
        unit->setPathGrid(pathGrid);
    }
    
    collisionSound = new Sound("./assets/sounds/hit.wav");
    collisionSound->setupDevice();
    
    for (int i = 0; i < 10; i++) {
        std::shared_ptr<GameEntity> dummy = std::make_shared<GameEntity>(app->getRenderer(), "./assets/img/char.bmp");
        dummy->addCollider();
        dummy->getSprite().setDimensions(40, 50);
        dummy->getSprite().setPosition(60 * (i % 3), 60 * (i / 3));
        dummies.push_back(dummy);
    }

    app->setEventCallback(handleEvents);
    app->setUpdateCallback(handleUpdates);
    app->setRenderCallback(handleRendering);
    app->runLoop();

    delete app;
    delete collisionSound;
    delete pathGrid;

    return 0;
}