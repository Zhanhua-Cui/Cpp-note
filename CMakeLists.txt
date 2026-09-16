# 顶层 CMakeLists.txt —— 配合 指南/CMake入门.md 阅读
# 用法：
#   cmake -B build          # 第1步：配置
#   cmake --build build     # 第2步：编译
#   ./build/01_raii         # 第3步：运行

# 1) 最低 CMake 版本 + 项目名（CXX = C++ 项目）
cmake_minimum_required(VERSION 3.16)
project(cpp_note CXX)

# 2) 统一使用 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 3) 默认 Debug 模式（方便调试，后续配合 sanitizer）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 4) 开启常见警告，帮你早发现 bug
add_compile_options(-Wall -Wextra)

# 5) 把「知识库/代码」下的 5 个示例各编译成一个可执行文件
add_executable(01_raii          知识库/代码/01_raii.cpp)
add_executable(02_copy_move     知识库/代码/02_copy_move.cpp)
add_executable(03_smart_ptr     知识库/代码/03_smart_ptr.cpp)
add_executable(04_modern_syntax 知识库/代码/04_modern_syntax.cpp)
add_executable(05_stl           知识库/代码/05_stl.cpp)
