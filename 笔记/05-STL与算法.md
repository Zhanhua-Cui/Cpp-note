# 05 · STL 与算法

> 收口篇：把前面的 RAII、移动语义、lambda 组合起来，用「容器 + 算法」清晰表达意图。

## 1. 心法：容器管内存，算法管逻辑

现代 C++ 的推荐写法是**尽量用算法表达意图，而不是手写循环**：

```cpp
std::vector<int> v{1, 2, 3, 4, 5};

// ❌ 手写循环：又要考虑边界、又要维护下标
int sum = 0;
for (size_t i = 0; i < v.size(); ++i) sum += v[i];

// ✅ 算法：意图一目了然
int sum2 = std::accumulate(v.begin(), v.end(), 0);
```

## 2. 容器怎么选（速查表）

| 容器 | 底层 | 特点 | 典型场景 |
|---|---|---|---|
| `vector` | 动态数组 | 随机访问 O(1)，尾部插入 O(1)，中间插入/删除 O(n) | **默认首选** |
| `deque` | 双端队列 | 头尾插入删除 O(1) | 需要头尾都操作 |
| `list` | 双向链表 | 任意位置插入删除 O(1)，但不支持随机访问 | 频繁中间插入删除 |
| `map` | 红黑树 | 有序，查找/插入 O(log n)，按 key 有序 | 需要有序遍历 |
| `unordered_map` | 哈希表 | 平均 O(1) 查找，无序 | **按 key 查找首选** |
| `set` / `unordered_set` | 同上 | 只存 key 不存 value | 去重、集合运算 |

**经验法则**：

1. 想不起来用啥 → 用 `vector`；
2. 按 key 快速查找 → `unordered_map`；
3. 需要有序遍历 → `map`。

## 3. 迭代器：容器和算法的桥梁

```cpp
std::vector<int> v{1, 2, 3, 4, 5};
auto begin = v.begin();   // 指向第一个元素
auto end   = v.end();     // 指向「最后一个元素的后一位」（哨兵）

// 左闭右开区间 [begin, end)
for (auto it = v.begin(); it != v.end(); ++it)
    std::cout << *it << ' ';
```

迭代器的类型：`begin`/`end`（可读写）、`cbegin`/`cend`（只读）、`rbegin`/`rend`（反向）。

## 4. 常用算法（`<algorithm>` 和 `<numeric>`）

| 算法 | 作用 |
|---|---|
| `std::sort` | 排序（可传比较器/lambda） |
| `std::find` / `find_if` | 查找元素/按条件查找 |
| `std::count` / `count_if` | 计数 |
| `std::accumulate` | 求和/累积（`<numeric>`） |
| `std::transform` | 逐元素变换到另一个容器 |
| `std::copy` | 拷贝区间 |
| `std::for_each` | 对每个元素执行操作 |
| `std::remove_if` + `erase` | 按条件删除（erase-remove 惯用法） |
| `std::min_element` / `max_element` | 找最小/最大元素 |
| `std::binary_search` / `lower_bound` | 有序区间二分查找 |

### 几个高频例子

```cpp
std::vector<int> v{5, 2, 8, 1, 9, 3, 7, 4, 6};

// 1. 排序
std::sort(v.begin(), v.end());

// 2. 查找第一个 > 5 的元素
auto it = std::find_if(v.begin(), v.end(), [](int x){ return x > 5; });

// 3. 统计偶数个数
int even = std::count_if(v.begin(), v.end(), [](int x){ return x % 2 == 0; });

// 4. 每个元素平方，存到新容器
std::vector<int> sq(v.size());
std::transform(v.begin(), v.end(), sq.begin(), [](int x){ return x * x; });

// 5. 求和
int total = std::accumulate(v.begin(), v.end(), 0);
```

### erase-remove 惯用法（按条件删除元素）

`remove_if` 本身**不真正删除**（它只把要保留的挪到前面），必须配合 `erase`：

```cpp
// 删除所有偶数
v.erase(std::remove_if(v.begin(), v.end(),
                       [](int x){ return x % 2 == 0; }),
        v.end());
```

## 5. 复杂度意识（面试常考）

| 操作 | vector | list | map | unordered_map |
|---|---|---|---|---|
| 随机访问 | O(1) | O(n) | O(log n) | — |
| 尾部插入 | O(1) 均摊 | O(1) | O(log n) | O(1) 均摊 |
| 中间插入 | O(n) | O(1) | — | — |
| 按 key 查找 | — | — | O(log n) | O(1) 平均 |

## 6. 组合拳：一个完整例子

```cpp
#include <algorithm>
#include <numeric>
#include <vector>
#include <string>
#include <iostream>

int main() {
    std::vector<int> nums{4, 1, 7, 3, 9, 2, 8, 5, 6};

    // 1. 排序 + 2. 过滤偶数 + 3. 平方 + 4. 求和，一条链表达清楚
    std::sort(nums.begin(), nums.end());
    nums.erase(std::remove_if(nums.begin(), nums.end(),
                              [](int x){ return x % 2 == 0; }),
               nums.end());
    int result = std::accumulate(nums.begin(), nums.end(), 0,
                                 [](int acc, int x){ return acc + x * x; });

    std::cout << "奇数平方和 = " << result << '\n';
}
```

## 自测

1. 为什么说「容器管内存，算法管逻辑」？
2. `vector` 和 `list` 的底层区别？为什么默认用 `vector`？
3. `map` 和 `unordered_map` 的底层和查找复杂度区别？
4. `v.end()` 指向哪里？区间为什么是左闭右开？
5. 为什么 `remove_if` 要配合 `erase` 才能真正删除元素？

> 跑 `代码/05_stl.cpp`，把所有算法都试一遍。
