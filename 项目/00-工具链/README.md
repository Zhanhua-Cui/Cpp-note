# U0 实战：第一个多文件 CMake 工程

> 目标：看懂「库（lib）+ 程序（app）」的目录结构和构建方式。这是后面所有单元的统一模板。

## 目录结构

```
00-工具链/
├── CMakeLists.txt        ← 构建脚本（重点！逐行看它的注释）
├── README.md             ← 你在这里
├── include/              ← 库的「公共头文件」（给别人 #include 用）
│   └── mathlib/
│       └── mathlib.h
├── src/                  ← 库的「实现」（.cpp）
│   └── mathlib.cpp
└── app/                  ← 可执行程序
    └── main.cpp
```

**为什么要这样分？**

- `include/` 放头文件、`src/` 放实现：把「接口」和「实现」分开，是 C++ 项目的标准做法；
- `include/mathlib/mathlib.h` 带一层目录：引用时写 `#include <mathlib/mathlib.h>`，避免和别的库同名头文件撞车；
- `app/` 单独放程序入口：一个库可以被多个程序复用。

## 怎么构建

```bash
cd 项目/00-工具链
cmake -B build          # 配置（生成构建文件）
cmake --build build     # 编译（先编出 libmathlib.a，再链接出 demo）
./build/demo            # 运行
```

预期输出：

```
add(3, 4)      = 7
factorial(10)  = 3628800
is_prime(97)   = true
is_prime(100)  = false
```

## 三个关键 CMake 命令（务必搞懂）

| 命令 | 作用 | 类比 |
|---|---|---|
| `add_library(mathlib STATIC src/mathlib.cpp)` | 把 `mathlib.cpp` 编译成一个**库** | 做零件 |
| `add_executable(demo app/main.cpp)` | 把 `main.cpp` 编译成**可执行程序** | 做成品 |
| `target_link_libraries(demo PRIVATE mathlib)` | 让 demo **链接** mathlib | 把零件装进成品 |

再配合 `target_include_directories(mathlib PUBLIC include)`——告诉编译器「头文件在 `include/` 目录」，`main.cpp` 才能 `#include <mathlib/mathlib.h>`。

## 试试亲手改（照做一遍才算学会）

1. **加一个函数**：在 `mathlib.h` 声明 `int multiply(int, int)`，在 `mathlib.cpp` 实现，在 `main.cpp` 调用。重新 `cmake --build build` 看效果。
2. **加第二个程序**：新建 `app/calc.cpp`，再写一行 `add_executable(calc app/calc.cpp)` + `target_link_libraries(calc PRIVATE mathlib)`。重新配置（`cmake -B build`）再编译，看会不会多出一个 `build/calc`。
3. **故意制造链接错误**：把 `target_link_libraries(demo PRIVATE mathlib)` 注释掉再编译，观察 `undefined reference` 报错长什么样——理解「链接」到底在干嘛。
4. **开 sanitizer 编译**（进阶）：
   ```bash
   cmake -B build -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined"
   cmake --build build && ./build/demo
   ```

## 检查自己是否真懂

1. `add_library` 和 `add_executable` 分别产出什么文件？
2. `main.cpp` 能 `#include <mathlib/mathlib.h>`，靠的是 CMake 里哪一行？
3. 如果把 `target_link_libraries` 删掉，会发生什么？为什么？

> 全部答上，U0 的 CMake 部分就过关了，可以进入 U1（`MyString` 项目）。
> 答不上就回 [`../../指南/CMake入门.md`](../../指南/CMake入门.md) 看对应小节。
