# GameEngine

[Running included projects](https://github.com/mnojokes/GameEngine/edit/main/README.md#running-included-projects)

## Introduction

This is a simple 2D game engine, created in tandem with my software engineering studies to test and apply various concepts and use it as an easy-to-use solution visualizer for various programming problems encountered during my studies. It uses DirectX11 for graphics rendering and employs many of the patterns found in well-known commercial engines, such as an entity component system architecture, object factories, objects from serialized data, internal messaging, pathfinding. In addition to this, it uses multiple third-party libraries: [DirectXTex](https://github.com/microsoft/DirectXTex), [Dear ImGui](https://github.com/ocornut/imgui), [RapidJSON](https://rapidjson.org/), [INIH](https://github.com/benhoyt/inih). This entire project is intended solely as proof of my ability to learn and then apply and combine this knowledge in practice, and should not be expected to work as a full-featured professional tool.

### Key parts

#### Game Object Factory

The Game Object Factory creates, manages, and destroys game objects. Default objects can be created from archetypes, additionally each object can be overriden with customized parameters as required by a particular level/location of the game.
- [GameObjectFactory.h](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/GameObjectFactory.h)
- [GameObjectFactory.cpp](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/GameObjectFactory.cpp)
- [GameObject.h](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/GameObject.h) - GameObject base class
- [GameObject.cpp](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/GameObject.cpp)
- [Component.h](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/Component.h) - Component base class
- [Component.cpp](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/Component.cpp)

#### Messenger

The Messenger acts as a message broker for the different components of the engine. Systems, game objects, and components do not interact with each other directly, which allows parts of the engine to be maintained, changed, or replaced easily without breaking the others. 
- [Messenger.h](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/Messenger.h)
- [Messenger.cpp](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/Messenger.cpp)
- [Message.h](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/Message.h) - message interface

#### Systems
- [ISystem.h](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/ISystem.h) - system interface

**File System** - handles the engine and game files. Can notify other systems via the Messenger when it locates certain files that they requested.
- [FileSystem.h](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/FileSystem.h)
- [FileSystem.cpp](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/FileSystem.cpp)

**Gameplay System** - handles the gameplay logic. The intended use is for the client to create a derived class where they would implement the gameplay logic as they see fit.
- [GameplaySystem.h](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/GameplaySystem.h)
- [GameplaySystem.cpp](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/GameplaySystem.cpp)

**Graphics System** - processes the visual part of the engine, renders objects that contain a valid GraphicsComponent on display.
- [GraphicsSystem.h](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/GraphicsSystem.h)
- [GraphicsSystem.cpp](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/GraphicsSystem.cpp)

**Physics System** - handles the movement of objects containing a valid PhysicsComponent, resolves the basic collisions of objects containing a Collider.
- [PhysicsSystem.h](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/PhysicsSystem.h)
- [PhysicsSystem.cpp](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/PhysicsSystem.cpp)

#### World Manager
Manages different Worlds (levels) in the game. Reads world data from files, requests object creation/destruction.
- [WorldManager.h](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/WorldManager.h)
- [WorldManager.cpp](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/WorldManager.cpp)
- [World.h](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/World.h)
- [World.cpp](https://github.com/mnojokes/GameEngine/blob/main/GameEngine/World.cpp)

## Running included projects

The solution should be opened with Visual Studio for Windows. Building an included project places the executable in the Output folder within the solution directory. The Debug configuration opens a console window for the debugging output. Build and run the Release configuration to run the projects without the debugging console window.

## Included projects

### BootlegArkanoid

A simple game based on the classic Arkanoid logic of bouncing a ball with a paddle and colliding it with bricks to destroy them. Created as the initial testing stage for the engine, it required developing a basic velocity and collision physics system to make it work. The game contains 3 levels. Paddle is controlled with the left and right arrow keys (Debug config allows vertical paddle movement as well - up/down arrow keys).

### Pathfinder

A side-project for my Algorithms studies. It was created to visualize various pathfinding algorithms (currently supports AStar, Greedy BFS, and Dijkstra). It contains 3 pathfinding "maps", however, it can also accept plain-text user-created maps. Files should have a .pfmap extension and be saved in Assets/Pathfinder/Maps/. Check existing files to understand their structure. It was not optimized for graphics performance, so unreasonably large maps with many tiles will be slower.
