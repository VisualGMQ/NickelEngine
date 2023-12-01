# NickelEngine

An opensource, data-oriented ECS game engine.

## How-To-Build

use cmake:

```bash
git submodule update --init --recursive
cmake -S . -B cmake-build
cmake --build cmake-build
```

## 3rd Libraries

* [SDL2](https://github.com/libsdl-org/SDL): use to support create window, render context and basic input
* [gecs](https://github.com/VisualGMQ/gecs.git): basic ECS framework
* [mirrow](https://github.com/VisualGMQ/mirrow.git): TMP framework, use to do dynamic/static reflection and serialization
* [stb_image](http://nothings.org/stb): use to loading images
* [glad](https://glad.dav1d.de/): use to load opengl
* [dear-imgui](https://github.com/ocornut/imgui): use to support GUI in editor
* [tomlplusplus](https://github.com/marzer/tomlplusplus): use to support toml file parse
* [miniaudio](https://miniaud.io/): use to play audio cross platform
* [nanobench](https://nanobench.ankerl.com/): use to do benchmark
* [nameof](https://github.com/Neargye/nameof): use to get enum name convniently
* [freetype2](https://freetype.org/): use to render ttf font

## Features

* game engine features:
    - [x] 2D renderer based on OpenGL4
    - [x] Data Oriented based on ECS
    - [x] Animation applyed on arbitary properties
    - [x] Text render with UTF8 encoding
    - [ ] UI(WIP)
    - [ ] Game physics(WIP)
    - [ ] 3D rendering
    - [ ] RHI covered OpenGL3/ES and Vulkan
    - [ ] Game sound playing
* editor features:
    - [x] Entity list window to show entities
    - [x] Inspector window to show components of selected entity
    - [x] Game scene display
    - [ ] Assets window
    - [ ] Animation editor

## Editor

![editor](./snapshot/editor.png)