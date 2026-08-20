#ifndef BUTTON_PANEL_H
#define BUTTON_PANEL_H

#include <vector>
#include <string>
#include <functional>
#include <SDL2/SDL.h>

#include "button.h"

#define PANEL_GRID_ROWS 3
#define PANEL_GRID_COLS 3
#define PANEL_SLOT_SIZE 64
#define PANEL_SLOT_GAP  4

struct ButtonDefinition {
    std::string imagePath;
    int width, height;
    std::function<void()> onClick;
};

class ButtonPanel {
    private:
        SDL_Renderer* _renderer;
        int _originX, _originY;
        std::vector<Button*> _slots;

        void layoutSlot(int index, const ButtonDefinition& def);

    public:
        ButtonPanel(SDL_Renderer* renderer);
        void populate(const std::vector<ButtonDefinition>& definitions);
        void clear();
        void render();
        bool handleClick(int px, int py);
        int getOriginX() const;
        int getOriginY() const;
        int getWidth() const;
        int getHeight() const;
        ~ButtonPanel();
};

#endif
