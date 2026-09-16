// 05_stl.cpp —— STL 容器与算法
// 编译：g++ -std=c++17 -Wall -Wextra 05_stl.cpp -o stl && ./stl

#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <numeric>
#include <string>

// 打印 vector 的小工具
void print_vec(const std::vector<int>& v, const char* label) {
    std::cout << "  " << label << ": ";
    for (int x : v) std::cout << x << ' ';
    std::cout << "\n";
}

// ============================================================
// 1. 容器选择速览
// ============================================================
void demo_containers() {
    std::cout << "\n===== 1. 容器选择 =====\n";

    std::vector<int> v{1, 2, 3};          // 动态数组：默认首选
    v.push_back(4);

    std::deque<int> d{1, 2, 3};           // 双端队列：头尾都 O(1)
    d.push_front(0);
    d.push_back(4);

    std::list<int> l{1, 2, 3};            // 双向链表：任意位置插入 O(1)
    l.insert(l.begin(), 0);

    std::map<std::string, int> m;         // 红黑树：有序
    m["banana"] = 2;
    m["apple"]  = 1;

    std::unordered_map<std::string, int> um;  // 哈希表：无序，O(1) 平均
    um["banana"] = 2;
    um["apple"]  = 1;

    std::set<int> s{3, 1, 2, 1};          // 自动去重 + 有序
    std::cout << "  vector 尾部=" << v.back()
              << ", deque 头=" << d.front()
              << ", list 头=" << l.front() << "\n";

    std::cout << "  map(有序): ";
    for (auto& [k, val] : m) std::cout << k << "=" << val << " ";
    std::cout << "\n  set(去重有序): ";
    for (int x : s) std::cout << x << ' ';
    std::cout << "\n";
}

// ============================================================
// 2. 常用算法
// ============================================================
void demo_algorithms() {
    std::cout << "\n===== 2. 常用算法 =====\n";
    std::vector<int> v{5, 2, 8, 1, 9, 3, 7, 4, 6};

    // sort 排序
    std::vector<int> sorted = v;
    std::sort(sorted.begin(), sorted.end());
    print_vec(sorted, "sort 升序");

    // find_if 按条件查找
    auto it = std::find_if(v.begin(), v.end(), [](int x) { return x > 5; });
    std::cout << "  第一个 >5 的元素是 " << *it << "\n";

    // count_if 计数
    int even = std::count_if(v.begin(), v.end(), [](int x) { return x % 2 == 0; });
    std::cout << "  偶数个数 = " << even << "\n";

    // transform 变换
    std::vector<int> sq(v.size());
    std::transform(v.begin(), v.end(), sq.begin(), [](int x) { return x * x; });
    print_vec(sq, "transform 平方");

    // accumulate 求和
    int total = std::accumulate(v.begin(), v.end(), 0);
    std::cout << "  求和 = " << total << "\n";

    // min_element / max_element
    auto mn = std::min_element(v.begin(), v.end());
    auto mx = std::max_element(v.begin(), v.end());
    std::cout << "  最小 = " << *mn << ", 最大 = " << *mx << "\n";

    // lower_bound（要求有序）：第一个 >= 5 的位置
    std::cout << "  lower_bound(5) 的值 = " << *std::lower_bound(sorted.begin(), sorted.end(), 5) << "\n";
}

// ============================================================
// 3. erase-remove 惯用法：按条件删除
// ============================================================
void demo_erase_remove() {
    std::cout << "\n===== 3. erase-remove 删除偶数 =====\n";
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    print_vec(v, "删除前");

    // remove_if 只把要保留的挪到前面，真正删除靠 erase
    v.erase(std::remove_if(v.begin(), v.end(),
                           [](int x) { return x % 2 == 0; }),
            v.end());
    print_vec(v, "删除偶数后");
}

// ============================================================
// 4. 组合拳：排序 → 过滤 → 变换 → 聚合
// ============================================================
void demo_pipeline() {
    std::cout << "\n===== 4. 组合拳：奇数平方和 =====\n";
    std::vector<int> nums{4, 1, 7, 3, 9, 2, 8, 5, 6};

    std::sort(nums.begin(), nums.end());
    nums.erase(std::remove_if(nums.begin(), nums.end(),
                              [](int x) { return x % 2 == 0; }),
               nums.end());
    int result = std::accumulate(nums.begin(), nums.end(), 0,
                                 [](int acc, int x) { return acc + x * x; });
    print_vec(nums, "过滤偶数后");
    std::cout << "  奇数平方和 = " << result << "\n";
}

int main() {
    demo_containers();
    demo_algorithms();
    demo_erase_remove();
    demo_pipeline();
    return 0;
}
