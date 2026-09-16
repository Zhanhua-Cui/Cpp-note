// 03_smart_ptr.cpp —— 智能指针
// 编译：g++ -std=c++17 -Wall -Wextra 03_smart_ptr.cpp -o smartptr && ./smartptr

#include <iostream>
#include <memory>    // unique_ptr, shared_ptr, weak_ptr, make_unique, make_shared
#include <string>

// ============================================================
// 1. unique_ptr：独占所有权
// ============================================================
void demo_unique_ptr() {
    std::cout << "\n===== 1. unique_ptr（独占所有权）=====\n";

    auto p = std::make_unique<int>(42);
    std::cout << "  *p = " << *p << "\n";

    // auto q = p;              // ❌ 编译错误：unique_ptr 不可拷贝
    auto q = std::move(p);      // ✅ 移动：所有权转移给 q
    std::cout << "  移动后: p 是" << (p ? "非空" : "空(nullptr)")
              << ", q 是" << (q ? "非空" : "空") << "，*q = " << *q << "\n";

    // unique_ptr 管理数组（但通常更推荐 std::vector）
    auto arr = std::make_unique<int[]>(5);
    arr[0] = 100;
    std::cout << "  arr[0] = " << arr[0] << "\n";
}

// ============================================================
// 2. shared_ptr：共享所有权 + 引用计数
// ============================================================
struct Dog {
    std::string name;
    explicit Dog(std::string n) : name(std::move(n)) {}
    ~Dog() { std::cout << "  [Dog " << name << " 析构]\n"; }
};

void demo_shared_ptr() {
    std::cout << "\n===== 2. shared_ptr（共享所有权 + 引用计数）=====\n";

    auto s1 = std::make_shared<Dog>("旺财");
    std::cout << "  创建 s1，use_count = " << s1.use_count() << "\n";
    {
        auto s2 = s1;    // 拷贝：计数 +1
        auto s3 = s1;    // 计数 +1
        std::cout << "  块内又复制了 s2、s3，use_count = " << s1.use_count() << "\n";
    }
    std::cout << "  离开块，s2、s3 析构，use_count = " << s1.use_count() << "\n";
    std::cout << "  等 s1 也析构（函数结束）时，use_count 归 0，Dog 才被释放\n";
}

// ============================================================
// 3. 循环引用问题：两个 shared_ptr 互相引用 → 泄漏
// ============================================================
struct Node {
    std::shared_ptr<Node> next;   // 强引用（占计数）
    int id;
    explicit Node(int i) : id(i) {}
    ~Node() { std::cout << "  [Node " << id << " 析构]\n"; }
};

void demo_cycle_bad() {
    std::cout << "\n===== 3. 循环引用（❌ 内存泄漏）=====\n";
    {
        auto a = std::make_shared<Node>(1);
        auto b = std::make_shared<Node>(2);
        a->next = b;   // b 计数 = 2
        b->next = a;   // a 计数 = 2
        std::cout << "  a.use_count=" << a.use_count()
                  << ", b.use_count=" << b.use_count() << "\n";
        std::cout << "  即将离开作用域……\n";
    }
    std::cout << "  ❌ 离开作用域后没有任何 Node 析构：a、b 计数只从 2 降到 1，永不为 0 → 泄漏！\n";
}

// ============================================================
// 4. weak_ptr：打破循环引用
// ============================================================
struct SafeNode {
    std::shared_ptr<SafeNode> next;   // 强引用
    std::weak_ptr<SafeNode>   prev;   // 弱引用：不占计数
    int id;
    explicit SafeNode(int i) : id(i) {}
    ~SafeNode() { std::cout << "  [SafeNode " << id << " 析构]\n"; }
};

void demo_weak_ptr() {
    std::cout << "\n===== 4. weak_ptr 打破循环引用（✅ 正确释放）=====\n";
    {
        auto a = std::make_shared<SafeNode>(1);
        auto b = std::make_shared<SafeNode>(2);
        a->next = b;    // b 计数 = 2
        b->prev = a;    // 弱引用，a 计数不变（还是 1）
        std::cout << "  a.use_count=" << a.use_count()
                  << ", b.use_count=" << b.use_count() << "\n";
        std::cout << "  即将离开作用域……\n";
    }
    std::cout << "  ✅ 两个 SafeNode 都正常析构了：weak_ptr 不占引用计数，环被打破\n";
}

// ============================================================
// 5. weak_ptr 的使用：lock() 升级
// ============================================================
void demo_weak_lock() {
    std::cout << "\n===== 5. weak_ptr 要先 lock() 才能用 =====\n";
    std::weak_ptr<Dog> w;
    {
        auto s = std::make_shared<Dog>("临时狗");
        w = s;                                  // weak_ptr 观察 s，但不占计数
        if (auto sp = w.lock()) {               // 升级成 shared_ptr
            std::cout << "  对象还活着，name = " << sp->name << "\n";
        }
        std::cout << "  expired() = " << (w.expired() ? "true" : "false") << "\n";
    }   // s 析构，Dog 被释放
    std::cout << "  离开块后 expired() = " << (w.expired() ? "true（对象已死）" : "false") << "\n";
    if (auto sp = w.lock()) {
        std::cout << "  不该走到这里\n";
    } else {
        std::cout << "  lock() 返回空：对象已释放，无法升级\n";
    }
}

int main() {
    demo_unique_ptr();
    demo_shared_ptr();
    demo_cycle_bad();
    demo_weak_ptr();
    demo_weak_lock();

    std::cout << "\n===== 总结 =====\n";
    std::cout << "1. 默认用 unique_ptr，需要共享才用 shared_ptr\n";
    std::cout << "2. 创建用 make_unique / make_shared，别手动 new\n";
    std::cout << "3. 出现互相引用时，用 weak_ptr 打破循环\n";
    std::cout << "4. weak_ptr 要先 lock() 再用\n";
    return 0;
}
