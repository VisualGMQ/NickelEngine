# 内存分配和资源管理

内存分配使用`DataPool`，会自动复用无效的内存。在每一帧最后回收引用计数为0的内存（未制作）

## 资源管理

使用`AssetManager`进行资源管理。

`Create`函数用来在内存中创建一份资源，但不和任何meta文件对应
`Load(path)`函数用来导入已有文件（如图片），并将`path`与其关联
`LoadFromMeta`函数用来从meta文件中加载资源。

一般来说，如果是程序自己创建的临时资源（如动画，临时读入的tilesheet），使用`Create`创建。
如果是可导入文件，使用`Load(path)`创建。用户代码中最常用的导入方式(如果是本身资源就是meta文件就会调用`LoadFromMeta`)。
如果是meta文件，使用`LoadFromMeta`。这个函数一般是提供给编辑器进行meta文件导入的。