#include "button_panel.h"

ButtonPanel::ButtonPanel(SDL_Renderer* renderer)
    : _renderer(renderer) {
    int totalW = PANEL_GRID_COLS * PANEL_SLOT_SIZE + (PANEL_GRID_COLS - 1) * PANEL_SLOT_GAP;
    int totalH = PANEL_GRID_ROWS * PANEL_SLOT_SIZE + (PANEL_GRID_ROWS - 1) * PANEL_SLOT_GAP;
    _originX = 1600 - totalW - 20;
    _originY = 900 - totalH - 20;
    _slots.resize(PANEL_GRID_ROWS * PANEL_GRID_COLS, nullptr);
}

void ButtonPanel::layoutSlot(int index, const ButtonDefinition& def) {
    int row = index / PANEL_GRID_COLS;
    int col = index % PANEL_GRID_COLS;
    int x = _originX + col * (PANEL_SLOT_SIZE + PANEL_SLOT_GAP);
    int y = _originY + row * (PANEL_SLOT_SIZE + PANEL_SLOT_GAP);

    Button* btn = new Button(_renderer, def.imagePath, x, y, def.width, def.height);
    btn->setOnClick(def.onClick);
    _slots[index] = btn;
}

void ButtonPanel::populate(const std::vector<ButtonDefinition>& definitions) {
    clear();
    for (size_t i = 0; i < definitions.size() && i < _slots.size(); i++) {
        layoutSlot(i, definitions[i]);
    }
}

void ButtonPanel::clear() {
    for (auto* slot : _slots) {
        delete slot;
    }
    std::fill(_slots.begin(), _slots.end(), nullptr);
}

void ButtonPanel::render() {
    for (int row = 0; row < PANEL_GRID_ROWS; row++) {
        for (int col = 0; col < PANEL_GRID_COLS; col++) {
            int x = _originX + col * (PANEL_SLOT_SIZE + PANEL_SLOT_GAP);
            int y = _originY + row * (PANEL_SLOT_SIZE + PANEL_SLOT_GAP);
            int idx = row * PANEL_GRID_COLS + col;

            if (_slots[idx]) {
                _slots[idx]->render(_renderer);
            } else {
                SDL_Rect slotRect = {x, y, PANEL_SLOT_SIZE, PANEL_SLOT_SIZE};
                SDL_SetRenderDrawColor(_renderer, 60, 60, 60, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(_renderer, &slotRect);
            }

            SDL_Rect borderRect = {x, y, PANEL_SLOT_SIZE, PANEL_SLOT_SIZE};
            SDL_SetRenderDrawColor(_renderer, 40, 40, 40, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawRect(_renderer, &borderRect);
        }
    }
}

bool ButtonPanel::handleClick(int px, int py) {
    for (auto* slot : _slots) {
        if (slot && slot->containsPoint(px, py)) {
            slot->handleClick();
            return true;
        }
    }
    return false;
}

int ButtonPanel::getOriginX() const { return _originX; }
int ButtonPanel::getOriginY() const { return _originY; }
int ButtonPanel::getWidth() const {
    return PANEL_GRID_COLS * PANEL_SLOT_SIZE + (PANEL_GRID_COLS - 1) * PANEL_SLOT_GAP;
}
int ButtonPanel::getHeight() const {
    return PANEL_GRID_ROWS * PANEL_SLOT_SIZE + (PANEL_GRID_ROWS - 1) * PANEL_SLOT_GAP;
}

ButtonPanel::~ButtonPanel() {
    clear();
}
