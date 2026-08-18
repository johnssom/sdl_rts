#include "sdl_app.h"

SDLApp::SDLApp(Uint32 subsystemFlags, char* title, int windowWidth, int windowHeight) {
    _title = title;
    _windowWidth = windowWidth;
    _windowHeight = windowHeight;
    if (SDL_Init(subsystemFlags) != 0) {
        std::cout << "Failed to initialize SDL. ERROR: (" << SDL_GetError() << ")\n";
        exit(1);
    }
    if (TTF_Init() == -1) {
        std::cout << "Could not initialize SDL TTF. ERROR: (" << TTF_GetError() << ")\n";
        exit(1);
    }
    setOpenGLAttributes();
    _window = SDL_CreateWindow(_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _windowWidth, _windowHeight, SDL_WINDOW_OPENGL);
    _context = SDL_GL_CreateContext(_window);
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    glViewport(0, 0, _windowWidth, _windowHeight);
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
}

SDL_Renderer* SDLApp::getRenderer() const {
    return _renderer;
}

int SDLApp::getWindowWidth() {
    return _windowWidth;
}

int SDLApp::getWindowHeight() {
    return _windowHeight;
}

int SDLApp::getMouseX() {
    return _mouseX;
}

int SDLApp::getMouseY() {
    return _mouseY;
}

void SDLApp::setOpenGLAttributes() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
}

void SDLApp::setMaxFrameRate(Uint32 frameRate) {
    _minLoopDelay = (1 / frameRate) * 1000;
}

void SDLApp::setEventCallback(std::function<void(void)> callback) {
    _eventCallback = callback;
}

void SDLApp::setRenderCallback(std::function<void(void)> callback) {
    _renderCallback = callback;
}

void SDLApp::setUpdateCallback(std::function<void(void)> callback) {
    _updateCallback = callback;
}

void SDLApp::runLoop() {
    _isGameRunning = true;
    while (_isGameRunning) {
        Uint32 startTime = SDL_GetTicks();
        SDL_GetMouseState(&_mouseX, &_mouseY);
        _eventCallback();
        _updateCallback();
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(_renderer);
        _renderCallback();
        SDL_RenderPresent(_renderer);
        Uint32 elapsedTime = SDL_GetTicks() - startTime;

        if (elapsedTime < _minLoopDelay) {
            SDL_Delay(_minLoopDelay - elapsedTime);
        }
    }
}

void SDLApp::quitGame() {
    _isGameRunning = false;
}

SDLApp::~SDLApp() {
    SDL_DestroyWindow(_window);
    SDL_Quit();
}