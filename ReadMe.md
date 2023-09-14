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

* [glfw](https://github.com/glfw/glfw.git): use to support create window and render context
* [gecs](https://github.com/VisualGMQ/gecs.git): basic ECS framework
* [mirrow](https://github.com/VisualGMQ/mirrow.git): TMP framework, use to do dynamic/static reflection and serialization
* [stb_image](http://nothings.org/stb): use to loading images
* [glad](https://glad.dav1d.de/): use to load opengl
* [dear-imgui](https://github.com/ocornut/imgui): use to support GUI in editor
* [tomlplusplus](https://github.com/marzer/tomlplusplus): use to support toml file parse