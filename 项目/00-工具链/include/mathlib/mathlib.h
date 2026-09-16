#pragma once
// mathlib —— 一个最简单的示例库，用来演示「多文件 + 库」的 CMake 结构。
//
// 注意头文件路径：它放在 include/mathlib/ 下，
// 所以引用时要写 #include <mathlib/mathlib.h>（带一层目录）。
// 好处：避免和其它库的同名头文件撞车，这是 C++ 项目的通用约定。

namespace mathlib {

// 两个整数相加
int add(int a, int b);

// 阶乘 n!（要求 n >= 0）
long long factorial(int n);

// 判断 n 是否为质数
bool is_prime(int n);

} // namespace mathlib
