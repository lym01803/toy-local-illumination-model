# README

## PlayWithGL_final 目录文件结构

```sh
.
├── ball-2.obj
├── ball.obj
├── compile_commands.json // 便于 vscode clangd 插件语法补全
├── cone.obj
├── cube.obj
├── cylinder.obj
├── include
│   ├── GLFW    // glfw 窗口、事件、计时等API
│   ├── KHR     // opengl 相关库
│   ├── glad    // glad 收集opengl API
│   ├── glm     // opengl vec mat 相关计算库
│   ├── loadobj.h   // 用于加载模型、场景
│   └── loadshader.h // 用于加载各阶段shader的glsl代码
├── l-ball.obj
├── lib
│   ├── libglad.a   // 预编译的静态链接库 (适配 arm64/x86_64 macOS 13)
│   └── libglfw3.a  // 预编译的静态链接库 (适配 arm64/x86_64 macOS 13)
├── main    // 可执行文件 (arm64 macOS 13)
├── main-arm64  // 可执行文件 (arm64 macOS 13)
├── main-universal // 可执行文件 (arm64/x86_64 macOS 13)
├── main-x86_64 // 可执行文件 (x86_64 macOS 13)
├── main.exe    // 可执行文件 (x86_64 Windows 11)
├── makefile    // 仅适用于 macOS，编译器版本: clang 14， c++版本: c++17
├── plane.obj
├── ring.obj
├── run.command  // macOS 脚本，可双击运行
├── scene-2.txt  // 场景：透明物体
├── scene.txt    // 场景：非透明物体
├── shader   // glsl
│   ├── fragment.glsl   // 光照模型主体
│   ├── shadowmap_fragment.glsl  
│   ├── shadowmap_vertex.glsl
│   └── vertex.glsl
├── src      // 源代码
│   ├── loadobj.cpp
│   ├── loadobj.o
│   ├── loadshader.cpp
│   ├── loadshader.o
│   ├── main.cpp
│   └── main.o
├── 酒杯.obj
└── 六边形柱体.obj
```

## 直接运行

**还是建议在 macOS 13 arm64 平台上跑，因为开发过程是在这个环境下进行的**

**窗口大小为 1920 x 1200, 透视投影也是按 16 : 10 来算的。如果比例不正，可以调节一下窗口长宽比；如果是在 macbook 上运行，强烈建议将窗口全屏**

### macOS

#### arm64 平台 终端启动
```sh
cd PlayWithGL_final
./main
```

#### x86_64 平台 终端启动
```sh
cd PlayWithGL_final
./main-universal
```

**或者直接双击 PlayWithGL_final 目录下的 run.command 文件**

*Note：注意，不要直接双击 main 文件，这样加载模型的相对路径会有问题*

*Note：你可能需要在安全性设置里面同意该可执行文件运行*

### Windows 11

**直接双击 main.exe**

*Note: 模型加载可能会花点时间 (1~5秒不等，与平台和二进制文件版本以及运行设备性能有关)，因为其中一个球有16万面...*


## 编译运行

### macOS
打包glfw3和glad的预编译静态链接库都是通用架构的，可以直接link。
建议在 macOS 13 下 使用 clang 14 编译.

####

```sh
cd PlayWithGL_final
make clean
make
./main
```

### Windows 11

暂不支持，没有打包相应的预编译静态链接库，makefile 也不能直接运行。

## 调整视角

W: 上， S: 下，A: 左，D:右， E: 远, Q: 近

固定朝向坐标原点

## 通过修改 scene.txt, scene-2.txt 调整场景布置

scene.txt 是非透明物体

scene-2.txt 是半透明物体

格式很简单，一看就明白


