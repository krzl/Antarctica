# Antarctica
Work-in-progress 3D game engine made with RTS-like games in mind.

Major Features:
- Logic implemented using Entity Component System
- Rendering using DirectX12
- Skeletal animations calculated partially on gpu using compute shaders
- A* Pathfinding using navmesh created from constrained Delaunay triangulation
- Steering of entities using different behaviours to simulate crowd movement
- Simple physics system to resolve entities overlapping with each other and non-navigable terrain

[Sample video](https://www.youtube.com/watch?v=-js0Xm-ugOM)

## Running Project

Prebuild executable is available in [releases page](https://github.com/krzl/Antarctica/releases)

## Building Project

##### Building Visual Studio solution requires CMake [Click here to download CMake.](https://cmake.org/download/) or [Install using Visual Studio Installer](https://learn.microsoft.com/en-us/cpp/linux/download-install-and-setup-the-linux-development-workload?view=msvc-170/) 

Clone this repo, and make sure that submodules are properly initialized

 ```bash
git clone --recurse-submodules https://github.com/Antarctica
```

Generate Visual Studio solution

 ```bash
cmake CMakeLists.txt
```

Open Antarctica.sln , change build configuration to Release, make sure that Game is selected as Startup Project

On [releases page](https://github.com/krzl/Antarctica/releases) there is also available prebuild visual studio solution, however it requires to be unpacked in C:/Antarctica folder

## Controls

- Middle mouse button or arrow keys - camera movement
- Scroll wheel - camera zoom in/out
- Left button mouse drag - select entities
- Right click - issue move command to selected entities
