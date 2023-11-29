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

## Features

* Finished:
    * 2D renderer based on OpenGL4
    * Data Oriented based on ECS
    * Animation applyed on arbitary properties
* WIP:
    * UI
    * RHI covered OpenGL3/ES and Vulkan
    * 3D rendering
    * Customizable render pipeline
    * Game sound play
    * Game physics
    * Text render
    * More features of the editor

## Editor

features:

* has a inspector to see/change component properties

![editor](./snapshot/editor.png)