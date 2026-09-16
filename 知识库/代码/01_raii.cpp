// 01_raii.cpp —— RAII 与拷贝控制
// 编译：g++ -std=c++17 -Wall -Wextra 01_raii.cpp -o raii && ./raii

#include <iostream>
#include <cstring>   // strlen, strcpy
#include <fstream>   // 文件流（RAII 的现成例子）

// ============================================================
// 1. RAII 演示：一个作用域守卫（离开作用域自动做事）
// ============================================================
class ScopeGuard {
    const char* name_;
public:
    explicit ScopeGuard(const char* name) : name_(name) {
        std::cout << "[进入作用域] " << name_ << "\n";
    }
    ~ScopeGuard() {
        std::cout << "[离开作用域] " << name_ << " —— 析构自动触发\n";
    }
};

// ============================================================
// 2. 一个手动管理内存的类，演示「三法则」的必要性
// ============================================================
class String {
    char* data_;
public:
    // 普通构造：在堆上分配内存
    explicit String(const char* s = "") {
        data_ = new char[strlen(s) + 1];
        strcpy(data_, s);
        std::cout << "  构造: \"" << data_ << "\"\n";
    }

    // (1) 析构：释放堆内存 —— 没有它就会内存泄漏
    ~String() {
        std::cout << "  析构: \"" << (data_ ? data_ : "(null)") << "\"\n";
        delete[] data_;
    }

    // (2) 拷贝构造：深拷贝 —— 没有它就会「浅拷贝」，两个对象共享同一块内存
    String(const String& other) {
        data_ = new char[strlen(other.data_) + 1];
        strcpy(data_, other.data_);
        std::cout << "  拷贝构造(深拷贝): \"" << data_ << "\"\n";
    }

    // (3) 拷贝赋值：先释放旧资源，再深拷贝，还要防自赋值
    String& operator=(const String& other) {
        std::cout << "  拷贝赋值: \"" << other.data_ << "\"\n";
        if (this == &other) return *this;       // 自赋值保护
        delete[] data_;                          // 释放旧内存
        data_ = new char[strlen(other.data_) + 1];
        strcpy(data_, other.data_);
        return *this;
    }

    const char* c_str() const { return data_; }
};

// ============================================================
// 3. 浅拷贝的灾难（演示用，运行这段会导致 double free）
//    想看效果就取消注释 main 里对应的调用
// ============================================================
class ShallowBad {
    char* data_;
public:
    explicit ShallowBad(const char* s) {
        data_ = new char[strlen(s) + 1];
        strcpy(data_, s);
    }
    ~ShallowBad() { delete[] data_; }   // 有析构，但没写拷贝构造/拷贝赋值！
    // 编译器会隐式生成「浅拷贝」版本
};

// ============================================================
// 4. 零法则：自己不管理裸资源，就什么都不用写
// ============================================================
struct Person {
    std::string name;          // string 自己管理内存
    // 没有指针、没有 new/delete，所以拷贝/移动/析构全部默认正确
};

void demo_raii() {
    std::cout << "\n===== 1. RAII：对象析构自动释放资源 =====\n";
    {
        ScopeGuard g("块1");
        // 这里无论怎么离开（正常走完/提前 return/抛异常），g 都会析构
    }
    std::cout << "块1 结束，上面那行「离开作用域」是析构函数自动打印的\n";
}

void demo_file_raii() {
    std::cout << "\n===== 2. 文件流就是 RAII =====\n";
    std::ofstream out("/tmp/cpp_raii_demo.txt");
    if (out) {
        out << "这段文字由 ofstream 写入\n";
        std::cout << "  已写入 /tmp/cpp_raii_demo.txt\n";
    }
    // out 离开作用域时自动关闭文件，无需手动 close
}

void demo_rule_of_three() {
    std::cout << "\n===== 3. 三法则：自定义了拷贝构造/赋值，深拷贝安全 =====\n";
    {
        String a("hello");
        String b = a;        // 拷贝构造
        String c("world");
        c = a;               // 拷贝赋值
        String d(a);         // 也是拷贝构造
        (void)b; (void)c; (void)d;
        std::cout << "  a = " << a.c_str() << ", b = " << b.c_str()
                  << ", c = " << c.c_str() << "\n";
    }
    std::cout << "  离开作用域：4 个对象各自析构，内存各自释放，无泄漏\n";
}

void demo_zero_rule() {
    std::cout << "\n===== 4. 零法则：用值类型成员，什么都不用写 =====\n";
    Person p1{"张三"};
    Person p2 = p1;          // 自动正确拷贝（string 深拷贝）
    std::cout << "  p1=" << p1.name << ", p2=" << p2.name << "\n";
}

int main() {
    demo_raii();
    demo_file_raii();
    demo_rule_of_three();
    demo_zero_rule();

    std::cout << "\n===== 想体验「浅拷贝灾难」？ =====\n";
    std::cout << "取消下面这行注释再编译运行，会看到 double free 崩溃：\n";
    std::cout << "// { ShallowBad a(\"x\"); ShallowBad b = a; }  // 两个对象指向同一块内存，析构两次\n";

    // 下面这行故意注释掉，避免直接崩溃。想实验就把注释打开。
    // { ShallowBad a("boom"); ShallowBad b = a; }   // ⚠️ double free

    return 0;
}
