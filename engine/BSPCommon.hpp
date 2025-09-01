#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <span>
#include <filesystem>

enum class BspFlavor { Quake1, Quake2, Quake3, GoldSrc };

struct Lump { int32_t offset=0; int32_t length=0; };

struct BspInfo {
    BspFlavor flavor;
    std::string mapName;
    int32_t version=0;
    std::vector<Lump> lumps;
    std::vector<uint8_t> raw; // entire file buffer for quick slicing
};
