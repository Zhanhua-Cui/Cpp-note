// main.cpp —— 可执行程序，调用 mathlib 库里的函数。
// 关键：它只需要 include 头文件，链接工作由 CMake 的 target_link_libraries 完成。

#include <mathlib/mathlib.h>
#include <iostream>

int main() {
    std::cout << "add(3, 4)      = " << mathlib::add(3, 4) << "\n";
    std::cout << "factorial(10)  = " << mathlib::factorial(10) << "\n";
    std::cout << "is_prime(97)   = " << (mathlib::is_prime(97) ? "true" : "false") << "\n";
    std::cout << "is_prime(100)  = " << (mathlib::is_prime(100) ? "true" : "false") << "\n";
    return 0;
}
