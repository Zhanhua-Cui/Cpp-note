// mathlib.cpp —— mathlib 库的实现。
// 注意：#include <mathlib/mathlib.h> 用的是尖括号 + 目录前缀，
// 这需要 CMake 里 target_include_directories 把 include/ 目录告诉编译器（见 CMakeLists.txt）。

#include <mathlib/mathlib.h>

namespace mathlib {

int add(int a, int b) {
    return a + b;
}

long long factorial(int n) {
    long long result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

bool is_prime(int n) {
    if (n < 2) return false;
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

} // namespace mathlib
