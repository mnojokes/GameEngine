# GameEngine

## Introduction

This is a simple 2D game engine, created in tandem with my software engineering studies to test and apply various concepts and use it as an easy-to-use solution visualizer for various programming problems encountered during my studies. It uses DirectX11 for graphics rendering and employs many of the patterns found in well-known commercial engines, such as an entity component system architecture, object factories, objects from serialized data, internal messaging, pathfinding. In addition to this, it uses multiple third-party libraries: [DirectXTex](https://github.com/microsoft/DirectXTex), [Dear ImGui](https://github.com/ocornut/imgui), [RapidJSON](https://rapidjson.org/), [INIH](https://github.com/benhoyt/inih). This entire project is intended solely as proof of my ability to learn and then apply and combine this knowledge in practice, and should not be expected to work as a full-featured professional tool.

## Running included projects

The solution should be opened with Visual Studio for Windows. Building an included project places the executable in the Output folder within the solution directory. The Debug configuration opens a console window for the debugging output. Build and run the Release configuration to run the projects without the debugging console window.

## Included projects

### BootlegArkanoid

A simple game based on the classic Arkanoid logic of bouncing a ball with a paddle and colliding it with bricks to destroy them. Created as the initial testing stage for the engine, it required developing a basic velocity and collision physics system to make it work. The game contains 3 levels. Paddle is controlled with the left and right arrow keys (Debug config allows vertical paddle movement as well - up/down arrow keys).

### Pathfinder

A side-project for my Algorithms studies. It was created to visualize various pathfinding algorithms (currently supports AStar, Greedy BFS, and Dijkstra). It contains 3 pathfinding "maps", however, it can also accept plain-text user-created maps. Files should have a .pfmap extension and be saved in Assets/Pathfinder/Maps/. Check existing files to understand their structure. It was not optimized for graphics performance, so unreasonably large maps with many tiles will be slower.
