#ifndef SDL_APP_H
#define SDL_APP_H

#include <iostream>
#include <functional>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <glad/glad.h>

class SDLApp {
    private:
        char* _title;
        int _windowWidth;
        int _windowHeight;
        bool _isGameRunning = false;
        Uint32 _minLoopDelay = (1 / 60) * 1000;
        SDL_Window* _window = nullptr;
        SDL_Renderer* _renderer = nullptr;
        SDL_GLContext _context = nullptr;
        std::function<void(void)> _eventCallback;
        std::function<void(void)> _updateCallback;
        std::function<void(void)> _renderCallback;
        int _mouseX;
        int _mouseY;

        void setOpenGLAttributes();
        
        public:
        SDLApp(Uint32 subsystemFlags, char* title, int windowWidth, int windowHeight);
        SDL_Renderer* getRenderer() const;
        int getWindowWidth();
        int getWindowHeight();
        int getMouseX();
        int getMouseY();
        void setMaxFrameRate(Uint32 framRate);
        void setEventCallback(std::function<void(void)> callback);
        void setUpdateCallback(std::function<void(void)> callback);
        void setRenderCallback(std::function<void(void)> callback);
        void runLoop();
        void quitGame();
        ~SDLApp();
};

#endif