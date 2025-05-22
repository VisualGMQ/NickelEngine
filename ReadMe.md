NickelEngine重写中，老版本备份在[NickelEngine-backup](https://github.com/VisualGMQ/NickelEngine-backup)

更多信息请参阅[wiki](https://github.com/VisualGMQ/NickelEngine/wiki)

新NickelEngine的特点：

* 重置底层ECS系统
    [现在的ECS](https://github.com/VisualGMQ/gecs)是借鉴的EnTT，我希望能够改成Flecs那种基于Archetype的底层
* 重置反射系统
    [老的反射系统](https://github.com/VisualGMQ/mirrow)需要一套自动解析头文件的工具，以及更多的特性
* 重置渲染
    老的渲染层中Vulkan和GL/GLES并存。新版本打算只使用Vulkan
* 更好的工程结构
    老的第三方库作为submodule，由于网络问题及git submodule问题经常拉不下来/拉一半。新工程中使用小型第三方库，并且将库源码直接放在引擎工程中，这样既避免了额外拉取问题，又可以进行源码调试
* 更好的开发流程
    新版本每次开发功能/修改bug都会关联issue，以方便后续溯源。CI也从最开始就使用，保证main分支一定是可编译通过的

## How To Build

使用CMake进行编译（非安卓平台在`engine`目录下执行：）

```bash
cmake --preset=default
```

若需要编译Editor，需要安装Qt6.9，使用Qt Creator打开并构建

如果是纯CMake环境，需要指定Qt的工具链路径：

```bash
cmake -S . -B cmake-build -DNICKEL_BUILD_TOOLS=ON -DQT6_PATH=<your qt path>
```

使用CMake构建：

```bash
cmake --build cmake-build
```

安卓平台需要将`engine`文件夹拷贝到`android-project/app/jni`下然后开启`Android Studio`进行编译

## How To Run

所有可执行文件均在`engine`目录下运行。

VS及基于MSBuild的IDE（如Rider）会自动设置路径，可直接在IDE里面运行。
