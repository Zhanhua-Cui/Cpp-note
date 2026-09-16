# CMake 入门（针对零基础）

> 你是从「一键运行 IDE」过来的，这一篇用最小篇幅让你会用 CMake，并看懂本仓库的 `CMakeLists.txt`。

## 1. CMake 到底是什么

一句话：**CMake 是一个「生成构建文件」的工具，它自己不编译代码。**

- 你写一个 `CMakeLists.txt`（描述「要编译哪些文件、用什么标准、产出什么」）；
- CMake 读它，生成实际构建系统（macOS 上通常是 Makefile 或 Xcode 工程）；
- 再由 make / ninja / Xcode 真正调用编译器 `g++`/`clang++` 完成编译。

所以流程永远是三步：**配置（cmake）→ 构建（build）→ 运行**。

## 2. 先安装

macOS 用 Homebrew（如果你装了 brew）：

```bash
brew install cmake
```

验证：

```bash
cmake --version
```

> 没装 Homebrew？先装它：<https://brew.sh/> 。CMake 是后面所有单元的地基，必须装。

## 3. 三个核心概念

| 概念 | 是什么 | 类比 |
|---|---|---|
| **CMakeLists.txt** | 构建描述文件，一个项目一份 | 一份「怎么编译」的说明书 |
| **target（目标）** | 编译产物，如可执行文件、库 | 你要的「最终成品」 |
| **变量** | `set(名字 值)` 存的值 | 普通变量 |

## 4. 最小例子（10 秒看懂）

一个文件 `main.cpp`，配一个 `CMakeLists.txt`：

```cmake
cmake_minimum_required(VERSION 3.16)   # 声明最低 CMake 版本
project(hello CXX)                     # 项目名 + 语言是 C++

add_executable(hello main.cpp)         # 核心！生成可执行文件 hello
```

构建：

```bash
cmake -B build        # 第1步：配置，生成到 build/ 目录
cmake --build build   # 第2步：真正编译
./build/hello         # 第3步：运行
```

> `cmake -B build` 里的 `-B` 指定「构建目录」。所有生成物都堆在 `build/` 里，不污染源码目录（`.gitignore` 已忽略它）。

## 5. 逐行读懂本仓库的 CMakeLists.txt

打开根目录的 `CMakeLists.txt`，配合下面注释看：

```cmake
# 1) 最低版本 + 项目名
cmake_minimum_required(VERSION 3.16)
project(cpp_note CXX)

# 2) 指定用 C++17
set(CMAKE_CXX_STANDARD 17)          # 语言标准
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 编译器不支持就报错，别悄悄降级
set(CMAKE_CXX_EXTENSIONS OFF)       # 关掉编译器私有扩展，更标准

# 3) 默认 Debug 模式（方便调试 + 配合 sanitizer）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 4) 开启警告：-Wall -Wextra 帮你早抓 bug
add_compile_options(-Wall -Wextra)

# 5) 每个 .cpp 各生成一个可执行文件（target）
add_executable(01_raii          知识库/代码/01_raii.cpp)
add_executable(02_copy_move     知识库/代码/02_copy_move.cpp)
add_executable(03_smart_ptr     知识库/代码/03_smart_ptr.cpp)
add_executable(04_modern_syntax 知识库/代码/04_modern_syntax.cpp)
add_executable(05_stl           知识库/代码/05_stl.cpp)
```

`add_executable(名字 源文件)` 就是「把某几个源文件编译成一个可执行文件」。这里是 5 个独立示例，所以 5 个 target。

## 6. 用起来（你现在就能跑）

```bash
cd 到仓库根目录
cmake -B build          # 配置
cmake --build build     # 编译（5 个示例一起编好）
./build/01_raii         # 运行任意一个
./build/02_copy_move
```

以后每次改了代码，只需重复 `cmake --build build`（配置一般只做一次）。

## 7. 常用命令速查

| 命令 | 作用 |
|---|---|
| `cmake -B build` | 配置（生成构建文件） |
| `cmake --build build` | 编译 |
| `cmake --build build -j 8` | 并行编译（更快） |
| `cmake -B build -DCMAKE_BUILD_TYPE=Release` | 配置成 Release（优化） |
| `cmake -B build -DCMAKE_CXX_FLAGS="-fsanitize=address"` | 传额外编译选项（如开 sanitizer） |

## 8. 你接下来会遇到的（预告，不急着全懂）

- **多个文件**：`add_executable(app main.cpp util.cpp)`，把多个 `.cpp` 一起列进去；
- **库**：`add_library(mylib STATIC mylib.cpp)` 生成一个库，再 `target_link_libraries(app mylib)` 让程序链接它（U0 主线项目就是搭这个 `lib + app` 结构）；
- **外部库**：`find_package(...)` 找系统里的第三方库。

这些到了 U0 项目里我会带你一步步搭。

## 9. 常见报错速查

| 报错 | 含义 | 解决 |
|---|---|---|
| `cmake: command not found` | 没装 | `brew install cmake` |
| `CMake Error: The source directory ... does not exist` | 目录不对 | `cd` 到有 `CMakeLists.txt` 的目录 |
| `undefined reference to ...` | 链接错误 | 忘了把源文件/库加进 target |
| `No CMAKE_CXX_COMPILER could be found` | 没装 C++ 编译器 | 装 Xcode CLT：`xcode-select --install` |

## 自测

1. CMake 自己会编译代码吗？它生成的是什么？
2. `cmake -B build` 和 `cmake --build build` 分别做什么？
3. `add_executable(hello main.cpp)` 里的 `hello` 是什么？
4. 为什么要把构建产物都放进 `build/`，而不是源码目录里？
