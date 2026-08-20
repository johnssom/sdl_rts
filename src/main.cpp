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
#include <algorithm>
#include <cmath>
#include <functional>

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
#include "building.h"
#include "loading_bar.h"
#include "button_panel.h"

SDLApp* app;

GameEntity* selectionBox;
SDL_Texture* textTexture;
SDL_Rect textBox;
Sound* collisionSound;
DynamicText* text;
DynamicText* text2;
std::vector<std::shared_ptr<UnitEntity>> units;
std::vector<std::shared_ptr<UnitEntity>> selectedUnits;
std::vector<Obstacle> obstacles;
PathGrid* pathGrid = nullptr;

#define SCREEN_WIDTH              1600
#define SCREEN_HEIGHT             900

#define MAP_SIZE                  96
#define MAP_RENDER_SIZE           24
#define MAP_RENDER_OFFSET_X       ((SCREEN_WIDTH - (TILE_WIDTH * MAP_RENDER_SIZE)) / 2)
#define MAP_RENDER_OFFSET_Y       425

#define BUILDING_HEIGHT           50
#define ISO_RENDER_SPEED          15
#define MAX_ISO_OBJECTS           (MAP_SIZE * MAP_SIZE)

struct Selector {
    int startX = 0, startY = 0;
    int currX = 0, currY = 0;
    bool isDragging = false;
};
Selector selector;

struct ISOObject {
    int x, y;
    int sx, sy;
    SDL_Texture* texture;
};

ISOObject isoObjects[MAX_ISO_OBJECTS];
int numISOObjects = 0;
int mapData[MAP_RENDER_SIZE][MAP_RENDER_SIZE];
SDL_Texture* tileTexture = nullptr;
int tileTextureW = 0;
int tileTextureH = 0;
double drawTimer = 0;

struct ISORenderEntry {
    int sortY;
    std::function<void()> render;
};
std::vector<ISORenderEntry> renderEntries;

std::vector<Building> buildings;
LoadingBar* productionBar = nullptr;
Building* selectedBuilding = nullptr;
ButtonPanel* buttonPanel = nullptr;

void spawnUnitAtTile(int tileX, int tileY) {
    int sx, sy;
    PathGrid::tileToISO(tileX, tileY, &sx, &sy);
    GameEntity* ge = new GameEntity(app->getRenderer());
    ge->setSprite(new AnimatedSprite(app->getRenderer(), "./assets/img/iso_char.bmp"));
    ge->addCollider();
    ge->setDimensions(30, 50);
    ge->setPosition(sx + TILE_WIDTH / 2, sy + TILE_HEIGHT / 2);
    ge->getCollider(0).setDimensions(PATH_CELL_WIDTH, PATH_CELL_HEIGHT);
    ge->getCollider(0).setPosition(sx + TILE_WIDTH / 2 - PATH_CELL_WIDTH / 2, sy + TILE_HEIGHT / 2 - PATH_CELL_HEIGHT / 2);
    auto unit = std::make_unique<UnitEntity>(ge);
    unit->setUnitsContext(&units);
    unit->setObstaclesContext(&obstacles);
    unit->setPathGrid(pathGrid);
    units.push_back(std::move(unit));
}

void setPixel(SDL_Surface* surface, int mouseX, int mouseY, uint8_t r, uint8_t g, uint8_t b) {
    std::cout << "mouse coords:" << mouseX << "," << mouseY << "\n";
    SDL_LockSurface(surface);
    uint8_t* pixelArray = (uint8_t*) surface->pixels;
    pixelArray[mouseY* surface->pitch + mouseX * surface->format->BytesPerPixel + 0] = g;
    pixelArray[mouseY* surface->pitch + mouseX * surface->format->BytesPerPixel + 1] = b;
    pixelArray[mouseY* surface->pitch + mouseX * surface->format->BytesPerPixel + 2] = r;
    SDL_UnlockSurface(surface);
}

void toISO(int x, int y, int* sx, int* sy) {
    *sx = MAP_RENDER_OFFSET_X + ((x * TILE_WIDTH / 2) + (y * TILE_WIDTH / 2));
    *sy = MAP_RENDER_OFFSET_Y + ((y * TILE_HEIGHT / 2) - (x * TILE_HEIGHT / 2));
}

void clearISOObjects() {
    numISOObjects = 0;
}

void addISOObject(int x, int z, int sx, int sy, SDL_Texture* texture) {
    if (numISOObjects < MAX_ISO_OBJECTS) {
        ISOObject* o = &isoObjects[numISOObjects++];
        toISO(x, z, &o->x, &o->y);
        o->sx = o->x + sx;
        o->sy = o->y + sy;
        o->texture = texture;
    }
}

static int drawComparator(const void* a, const void* b) {
    const ISOObject* o1 = (const ISOObject*)a;
    const ISOObject* o2 = (const ISOObject*)b;
    return o1->y - o2->y;
}

void drawISOObjects(SDL_Renderer* renderer) {
    qsort(isoObjects, numISOObjects, sizeof(ISOObject), drawComparator);

    for (int i = 0; i < numISOObjects; i++) {
        if (drawTimer >= i) {
            ISOObject* o = &isoObjects[i];
            SDL_Rect dest = {o->sx, o->sy, tileTextureW, tileTextureH};
            SDL_RenderCopy(renderer, o->texture, NULL, &dest);
        }
    }
}

void initMap() {
    for (int x = 0; x < MAP_RENDER_SIZE; x++) {
        for (int z = 0; z < MAP_RENDER_SIZE; z++) {
            mapData[x][z] = 0;
        }
    }
}

void loadTileTexture(SDL_Renderer* renderer) {
    SDL_Surface* surface = ResourceManager::getInstance().getSurface("./assets/img/tile.bmp");
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0xFF, 0, 0xFF));
    tileTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_QueryTexture(tileTexture, NULL, NULL, &tileTextureW, &tileTextureH);
}

void drawMap() {
    for (int x = 0; x < MAP_RENDER_SIZE; x++) {
        for (int z = 0; z < MAP_RENDER_SIZE; z++) {
            int n = mapData[x][z];
            if (n >= 0 && tileTexture) {
                addISOObject(x, z, 0, 0, tileTexture);
            }
        }
    }
}

void spawnObstacles(SDL_Renderer* renderer, int count) {
    for (int i = 0; i < count; i++) {
        int gx = 5 + std::rand() % 10;
        int gy = 5 + std::rand() % 10;
        obstacles.push_back(Obstacle(gx, gy));
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
                    int n = selectedUnits.size();
                    if (n == 1 || !pathGrid) {
                        for (const auto& selectedUnit : selectedUnits) {
                            if (state[SDL_SCANCODE_LSHIFT]) {
                                selectedUnit->queueMoveCommand(app->getMouseX(), app->getMouseY());
                            } else {
                                selectedUnit->commandMove(app->getMouseX(), app->getMouseY());
                            }
                        }
                    } else {
                        double avgPX = 0, avgPY = 0;
                        for (const auto& u : selectedUnits) {
                            avgPX += u->getGameEntity().getPositionX();
                            avgPY += u->getGameEntity().getPositionY();
                        }
                        avgPX /= n;
                        avgPY /= n;
                        PathGrid::GridCoord avgGC = pathGrid->pixelToGrid((int)avgPX, (int)avgPY);

                        PathGrid::GridCoord target = pathGrid->pixelToGrid(app->getMouseX(), app->getMouseY());
                        double maxDist2 = (double)(target.x - avgGC.x) * (target.x - avgGC.x)
                                        + (double)(target.y - avgGC.y) * (target.y - avgGC.y);

                        struct Target { int gx, gy; };
                        std::vector<Target> targets;
                        targets.push_back({target.x, target.y});
                        for (int radius = 3; (int)targets.size() < n; radius += 3) {
                            for (int dx = -radius; dx <= radius; dx += 3) {
                                for (int dy = -radius; dy <= radius; dy += 3) {
                                    if (std::abs(dx) != radius && std::abs(dy) != radius) continue;
                                    int gx = target.x + dx;
                                    int gy = target.y + dy;
                                    double d2 = (double)(gx - avgGC.x) * (gx - avgGC.x)
                                              + (double)(gy - avgGC.y) * (gy - avgGC.y);
                                    if (d2 <= maxDist2) {
                                        targets.push_back({gx, gy});
                                        if ((int)targets.size() >= n) break;
                                    }
                                }
                                if ((int)targets.size() >= n) break;
                            }
                        }

                        std::vector<std::pair<std::shared_ptr<UnitEntity>, double>> indexed(n);
                        for (int i = 0; i < n; i++) {
                            indexed[i].first = selectedUnits[i];
                            int ux = selectedUnits[i]->getGameEntity().getPositionX();
                            int uy = selectedUnits[i]->getGameEntity().getPositionY();
                            indexed[i].second = (ux - app->getMouseX()) * (ux - app->getMouseX())
                                              + (uy - app->getMouseY()) * (uy - app->getMouseY());
                        }
                        std::sort(indexed.begin(), indexed.end(), [](const auto& a, const auto& b) {
                            return a.second > b.second;
                        });

                        for (int i = 0; i < n && i < (int)targets.size(); i++) {
                            auto [px, py] = pathGrid->gridToPixel(targets[i].gx, targets[i].gy);
                            if (state[SDL_SCANCODE_LSHIFT]) {
                                indexed[i].first->queueMoveCommand(px, py);
                            } else {
                                indexed[i].first->commandMove(px, py);
                            }
                        }
                    }
                }
                else if (event.button.button == SDL_BUTTON_LEFT) {
                    selectedUnits.clear();
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int mx = event.button.x;
                    int my = event.button.y;

                    if (selectedBuilding && buttonPanel->handleClick(mx, my)) {
                        selector.isDragging = false;
                        break;
                    }

                    bool selectedNew = false;
                    for (auto& building : buildings) {
                        if (building.isInsideFootprint(mx, my)) {
                            selectedBuilding = &building;
                            buttonPanel->populate(building.getButtonDefs());
                            selectedNew = true;
                            break;
                        }
                    }
                    if (!selectedNew) {
                        selectedBuilding = nullptr;
                        buttonPanel->clear();
                    }

                    int dx = selector.currX - selector.startX;
                    int dy = selector.currY - selector.startY;
                    bool wasDrag = (dx * dx + dy * dy) > 16;

                    if (wasDrag) {
                        for (const auto& unit : units) {
                            if (selectionBox->getCollider(0).isColliding(unit->getGameEntity().getCollider(0))) {
                                selectedUnits.push_back(unit);
                            }
                        }
                    } else {
                        selectedUnits.clear();
                        SDL_Point clickPt = {mx, my};
                        for (const auto& unit : units) {
                            SDL_Rect hitbox = unit->getGameEntity().getCollider(0).getHitBox();
                            if (SDL_PointInRect(&clickPt, &hitbox)) {
                                selectedUnits.push_back(unit);
                                break;
                            }
                        }
                    }
                    selector.isDragging = false;
                }
                break;
        }
    }
}

void handleUpdates() {
    for (const auto& unit : units) {
        int prevX = unit->getGameEntity().getPositionX();
        int prevY = unit->getGameEntity().getPositionY();
        unit->update();
        int newX = unit->getGameEntity().getPositionX();
        int newY = unit->getGameEntity().getPositionY();
        if (Building::isInsideAnyFootprint(newX, newY, buildings)) {
            unit->getGameEntity().setPosition(prevX, prevY);
        }
    }

    for (auto& building : buildings) {
        building.update(10.0);
        if (building.isDone()) {
            building.resetProduction();
            spawnUnitAtTile(building.getTileX() - 1, building.getTileY());
        }
    }

    drawTimer = std::min(drawTimer + ISO_RENDER_SPEED, (double)numISOObjects);
    SDL_Delay(10);
}

void handleRendering() {
    SDL_SetRenderDrawColor(app->getRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(app->getRenderer(), &textBox);
    SDL_RenderCopy(app->getRenderer(), textTexture, NULL, &textBox);

    clearISOObjects();
    drawMap();
    drawISOObjects(app->getRenderer());

    if (pathGrid) {
        for (int gy = 0; gy < pathGrid->getGridHeight(); gy++) {
            for (int gx = 0; gx < pathGrid->getGridWidth(); gx++) {
                int sx, sy;
                PathGrid::toISO(gx, gy, &sx, &sy);

                int cw = PATH_CELL_WIDTH;
                int ch = PATH_CELL_HEIGHT;
                SDL_Point points[5];
                points[0] = {sx + cw / 2, sy};
                points[1] = {sx + cw, sy + ch / 2};
                points[2] = {sx + cw / 2, sy + ch};
                points[3] = {sx, sy + ch / 2};
                points[4] = {sx + cw / 2, sy};

                if (pathGrid->isWalkable(gx, gy)) {
                    SDL_SetRenderDrawColor(app->getRenderer(), 40, 40, 255, SDL_ALPHA_OPAQUE);
                    SDL_RenderDrawLines(app->getRenderer(), points, 5);
                } else {
                    SDL_SetRenderDrawColor(app->getRenderer(), 180, 40, 40, 120);
                    SDL_RenderDrawLines(app->getRenderer(), points, 5);
                }
            }
        }
    }
    
    if (selector.isDragging) {
        int x = std::min(selector.startX, selector.currX);
        int y = std::min(selector.startY, selector.currY);
        int w = std::abs(selector.startX - selector.currX);
        int h = std::abs(selector.startY - selector.currY);
        selectionBox->setDimensions(w, h);
        selectionBox->setPosition(x + w / 2, y + h / 2);
        selectionBox->render();
    }

    renderEntries.clear();

    for (const auto& unit : units) {
        int sortY = unit->getGameEntity().getPositionY();
        renderEntries.push_back({sortY, [unit]() {
            int ax = unit->getGameEntity().getPositionX();
            int ay = unit->getGameEntity().getPositionY();
            int cw = PATH_CELL_WIDTH;
            int ch = PATH_CELL_HEIGHT;
            int boxH = ch * 2;
            float hw = cw / 2.0f;
            float hh = ch / 2.0f;

            SDL_Renderer* r = app->getRenderer();
            SDL_Color leftColor  = {255, 120, 180, 200};
            SDL_Color rightColor = {255, 90, 140, 200};
            SDL_Color topColor   = {255, 150, 210, 200};
            int indices[6] = {0, 1, 2, 0, 2, 3};

            SDL_Vertex leftVerts[4] = {
                { SDL_FPoint{(float)(ax - hw), (float)ay},             leftColor, SDL_FPoint{0,0} },
                { SDL_FPoint{(float)ax, (float)(ay + hh)},           leftColor, SDL_FPoint{0,0} },
                { SDL_FPoint{(float)ax, (float)(ay - boxH + hh)},   leftColor, SDL_FPoint{0,0} },
                { SDL_FPoint{(float)(ax - hw), (float)(ay - boxH)},   leftColor, SDL_FPoint{0,0} }
            };
            SDL_RenderGeometry(r, NULL, leftVerts, 4, indices, 6);

            SDL_Vertex rightVerts[4] = {
                { SDL_FPoint{(float)ax, (float)(ay + hh)},           rightColor, SDL_FPoint{0,0} },
                { SDL_FPoint{(float)(ax + hw), (float)ay},             rightColor, SDL_FPoint{0,0} },
                { SDL_FPoint{(float)(ax + hw), (float)(ay - boxH)},   rightColor, SDL_FPoint{0,0} },
                { SDL_FPoint{(float)ax, (float)(ay - boxH + hh)},   rightColor, SDL_FPoint{0,0} }
            };
            SDL_RenderGeometry(r, NULL, rightVerts, 4, indices, 6);

            SDL_Vertex topVerts[4] = {
                { SDL_FPoint{(float)ax, (float)(ay - boxH - hh)},   topColor, SDL_FPoint{0,0} },
                { SDL_FPoint{(float)(ax + hw), (float)(ay - boxH)},   topColor, SDL_FPoint{0,0} },
                { SDL_FPoint{(float)ax, (float)(ay - boxH + hh)},   topColor, SDL_FPoint{0,0} },
                { SDL_FPoint{(float)(ax - hw), (float)(ay - boxH)},   topColor, SDL_FPoint{0,0} }
            };
            SDL_RenderGeometry(r, NULL, topVerts, 4, indices, 6);

            AnimatedSprite* animSprite = dynamic_cast<AnimatedSprite*>(&unit->getGameEntity().getSprite());
            if (animSprite) {
                int row = unit->getDirection() * 44;
                int frame = unit->isMoving() ? unit->getAnimFrame() : 1;
                animSprite->playFrame(0, row, 32, 44, frame);
            }
            unit->getGameEntity().render();

            Action* action = unit->getCurrentAction();
            PathAction* pathAction = dynamic_cast<PathAction*>(action);
            if (pathAction) {
                const auto& path = pathAction->getPath();
                int wp = pathAction->getCurrentWaypoint();
                if (!path.empty()) {
                    int startX = unit->getGameEntity().getPositionX();
                    int startY = unit->getGameEntity().getPositionY();
                    for (size_t i = wp; i < path.size(); i++) {
                        SDL_SetRenderDrawColor(r, 0, 200, 255, SDL_ALPHA_OPAQUE);
                        SDL_RenderDrawLine(r, startX, startY, path[i].first, path[i].second);
                        startX = path[i].first;
                        startY = path[i].second;
                    }
                }
            }
        }});
    }

    for (const auto& obs : obstacles) {
        int sx, sy;
        PathGrid::tileToISO(obs.x, obs.y, &sx, &sy);
        int sortY = sy + TILE_HEIGHT / 2;
        renderEntries.push_back({sortY, [obs, sx, sy]() {
            SDL_Color fillColor = {100, 60, 20, SDL_ALPHA_OPAQUE};
            SDL_Vertex vertices[4] = {
                { SDL_FPoint{(float)(sx + TILE_WIDTH / 2), (float)sy}, fillColor, SDL_FPoint{0.0f, 0.0f} },
                { SDL_FPoint{(float)(sx + TILE_WIDTH), (float)(sy + TILE_HEIGHT / 2)}, fillColor, SDL_FPoint{0.0f, 0.0f} },
                { SDL_FPoint{(float)(sx + TILE_WIDTH / 2), (float)(sy + TILE_HEIGHT)}, fillColor, SDL_FPoint{0.0f, 0.0f} },
                { SDL_FPoint{(float)sx, (float)(sy + TILE_HEIGHT / 2)}, fillColor, SDL_FPoint{0.0f, 0.0f} }
            };
            int indices[6] = {0, 1, 2, 0, 2, 3};
            SDL_RenderGeometry(app->getRenderer(), NULL, vertices, 4, indices, 6);
        }});
    }

    for (const auto& building : buildings) {
        BuildingDiamond d = building.getBuildingDiamond();
        int sortY = (int)d.bottomY;
        renderEntries.push_back({sortY, [&building, d]() {
            int h = BUILDING_HEIGHT;
            SDL_Renderer* r = app->getRenderer();

            SDL_Color topColor    = {160, 150, 130, SDL_ALPHA_OPAQUE};
            SDL_Color leftColor   = {110, 100,  85, SDL_ALPHA_OPAQUE};
            SDL_Color rightColor  = { 80,  75,  65, SDL_ALPHA_OPAQUE};

            SDL_Vertex topVerts[4] = {
                { SDL_FPoint{d.topX,    d.topY - h},    topColor, SDL_FPoint{0,0} },
                { SDL_FPoint{d.rightX,  d.rightY - h},  topColor, SDL_FPoint{0,0} },
                { SDL_FPoint{d.bottomX, d.bottomY - h}, topColor, SDL_FPoint{0,0} },
                { SDL_FPoint{d.leftX,   d.leftY - h},   topColor, SDL_FPoint{0,0} }
            };
            int topIdx[6] = {0, 1, 2, 0, 2, 3};
            SDL_RenderGeometry(r, NULL, topVerts, 4, topIdx, 6);

            SDL_Vertex leftVerts[4] = {
                { SDL_FPoint{d.leftX,   d.leftY},   leftColor, SDL_FPoint{0,0} },
                { SDL_FPoint{d.bottomX, d.bottomY},  leftColor, SDL_FPoint{0,0} },
                { SDL_FPoint{d.bottomX, d.bottomY - h}, leftColor, SDL_FPoint{0,0} },
                { SDL_FPoint{d.leftX,   d.leftY - h},   leftColor, SDL_FPoint{0,0} }
            };
            int leftIdx[6] = {0, 1, 2, 0, 2, 3};
            SDL_RenderGeometry(r, NULL, leftVerts, 4, leftIdx, 6);

            SDL_Vertex rightVerts[4] = {
                { SDL_FPoint{d.bottomX, d.bottomY},  rightColor, SDL_FPoint{0,0} },
                { SDL_FPoint{d.rightX,  d.rightY},   rightColor, SDL_FPoint{0,0} },
                { SDL_FPoint{d.rightX,  d.rightY - h},  rightColor, SDL_FPoint{0,0} },
                { SDL_FPoint{d.bottomX, d.bottomY - h}, rightColor, SDL_FPoint{0,0} }
            };
            int rightIdx[6] = {0, 1, 2, 0, 2, 3};
            SDL_RenderGeometry(r, NULL, rightVerts, 4, rightIdx, 6);

            if (building.isProducing()) {
                float barX = d.cx - 30.0f;
                float barY = d.topY - BUILDING_HEIGHT - 16.0f;
                productionBar->setPosition(barX, barY);
                productionBar->render(r, (float)building.getProgress());
            }
        }});
    }

    std::sort(renderEntries.begin(), renderEntries.end(),
        [](const ISORenderEntry& a, const ISORenderEntry& b) { return a.sortY < b.sortY; });

    for (const auto& entry : renderEntries) {
        entry.render();
    }

    if (buttonPanel) {
        buttonPanel->render();
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
    
    // Set units context for flocking with separation
    for (auto& unit : units) {
        unit->setUnitsContext(&units);
    }

    std::srand(42);
    spawnObstacles(app->getRenderer(), 8);

    pathGrid = new PathGrid(MAP_RENDER_SIZE * 2);
    for (const auto& obs : obstacles) {
        pathGrid->markTile(obs.x * 2, obs.y * 2);
        pathGrid->markTile(obs.x * 2 + 1, obs.y * 2);
        pathGrid->markTile(obs.x * 2, obs.y * 2 + 1);
        pathGrid->markTile(obs.x * 2 + 1, obs.y * 2 + 1);
    }

    for (auto& unit : units) {
        unit->setObstaclesContext(&obstacles);
        unit->setPathGrid(pathGrid);
    }

    initMap();
    loadTileTexture(app->getRenderer());
    drawTimer = 0;

    productionBar = new LoadingBar(0, 0, 60.0f, 8.0f);
    buttonPanel = new ButtonPanel(app->getRenderer());

    buildings.push_back(Building(app->getRenderer(), 12, 12, 2, 2, 2000.0));
    Building& b = buildings.back();
    b.addButtonDef("./assets/img/pioneer.bmp", 64, 64,
        [&b]() {
            if (!b.isProducing()) {
                b.startProduction();
            }
        });

    for (const auto& building : buildings) {
        for (int tx = building.getTileX(); tx < building.getTileX() + building.getTileW(); tx++) {
            for (int ty = building.getTileY(); ty < building.getTileY() + building.getTileH(); ty++) {
                pathGrid->markTile(tx * 2, ty * 2);
                pathGrid->markTile(tx * 2 + 1, ty * 2);
                pathGrid->markTile(tx * 2, ty * 2 + 1);
                pathGrid->markTile(tx * 2 + 1, ty * 2 + 1);
            }
        }
    }

    collisionSound = new Sound("./assets/sounds/hit.wav");
    collisionSound->setupDevice();

    app->setEventCallback(handleEvents);
    app->setUpdateCallback(handleUpdates);
    app->setRenderCallback(handleRendering);
    app->runLoop();

    delete app;
    delete collisionSound;
    delete pathGrid;
    delete productionBar;
    delete buttonPanel;
    if (tileTexture) SDL_DestroyTexture(tileTexture);

    return 0;
}