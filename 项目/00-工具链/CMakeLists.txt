# 00-工具链 —— 第一个多文件 CMake 工程（lib + app）
# 这是 U0 的实战样板，也是后面每个单元的统一模板。核心不是代码，是看懂构建结构。
# 对照 ../../指南/CMake入门.md 第 8 节一起看。

cmake_minimum_required(VERSION 3.16)
project(toolchain_demo CXX)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============ 1) 定义一个「库」target ============
# STATIC = 静态库（编译成 .a 文件，链接时被拷进可执行文件）
add_library(mathlib STATIC
    src/mathlib.cpp
)

# 把 include/ 目录暴露给「引用 mathlib 的人」。
# PUBLIC 的意思是：谁链接了 mathlib，谁就自动获得这个 include 目录，
# 于是 app 里就能写 #include <mathlib/mathlib.h>。
target_include_directories(mathlib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ============ 2) 定义一个「可执行程序」target ============
add_executable(demo
    app/main.cpp
)

# 把 demo 链接到 mathlib：main.cpp 里调用的函数在库里，不链接就报 undefined reference。
# PRIVATE 的意思是：这个链接关系是 demo 自己用的，不外传。
target_link_libraries(demo PRIVATE mathlib)
