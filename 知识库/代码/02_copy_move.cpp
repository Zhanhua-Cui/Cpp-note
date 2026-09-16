// 02_copy_move.cpp —— 移动语义与右值引用
// 编译：g++ -std=c++17 -Wall -Wextra 02_copy_move.cpp -o copymove && ./copymove

#include <iostream>
#include <cstring>
#include <utility>   // std::move, std::forward
#include <vector>

// ============================================================
// 一个管理堆内存的 Buffer：同时实现拷贝 + 移动
// ============================================================
class Buffer {
    char* data_;
    size_t size_;
public:
    explicit Buffer(size_t n) : size_(n) {
        data_ = new char[n];
        std::cout << "  构造 " << n << " 字节\n";
    }

    // 拷贝构造：深拷贝，O(n)
    Buffer(const Buffer& other) : size_(other.size_) {
        data_ = new char[size_];
        std::copy(other.data_, other.data_ + size_, data_);
        std::cout << "  拷贝构造（深拷贝，复制 " << size_ << " 字节）\n";
    }

    // 移动构造：窃取资源，O(1)
    Buffer(Buffer&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;   // 关键！掏空对方，防止 double delete
        other.size_ = 0;
        std::cout << "  移动构造（窃取资源，O(1)）\n";
    }

    // 拷贝赋值
    Buffer& operator=(const Buffer& other) {
        std::cout << "  拷贝赋值\n";
        if (this == &other) return *this;
        delete[] data_;
        size_ = other.size_;
        data_ = new char[size_];
        std::copy(other.data_, other.data_ + size_, data_);
        return *this;
    }

    // 移动赋值
    Buffer& operator=(Buffer&& other) noexcept {
        std::cout << "  移动赋值\n";
        if (this == &other) return *this;
        delete[] data_;            // 先释放自己的旧资源
        data_ = other.data_;       // 接管对方资源
        size_ = other.size_;
        other.data_ = nullptr;     // 掏空对方
        other.size_ = 0;
        return *this;
    }

    ~Buffer() {
        std::cout << "  析构 " << size_ << " 字节\n";
        delete[] data_;
    }

    size_t size() const { return size_; }
};

// ============================================================
// 返回一个 Buffer：观察移动何时发生
// ============================================================
Buffer make_buffer() {
    Buffer b(64);
    return b;   // 返回值优化(RVO)可能连移动都省掉；看编译器输出
}

// ============================================================
// 1. 左值 vs 右值
// ============================================================
void demo_value_category() {
    std::cout << "\n===== 1. 左值 vs 右值 =====\n";
    int a = 10;
    int& lref = a;         // 左值引用绑左值
    // int&& bad = a;      // ❌ 右值引用不能绑左值
    int&& rref = 10;       // ✅ 右值引用绑右值（字面量）
    int&& r2 = a + 1;      // ✅ a+1 是临时值（右值）
    std::cout << "  a=" << a << ", lref=" << lref
              << ", rref=" << rref << ", r2=" << r2 << "\n";
    std::cout << "  口诀：能 &变量 取地址的是左值，否则是右值\n";
}

// ============================================================
// 2. 拷贝 vs 移动
// ============================================================
void demo_copy_vs_move() {
    std::cout << "\n===== 2. 拷贝 vs 移动 =====\n";
    std::cout << "--- 拷贝（a 之后还要用，必须复制一份）---\n";
    Buffer a(32);
    Buffer b = a;              // 拷贝构造：a 仍然有效
    std::cout << "  a.size=" << a.size() << ", b.size=" << b.size() << "\n";

    std::cout << "--- 移动（承诺 a 不再用，资源被搬空）---\n";
    Buffer c = std::move(a);   // 移动构造：a 的资源被偷走
    std::cout << "  c.size=" << c.size()
              << ", a.size=" << a.size() << "（a 已被掏空）\n";
}

// ============================================================
// 3. std::move 只是类型转换，不移动任何东西
// ============================================================
void demo_move_is_cast() {
    std::cout << "\n===== 3. std::move 只是把左值转成右值引用 =====\n";
    int x = 42;
    int&& r = std::move(x);   // 只是转换，x 的值完全没变
    std::cout << "  x=" << x << "，r=" << r
              << "（std::move 后 x 的值还在，它没动任何东西）\n";
    std::cout << "  它只是让编译器「按右值处理 x」，从而触发移动语义\n";
}

// ============================================================
// 4. vector 搬移元素：为什么移动构造要 noexcept
// ============================================================
void demo_vector_move() {
    std::cout << "\n===== 4. vector 扩容时搬移元素 =====\n";
    std::vector<Buffer> v;
    v.reserve(4);              // 预留容量，避免频繁扩容干扰观察
    for (int i = 0; i < 3; ++i) {
        std::cout << "push_back #" << i << ":\n";
        v.push_back(Buffer(16));   // 临时对象是右值 → 触发移动
    }
    std::cout << "（如果移动构造没加 noexcept，vector 会退化成拷贝）\n";
}

// ============================================================
// 5. 完美转发（了解）：std::forward 保留左右值
// ============================================================
void target(int&)  { std::cout << "  转发给了「左值引用」版本\n"; }
void target(int&&) { std::cout << "  转发给了「右值引用」版本\n"; }

template <typename T>
void relay(T&& arg) {
    target(std::forward<T>(arg));   // arg 是左值就按左值转发，右值就按右值转发
}

void demo_forward() {
    std::cout << "\n===== 5. std::forward 完美转发 =====\n";
    int n = 5;
    relay(n);             // 左值 → 左值引用版本
    relay(10);            // 右值 → 右值引用版本
    relay(std::move(n));  // 转成右值 → 右值引用版本
}

int main() {
    demo_value_category();
    demo_copy_vs_move();
    demo_move_is_cast();
    demo_vector_move();
    demo_forward();

    std::cout << "\n===== 总结 =====\n";
    std::cout << "1. 左值能取地址，右值是临时的、即将销毁的\n";
    std::cout << "2. 移动 = 把即将销毁对象的资源「偷」过来，O(1)\n";
    std::cout << "3. std::move 只是类型转换，真正的移动发生在移动构造/移动赋值里\n";
    std::cout << "4. 移动构造必须把对方掏空 + 加 noexcept\n";
    return 0;
}
