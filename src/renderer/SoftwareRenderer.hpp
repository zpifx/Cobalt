#pragma once
#include <cstdint>
#include <vector>
#include <algorithm>
#include <array>

struct Color { uint8_t r,g,b,a; };
inline uint32_t packRGBA(Color c) { return (uint32_t(c.a)<<24)|(uint32_t(c.b)<<16)|(uint32_t(c.g)<<8)|uint32_t(c.r); }

class SoftwareRenderer {
public:
    int width, height;
    std::vector<uint32_t> color;

    explicit SoftwareRenderer(int w, int h): width(w), height(h), color(w*h, 0) {}

    void resize(int w, int h) {
        width = w; height = h; color.assign(size_t(w)*h, 0);
    }

    void clear(Color c = {20,20,26,255}) {
        std::fill(color.begin(), color.end(), packRGBA(c));
    }

    // Simple pixel put with bounds
    inline void putPixel(int x, int y, uint32_t rgba) {
        if ((unsigned)x < (unsigned)width && (unsigned)y < (unsigned)height)
            color[y*width + x] = rgba;
    }

    // Tiny demo: filled triangle via edge functions (screen-space)
    struct Vec2 { float x,y; };
    void fillTriangle(Vec2 a, Vec2 b, Vec2 c, Color col) {
        auto toInt = [](float v){ return int(std::floor(v)); };
        int minX = std::max(0, std::min({toInt(a.x), toInt(b.x), toInt(c.x)}));
        int maxX = std::min(width-1, std::max({toInt(a.x), toInt(b.x), toInt(c.x)}));
        int minY = std::max(0, std::min({toInt(a.y), toInt(b.y), toInt(c.y)}));
        int maxY = std::min(height-1, std::max({toInt(a.y), toInt(b.y), toInt(c.y)}));

        auto edge = [](const Vec2& p, const Vec2& v0, const Vec2& v1){
            return (p.x - v0.x)*(v1.y - v0.y) - (p.y - v0.y)*(v1.x - v0.x);
        };
        uint32_t rgba = packRGBA(col);
        for (int y=minY; y<=maxY; ++y) {
            for (int x=minX; x<=maxX; ++x) {
                Vec2 p{float(x)+0.5f, float(y)+0.5f};
                float w0 = edge(p, b, c);
                float w1 = edge(p, c, a);
                float w2 = edge(p, a, b);
                if ((w0>=0 && w1>=0 && w2>=0) || (w0<=0 && w1<=0 && w2<=0)) {
                    putPixel(x,y,rgba);
                }
            }
        }
    }
};
