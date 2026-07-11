/**
 * @file
 * @brief Implementation of [Mo's Algorithm](https://cp-algorithms.com/data_structures/mos_algorithm.html) (莫队算法实现)
 *
 * @details
 * 莫队算法（Mo's Algorithm）是由莫涛提出的一种高效的**离线查询分块算法**。
 * 它的核心思想是对所有查询区间进行合理的排序，然后利用双指针维护当前区间的答案，通过极其微小的移动（一步一步地扩展或收缩）来推导相邻区间的答案。
 *
 * ### 排序策略 (分块贪心排序)
 * 1. 将原序列划分为大小为 $\sqrt{N}$ 的分块。
 * 2. 对所有查询 `(L, R)` 进行排序：
 *    - 优先按照左端点 $L$ 所在的块编号升序排序。
 *    - 若左端点在同一个块内，则按照右端点 $R$ 升序（或奇偶交替排序）排序。
 * 3. 排序后，双指针 `left` 和 `right` 的总移动距离可以证明达到最优的 $O((N + Q) \sqrt{N})$。
 *
 * 时间复杂度: $O((N + Q) \sqrt{N})$
 * 空间复杂度: $O(N + Q)$
 *
 * @note
 * 【原版代码的严重越界崩溃与逻辑初始化错误 Bug 审计与修复】：
 * 1. **未离散化导致数组越界崩溃 Bug**：原代码直接使用固定大小数组 `cnt[N]`。
 *    若输入元素 `a[i]` 包含负数或大于 $10^6$ 的大数（例如 $2 \cdot 10^6$），执行 `cnt[a[index]]++` 会直接发生**越界写内存崩溃**。
 *    **修复**：引入**坐标离散化（Coordinate Compression）**技术，将原数组所有元素映射到 `[0, unique_count - 1]` 范围，使其对任意大数和负数安全。
 * 2. **区间双指针初始化逻辑错误 Bug**：原代码初始化 `left = 0, right = 0;` 但此时并没有执行 `add(0)`。
 *    如果在第一个查询中 `left` 向右移动，会直接对未曾添加的 `a[0]` 调用 `remove`，导致频次 `cnt` 变为负数，计数彻底错乱。
 *    **修复**：将指针初始化为标准空区间 `left = 0, right = -1`，并调整四条 `while` 移动链的顺序：
 *    **先扩大区间（左指针左移，右指针右移），再缩小区间（左指针右移，右指针左移）**，保证频次统计绝对正确。
 *
 * @author Unknown author
 */

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

/**
 * @namespace range_queries
 * @brief 区间查询命名空间
 */
namespace range_queries {
/**
 * @namespace mo_algorithm
 * @brief 莫队算法相关命名空间
 */
namespace mo_algorithm {

// 查询结构体
struct Query {
    int l, r, id;
};

int bucket_size = 0;

/**
 * @brief 莫队排序比较函数
 */
bool compareQuery(const Query& x, const Query& y) {
    int block_x = x.l / bucket_size;
    int block_y = y.l / bucket_size;
    if (block_x != block_y) {
        return block_x < block_y;
    }
    // 奇偶块交替排序优化：若块号为奇数，右端点降序；偶数则升序。这能减少右指针的往返移动
    return (block_x & 1) ? (x.r < y.r) : (x.r > y.r);
}

}  // namespace mo_algorithm
}  // namespace range_queries

/**
 * @brief 主函数
 */
int main() {
    int n = 0;
    if (!(std::cin >> n) || n <= 0) {
        return 0;
    }

    std::vector<int> a(n);
    std::vector<int> temp(n);
    for (int i = 0; i < n; i++) {
        std::cin >> a[i];
        temp[i] = a[i];
    }

    // 1. 坐标离散化：解决大数与负数越界访问 cnt 数组的致命 Bug
    std::sort(temp.begin(), temp.end());
    temp.erase(std::unique(temp.begin(), temp.end()), temp.end());
    for (int i = 0; i < n; i++) {
        a[i] = std::lower_bound(temp.begin(), temp.end(), a[i]) - temp.begin();
    }

    int t = 0; // 查询数量
    if (!(std::cin >> t) || t <= 0) {
        return 0;
    }

    range_queries::mo_algorithm::bucket_size = static_cast<int>(std::ceil(std::sqrt(n)));
    std::vector<range_queries::mo_algorithm::Query> q(t);
    for (int i = 0; i < t; i++) {
        std::cin >> q[i].l >> q[i].r;
        q[i].l--; // 转换为 0-indexed
        q[i].r--; // 转换为 0-indexed
        q[i].id = i;
    }

    // 对查询按分块进行排序
    std::sort(q.begin(), q.end(), range_queries::mo_algorithm::compareQuery);

    std::vector<int> cnt(temp.size(), 0); // 频次数组，大小等于离散化后不同元素的数量
    std::vector<int> answers(t);          // 记录每个查询的答案
    int current_ans = 0;                  // 当前窗口区间内不同数字的个数

    // 辅助添加函数
    auto add = [&](int index) {
        cnt[a[index]]++;
        if (cnt[a[index]] == 1) {
            current_ans++;
        }
    };

    // 辅助移除函数
    auto remove = [&](int index) {
        cnt[a[index]]--;
        if (cnt[a[index]] == 0) {
            current_ans--;
        }
    };

    // 2. 双指针维护区间：核心修复，初始化为空区间 [0, -1]
    int left = 0, right = -1;

    for (int i = 0; i < t; i++) {
        int L = q[i].l;
        int R = q[i].r;

        // 核心修复：调整移动顺序，先扩区间，后缩区间
        // 1. 左指针左移：扩大区间
        while (left > L) {
            left--;
            add(left);
        }
        // 2. 右指针右移：扩大区间
        while (right < R) {
            right++;
            add(right);
        }
        // 3. 左指针右移：缩小区间
        while (left < L) {
            remove(left);
            left++;
        }
        // 4. 右指针左移：缩小区间
        while (right > R) {
            remove(right);
            right--;
        }

        answers[q[i].id] = current_ans;
    }

    // 按原始查询顺序输出答案
    for (int i = 0; i < t; i++) {
        std::cout << answers[i] << "\n";
    }

    return 0;
}
