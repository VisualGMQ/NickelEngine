# NickelEngine

An opensource, data-oriented ECS game engine.

## How-To-Build

use cmake:

```bash
git submodule update --init --recursive
cmake -S . -B cmake-build
cmake --build cmake-build
```

in some system, you may want to copy needed dll to your output dir, use:

```bash
cmake -S . -B cmake-build -DNICKEL_COPYDLL=ON
```

to enable auto copy dlls

## 3rd Libraries

|library|description|
|--|--|
|[SDL2](https://github.com/libsdl-org/SDL)| use to support create window, render context and basic input|
|[gecs](https://github.com/VisualGMQ/gecs.git)| basic ECS framework|
|[mirrow](https://github.com/VisualGMQ/mirrow.git)| TMP framework, use to do dynamic/static reflection and serialization|
|[stb_image](http://nothings.org/stb)| use to loading images|
|[glad](https://glad.dav1d.de/)| use to load opengl|
|[dear-imgui](https://github.com/ocornut/imgui)| use to support GUI in editor|
|[miniaudio](https://miniaud.io/)| use to play audio cross platform|
|[nanobench](https://nanobench.ankerl.com/)| use to do benchmark|
|[nameof](https://github.com/Neargye/nameof)| use to get enum name convniently|
|[freetype2](https://freetype.org/)| use to render ttf font|
|[lunasvg](https://github.com/sammycage/lunasvg)| use to render svg|
|[rapidxml](https://rapidxml.sourceforge.net/)| use to parse XML|
|[tomlplusplus](https://github.com/marzer/tomlplusplus)| use to support toml file parse|

editor icon: [vscode-material-icon-theme](https://github.com/PKief/vscode-material-icon-theme)

## Features

- [x] 2D renderer based on OpenGL4
- [x] Data Oriented based on ECS
- [x] Animation applyed on arbitary properties
- [x] Text render with UTF8 encoding
- [x] Prefab save/load
- [ ] UI(WIP)
- [ ] Game physics(WIP)
- [ ] RHI covered OpenGL3/ES and Vulkan(WIP)
- [ ] 3D rendering
- [ ] Game sound playing
- [ ] Scene change/load/save
    - [x] scene save/load
    - [ ] multiple scene
    - [ ] scene chagne
- [ ] script
    - [ ] cpp(dynamic library)
    - [ ] lua/quickJs/C#mono (don't determine currently)

## Editor

* features:
    - [x] Entity list window
        - [x] show entities
        - [x] drag entity to reorder/set hierarchy
    - [x] Inspector window to show components of selected entity
    - [x] Game scene display
    - [x] Assets window
        - [x] show assets
        - [x] load/release asset
        - [ ] asset property window
            - [x] texture asset property window
            - [ ] font asset property window
            - [ ] audio asset property window
            - [ ] script asset property window
    - [ ] Animation editor
    - [x] Project Save/Load
        - [x] asset save/load
        - [x] scene(entities) save/load

![editor](./snapshot/editor.png)