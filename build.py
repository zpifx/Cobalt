import os
import subprocess
import sys
import glob

# Paths
SRC_DIR = "src"
SRC_CPP = ' '.join(glob.glob(os.path.join(SRC_DIR, "*.cpp")))
IMGUI_DIR = "imgui"
TARGET = "Co"

# ImGui repository
IMGUI_REPO = "https://github.com/ocornut/imgui.git"

# Compiler and flags
COMPILER = "g++"
CXXFLAGS = [
    "-I.", f"-I{IMGUI_DIR}", f"-I{IMGUI_DIR}/backends",
    "`sdl2-config --cflags`", "-Wall", "-Wextra", "-O2"
]
LDFLAGS = [
    "`sdl2-config --libs`", "-lGL", "-lGLEW", "-ldl", "-lm"
]

def run_cmd(cmd, check=True, shell=False):
    print(f"[RUN] {cmd}")
    subprocess.run(cmd, check=check, shell=shell)

def clone_imgui():
    if not os.path.exists(IMGUI_DIR):
        print(f"=== Cloning Dear ImGui into {IMGUI_DIR} ===")
        run_cmd(["git", "clone", IMGUI_REPO, IMGUI_DIR])
    else:
        print(f"=== Dear ImGui already exists in {IMGUI_DIR}, pulling latest ===")
        run_cmd(["git", "-C", IMGUI_DIR, "pull"])

def build_project():
    print("=== Building project ===")
    imgui_sources = [
        "imgui.cpp", 
        "imgui_draw.cpp", 
        "imgui_tables.cpp",
        "imgui_widgets.cpp",
    ]
    backend_sources = ["imgui_impl_sdl2.cpp", "imgui_impl_opengl3.cpp"]

    sources = [SRC_CPP] \
              + [f"{IMGUI_DIR}/{src}" for src in imgui_sources] \
              + [f"{IMGUI_DIR}/backends/{src}" for src in backend_sources]

    cmd = f"{COMPILER} {' '.join(sources)} {' '.join(CXXFLAGS)} {' '.join(LDFLAGS)} -o {TARGET}"
    run_cmd(cmd, shell=True)

def run_executable():
    print("=== Running exe ===")
    run_cmd([f"./{TARGET}"])

if __name__ == "__main__":
    clone_imgui()
    build_project()
    run_executable()

