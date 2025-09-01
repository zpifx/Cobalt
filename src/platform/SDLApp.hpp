#pragma once
#include <SDL.h>
#include <stdexcept>
#include <string>

struct SDLApp {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int winW = 1280, winH = 720;

    SDLApp(const char* title, int w, int h, bool vsync = true) : winW(w), winH(h) {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0)
            throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());

        Uint32 flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);
        if (!window) throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());

        Uint32 rflags = SDL_RENDERER_ACCELERATED; // You can switch to SDL_RENDERER_SOFTWARE if desired
        if (vsync) rflags |= SDL_RENDERER_PRESENTVSYNC;
        renderer = SDL_CreateRenderer(window, -1, rflags);
        if (!renderer) throw std::runtime_error(std::string("SDL_CreateRenderer failed: ") + SDL_GetError());
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    }

    ~SDLApp() {
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }
};
