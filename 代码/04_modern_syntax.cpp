// 04_modern_syntax.cpp —— 现代语法糖（C++11/14/17）
// 编译：g++ -std=c++17 -Wall -Wextra 04_modern_syntax.cpp -o modern && ./modern

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

// ============================================================
// 1. auto：类型推导
// ============================================================
void demo_auto() {
    std::cout << "\n===== 1. auto =====\n";
    auto i = 42;               // int
    auto d = 3.14;             // double
    auto s = std::string("hi");// std::string
    std::cout << "  i=" << i << ", d=" << d << ", s=" << s << "\n";

    // auto 会丢弃引用和顶层 const
    const int c = 10;
    auto a = c;          // int（const 丢了）
    a = 99;              // 可以改
    const auto& b = c;   // 想保留 const 引用要显式写
    std::cout << "  a=" << a << ", b=" << b << "（b 是 const int&，不能改）\n";
}

// ============================================================
// 2. nullptr + 3. 范围 for
// ============================================================
void demo_nullptr_and_range_for() {
    std::cout << "\n===== 2. nullptr & 3. 范围 for =====\n";
    int* p = nullptr;   // 而不是 NULL 或 0
    std::cout << "  p 是" << (p == nullptr ? "空指针" : "非空") << "\n";

    std::vector<int> v{1, 2, 3, 4};

    std::cout << "  拷贝遍历: ";
    for (int x : v) std::cout << x << ' ';

    for (int& x : v) x *= 2;   // 引用遍历：修改元素

    std::cout << "\n  乘2后(只读遍历): ";
    for (const auto& x : v) std::cout << x << ' ';
    std::cout << "\n";
}

// ============================================================
// 4. lambda 表达式
// ============================================================
void demo_lambda() {
    std::cout << "\n===== 4. lambda =====\n";

    auto add = [](int a, int b) { return a + b; };
    std::cout << "  add(2,3) = " << add(2, 3) << "\n";

    // 捕获外部变量
    int base = 10;
    auto plus_base = [base](int x) { return x + base; };   // 按值捕获
    std::cout << "  plus_base(5) = " << plus_base(5) << "\n";

    int counter = 0;
    auto inc = [&counter]() { ++counter; };                // 按引用捕获
    inc(); inc(); inc();
    std::cout << "  counter = " << counter << "\n";

    // 和算法配合：降序排序
    std::vector<int> v{5, 1, 4, 2, 3};
    std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });
    std::cout << "  降序: ";
    for (int x : v) std::cout << x << ' ';
    std::cout << "\n";
}

// ============================================================
// 5. enum class + 6. override/final
// ============================================================
enum class Color { red, green, blue };
enum class Traffic { red, yellow, green };   // 两个 red 不冲突

struct Shape {
    virtual void draw() const { std::cout << "  Shape::draw\n"; }
    virtual void area()  { std::cout << "  Shape::area\n"; }
};

struct Circle : Shape {
    void draw() const override { std::cout << "  Circle::draw (override)\n"; }
    void area()  final      { std::cout << "  Circle::area (final，子类不能再重写)\n"; }
    // void drw() const override {}  // ❌ 拼错名字，加了 override 编译器会报错
};

void demo_enum_override() {
    std::cout << "\n===== 5. enum class & 6. override/final =====\n";
    Color c = Color::red;
    Traffic t = Traffic::red;      // 不冲突，都要带作用域
    (void)c; (void)t;

    Circle circle;
    circle.draw();
    circle.area();
}

// ============================================================
// 7. constexpr + 8. 结构化绑定
// ============================================================
constexpr int square(int x) { return x * x; }

struct Point { int x, y; };

void demo_constexpr_binding() {
    std::cout << "\n===== 7. constexpr & 8. 结构化绑定 =====\n";

    constexpr int N = square(10);    // 编译期就算出 100
    int arr[N];                      // 可以当数组大小
    arr[0] = N;
    std::cout << "  N = square(10) = " << N << "（编译期常量），arr[0]=" << arr[0] << "\n";

    // 结构化绑定：解包 struct
    Point p{3, 4};
    auto [px, py] = p;
    std::cout << "  点坐标: (" << px << ", " << py << ")\n";

    // 解包 map（迭代时最常用）
    std::map<std::string, int> m{{"a", 1}, {"b", 2}};
    for (auto& [key, value] : m) {
        std::cout << "  " << key << " -> " << value << "\n";
    }
}

int main() {
    demo_auto();
    demo_nullptr_and_range_for();
    demo_lambda();
    demo_enum_override();
    demo_constexpr_binding();
    return 0;
}
