cmake_minimum_required(VERSION 3.16)
project(quake_sw_engine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL2
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS})

# Dear ImGui (you: git clone https://github.com/ocornut/imgui into extern/imgui)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)

file(GLOB ENGINE_HEADERS engine/*.hpp)
file(GLOB RENDERER_SRC src/renderer/*.cpp src/renderer/*.hpp)
file(GLOB PLATFORM_HEADERS src/platform/*.hpp)

add_executable(quake_sw
    src/main.cpp
    ${RENDERER_SRC}
    ${ENGINE_HEADERS}
    ${PLATFORM_HEADERS}
    ${IMGUI_SOURCES}
)

target_include_directories(quake_sw PRIVATE src engine)

target_link_libraries(quake_sw
    ${SDL2_LIBRARIES}
)

if (UNIX AND NOT APPLE)
    target_link_libraries(quake_sw pthread dl)
endif()
