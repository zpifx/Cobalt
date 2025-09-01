#include <cstdio>
#include <chrono>
#include <vector>
#include <string>
#include <optional>
#include <filesystem>

#include <SDL.h>
#include "platform/SDLApp.hpp"
#include "renderer/SoftwareRenderer.hpp"

// Dear ImGui
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"

struct Blit {
    SDL_Texture* tex = nullptr;
    int w=0, h=0;

    void ensure(SDL_Renderer* r, int W, int H) {
        if (tex && (W!=w || H!=h)) { SDL_DestroyTexture(tex); tex=nullptr; }
        if (!tex) {
            tex = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, W, H);
            w=W; h=H;
            SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_NONE);
        }
    }
    void upload(SDL_Renderer* r, const std::vector<uint32_t>& buf, int pitchPx) {
        void* pixels=nullptr; int pitch=0;
        if (SDL_LockTexture(tex, nullptr, &pixels, &pitch)==0) {
            // pitch is in bytes; our buffer is tightly packed RGBA8888
            uint8_t* dst = static_cast<uint8_t*>(pixels);
            const uint8_t* src = reinterpret_cast<const uint8_t*>(buf.data());
            const int rowBytes = pitchPx * 4;
            for (int y=0; y<h; ++y) {
                std::memcpy(dst + y*pitch, src + y*rowBytes, rowBytes);
            }
            SDL_UnlockTexture(tex);
        }
        SDL_RenderCopy(r, tex, nullptr, nullptr);
    }
};

int main(int argc, char** argv) try {
    (void)argc; (void)argv;

    SDLApp app("Quake-like SW Engine (SDL2 + ImGui)", 1280, 720, true);

    // ImGui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = "imgui.ini";

    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(app.window, app.renderer);
    ImGui_ImplSDLRenderer2_Init(app.renderer);

    // Our software framebuffer at a logical resolution
    int fbW = 800, fbH = 450; // 16:9; scale up by SDL_Renderer
    SoftwareRenderer sw(fbW, fbH);
    Blit blit;

    bool running = true;
    bool showDemo = false;
    float triAngle = 0.0f;
    float scale = 1.0f; // future: DPI scale or renderScale

    auto last = std::chrono::high_resolution_clock::now();

    while (running) {
        // --- Events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                app.winW = e.window.data1; app.winH = e.window.data2;
            }
        }

        // --- Timing
        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - last).count();
        last = now;

        // --- Update (example: spin a triangle)
        triAngle += dt * 1.0f;

        // --- Software render
        sw.clear({18,18,24,255});
        float cx = sw.width * 0.5f;
        float cy = sw.height * 0.5f;
        float r = std::min(sw.width, sw.height) * 0.3f * scale;
        auto rot = [triAngle, cx, cy, r](float a){
            float ca = std::cos(a+triAngle), sa = std::sin(a+triAngle);
            return SoftwareRenderer::Vec2 { cx + ca*r, cy + sa*r };
        };
        sw.fillTriangle(rot(0.0f), rot(2.094f), rot(4.188f), {200,120,40,255});

        // --- Begin ImGui
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Overlay
        ImGui::Begin("Engine");
        ImGui::Text("Quake-like SW Engine (WIP)");
        ImGui::Text("FPS: %.1f", 1.0f / std::max(1e-6f, dt));
        ImGui::SliderFloat("Triangle Speed", &triAngle, 0.0f, 100.0f);
        ImGui::SliderFloat("Render Scale", &scale, 0.25f, 2.0f);
        if (ImGui::Button("ImGui Demo")) showDemo = !showDemo;
        ImGui::Separator();
        ImGui::Text("Targets to support:");
        ImGui::BulletText("Quake 1 (BSP29/30) + lightmaps");
        ImGui::BulletText("Quake 2 (IBSP v38)");
        ImGui::BulletText("Quake 3 (IBSP v46) + shaders");
        ImGui::BulletText("GoldSrc (HL1 variant of Q1)");
        ImGui::End();

        if (showDemo) ImGui::ShowDemoWindow(&showDemo);

        ImGui::Render();

        // --- Present: upload SW buffer to a streaming texture, draw, then ImGui
        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
        SDL_RenderClear(app.renderer);

        blit.ensure(app.renderer, sw.width, sw.height);
        blit.upload(app.renderer, sw.color, sw.width);

        // ImGui on top
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

        SDL_RenderPresent(app.renderer);
    }

    // Cleanup ImGui
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
catch (const std::exception& ex) {
    std::fprintf(stderr, "Fatal: %s\n", ex.what());
    return 1;
}
