# NickelEngine

a game engine based on ECS

## How To Build

This project rely on `glslc` & `python3`, so you need install them firstly.

Install python dependencies:

```bash
pip install -r ./engine/engine/code_generator/requirements.txt
```

Then use cmake to build:

```bash
cmake --preset=default
cmake --build cmake-build

# build this doc
cmake --build cmake-build --target doc
```

## Examples

All examples should run under `engine/` dir.

You can run `sandbox` target to open a sandbox world.

There also some graphics test you can run using `ctest`:

```bash
cd cmake-build
ctest -C Debug -R <test_name>
```

- triangle: show a colorful triangle
- rectangle: show a pure color rectangle
- texture_rect: show a rect with texture
- colorful_rectangle: show a colorful rectangle by uniform
- colorful_rectangle2: show a colorful rectangle by push_constants
- cube3d: show a 3D cube
- skybox: show skybox
- \ref GLTFTest : show a gltf model

## Engine Architecture

Engine architecture and structures explaination are \ref EngineArchitecture