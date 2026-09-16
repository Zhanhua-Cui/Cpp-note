# 01 · RAII 与拷贝控制

> 这是整个现代 C++ 的「根」。移动语义、智能指针、文件流、锁管理，全都是它的延伸。

## 1. 什么是 RAII

**RAII = Resource Acquisition Is Initialization（资源获取即初始化）。**

核心思想一句话：**把资源的生命周期绑定到一个对象的生命周期上。**

- 对象**构造**时，获取资源（打开文件、分配内存、加锁）；
- 对象**析构**时，自动释放资源。

好处：无论函数是正常返回、提前 return、还是抛异常，只要对象离开作用域，析构函数一定被调用，资源一定被释放。**不会泄漏，也不用手动配对释放。**

### 对比：C 风格 vs RAII

```cpp
// ❌ C 风格：手动管理，任何提前 return/异常都会泄漏
void bad() {
    FILE* f = fopen("a.txt", "r");
    if (f == nullptr) return;
    // ... 中途 return 或抛异常，fclose 就永远执行不到
    fclose(f);
}

// ✅ RAII：fstream 的析构会自动关闭文件
void good() {
    std::ifstream f("a.txt");
    // ... 无论怎么离开这个函数，f 析构时文件都会被关闭
}
```

## 2. 为什么拷贝控制这么重要

C++ 会**隐式**为你生成 6 个特殊成员函数。当你用 `new` 管理了堆内存，隐式版本几乎一定是错的：

| 特殊成员 | 默认行为 | 问题 |
|---|---|---|
| 默认构造 | 什么都不做 | 指针成员未初始化 → 野指针 |
| 析构 | 什么都不做 | 堆内存泄漏 |
| 拷贝构造 | 逐成员拷贝 | **浅拷贝**：两个对象指向同一块内存 |
| 拷贝赋值 | 逐成员赋值 | 同上 + 覆盖时泄漏旧内存 |
| 移动构造 | 逐成员移动 | 见第 2 篇 |
| 移动赋值 | 逐成员移动 | 见第 2 篇 |

### 浅拷贝的灾难

```cpp
class String {
    char* data_;        // 指向堆内存
public:
    explicit String(const char* s) {
        data_ = new char[strlen(s) + 1];
        strcpy(data_, s);
    }
    // 没有自定义析构/拷贝 → 编译器给隐式版本
};

int main() {
    String a("hello");
    String b = a;       // 隐式拷贝：b.data_ 和 a.data_ 指向同一块内存
    // 作用域结束：a、b 各析构一次 → 同一块内存被 delete 两次 → 未定义行为
}
```

**结论：只要你的类手动管理了资源（new、文件句柄、锁），就必须显式处理这 6 个函数。**

## 3. 三/五法则

### 三法则（C++98 时代）

一个类如果需要**自定义析构、拷贝构造、拷贝赋值**三者之一，通常三者都需要。

```cpp
class String {
    char* data_;
public:
    explicit String(const char* s = "") {
        data_ = new char[strlen(s) + 1];
        strcpy(data_, s);
    }
    // 1) 析构：释放资源
    ~String() { delete[] data_; }

    // 2) 拷贝构造：深拷贝
    String(const String& other) {
        data_ = new char[strlen(other.data_) + 1];
        strcpy(data_, other.data_);
    }

    // 3) 拷贝赋值：先释放旧资源，再深拷贝
    String& operator=(const String& other) {
        if (this == &other) return *this;   // 自赋值保护！
        delete[] data_;                      // 释放旧内存
        data_ = new char[strlen(other.data_) + 1];
        strcpy(data_, other.data_);
        return *this;
    }
};
```

### 五法则（C++11 加入移动）

有了移动语义后，还要补上移动构造和移动赋值 → 五法则。详见第 2 篇。

### 零法则（现代 C++ 的追求）

**如果你的类自己不管理原始资源**（用 `std::string`、`std::vector`、智能指针等成员），就**什么都不要写**，全部交给编译器默认生成。

```cpp
// ✅ 零法则：没有裸指针，没有 new/delete，什么都不用写
struct Person {
    std::string name;          // string 自己管理内存
    std::vector<int> scores;   // vector 自己管理内存
    // 拷贝、移动、析构全部默认正确
};
```

**记住这句话：能用值类型（string/vector/智能指针）解决的问题，就不要自己 new/delete。** 这就是从 C++98 到现代 C++ 的思维转变。

## 4. 拷贝构造 vs 拷贝赋值

很多人的盲点：两者**触发时机不同**。

```cpp
String a("x");
String b(a);      // ① 拷贝构造：对象「诞生」时
String c = a;     // ② 拷贝构造（不是赋值！对象还在诞生）
c = b;            // ③ 拷贝赋值：对象「已经存在」时
```

- 拷贝构造：对象尚不存在，无需清理旧资源，直接构造。
- 拷贝赋值：对象已存在，必须**先释放旧资源**，再拷贝，还要处理**自赋值**。

## 5. 虚析构

> 当类会被继承、且可能通过基类指针删除派生类对象时，基类析构函数必须是 `virtual`。

```cpp
class Base { public: ~Base() {} };       // ❌ 非虚析构
class Derived : public Base { public: ~Derived() {} };

Base* p = new Derived();
delete p;      // ❌ 只调用 Base 析构，Derived 部分泄漏！

class Base2 { public: virtual ~Base2() {} };  // ✅ 虚析构
// 现在 delete 基类指针会正确先调 Derived 析构，再调 Base 析构
```

## 自测

1. RAII 的「R」指什么？它解决了什么问题？
2. 浅拷贝和深拷贝的区别？为什么管理堆内存的类默认拷贝是危险的？
3. 拷贝构造和拷贝赋值在「触发时机」和「要做的事」上有什么区别？
4. 什么情况下析构函数必须是 `virtual`？
5. 「零法则」是什么意思？为什么说它是现代 C++ 追求的目标？

> 答完再进下一篇。答案都能在第 1 篇正文和第 `知识库/代码/01_raii.cpp` 的注释里找到。
