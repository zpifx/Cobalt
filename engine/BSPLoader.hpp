#pragma once
#include "BspCommon.hpp"
#include <optional>

class IBspLoader {
public:
    virtual ~IBspLoader() = default;
    virtual bool canLoad(std::span<const uint8_t> bytes) const = 0;
    virtual std::optional<BspInfo> parse(std::span<const uint8_t> bytes) const = 0;
};

std::unique_ptr<IBspLoader> MakeBspLoader_Q1();
std::unique_ptr<IBspLoader> MakeBspLoader_Q2();
std::unique_ptr<IBspLoader> MakeBspLoader_Q3();
std::unique_ptr<IBspLoader> MakeBspLoader_GoldSrc();

// Dispatch that tries all loaders in a safe order
std::optional<BspInfo> LoadAnyBsp(const std::filesystem::path& file);
