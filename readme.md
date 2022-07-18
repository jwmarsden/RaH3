# Kinetic-Vulkan

This a personal repository of learnings. It currently includes; 

## Elements

* C++17 (https://en.cppreference.com/w/cpp/17)
* GLFW to manage the window across platforms and handle I/O - its not just an OpenGL library (https://www.glfw.org/)
* Vulkan; do it (https://www.vulkan.org/)
* glm for math - its not just an OpenGL library (https://github.com/g-truc/glm)
* Dear ImGui like everyone else in the universe at the moment (https://github.com/ocornut/imgui) 
* spdlog for logging (https://github.com/gabime/spdlog)
* entt for Entity Component Framework (https://github.com/skypjack/entt)

This is all built in CMake (https://cmake.org/) other than the C++17 compiler, CMake & Vulkan. 

## Requirements

1. You need Git and a Build Chain for C++17. You need CMake and this example uses Make. 
    * On my Mac I use the builtin Clang that I think come with XCode. I suggest you install XCode from the App Store. 
    * On Windows I use the C++ Community Edition of the time from Microsoft (https://visualstudio.microsoft.com/vs/community/).
    * On Linux, I install the build-essential package on Ubuntu. GLFW and Vulkan have a bunch more requirements to compile - its not overly hard to figure it out. 
2. You need a decent graphics card. Install the Vulkan SDK (https://vulkan.lunarg.com/) on your machine. 

## Get the Source

All of the required packages (including GLFW and glm) have been configured as CMake subdirectories which are managed as Git submodules. This makes our lives easy in many ways but it means that each of the dependencies needs to also be cloned. 

```
git clone --recursive https://github.com/jwmarsden/Kinetic-Vulkan.git
```

### Whoops, I didn't do recursive

```
➜  cplusplus cd Kinetic-Vulkan
➜  Kinetic-Vulkan git:(main) ✗ git submodule update --init --recursive
```

## Building (Release Build)

On a Mac, open a terminal in the source folder. 

```
➜  Kinetic-Vulkan git:(main) ✗ cmake -Bbuild .
➜  Kinetic-Vulkan git:(main) ✗ cd build
➜  build git:(main) ✗ make .
➜  build git:(main) ✗ ./kinetic
```

The binary will now be in the build folder. You will find it in the main directory or a subdirectory for the Release depending on your selected Build Chain.