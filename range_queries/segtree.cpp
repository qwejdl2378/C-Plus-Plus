/**
 * @file
 * @brief Implementation of [Segment Tree](https://en.wikipedia.org/wiki/Segment_tree) with Lazy Propagation (带懒惰传播的经典线段树实现)
 *
 * @details
 * 线段树（Segment Tree）是一种高度平衡的二叉树，用于管理区间段信息，特别适用于区间修改和区间查询。
 *
 * ### 核心操作与复杂度
 * - **区间建树 (ConsTree)**：通过递归自底向上计算区间值，时间复杂度 $O(N)$。
 * - **区间查询 (Query)**：在 $O(\log N)$ 时间内查询任意区间段的累加和。
 * - **区间更新 (Update)**：在 $O(\log N)$ 时间内对任意区间段的元素加上增量值（`delta`）。
 * - **空间复杂度**：$O(N)$（通常需要 $4N$ 的数组空间来以一维数组形式存储二叉树）。
 *
 * ### 懒惰传播技术 (Lazy Propagation)
 * - 当我们修改一个区间时，如果不做优化，每次修改需要更新到所有叶子节点，复杂度退化为 $O(N)$。
 * - 懒惰传播引入了 `lazy` 标记数组。当更新区间完全覆盖当前节点所代表的区间时：
 *   - 我们只将增量累加到当前节点上，并在此处打上懒惰标记（`lazy[pos] += delta`），不再继续向下递归更新其子节点。
 *   - 当后续的查询（`query`）或进一步更新（`update`）需要访问子节点时，再按需将该标记下传（Pushdown）给左右子节点。
 * - 这保证了每一次区间修改和区间查询都能稳定在 $O(\log N)$ 复杂度。
 *
 * @note
 * 【C++ 交互模式越界写 Bug 审计与修复】：
 * 1. **1-indexed 越界写入 Bug**：原代码在交互输入元素时使用：
 *    `for (int i = 1; i <= n; i++) { std::cin >> arr[i]; }`
 *    由于 `arr` 的初始化大小为 `n`（`std::vector<int64_t> arr(n);`），其合法索引范围为 `[0, n-1]`。
 *    在 `i = n` 时，执行 `std::cin >> arr[n]` 会直接导致**越界写内存崩溃**，且 `arr[0]` 始终保持为 0。
 *    **修复**：重构循环逻辑为标准的 0-indexed 读入，并在提示处输出 `i + 1` 以友好显示。
 * 2. **零节点安全防护**：当用户输入 `n == 0` 时，计算 `log2(0)` 会引发数学错误或负无穷溢出崩溃。
 *    **修复**：在 `main` 接口增加了非空防护。
 *
 * @author [Madhav Gaba](https://github.com/madhavgaba)
 * @author [Soham Roy](https://github.com/sohamroy19)
 */

#include <cassert>   /// 用于 assert 断言
#include <cmath>     /// 用于 std::log2, std::ceil, std::pow
#include <cstdint>   /// 用于 std::uint64_t, int64_t
#include <iostream>  /// 用于输入输出
#include <vector>    /// 用于 std::vector

/**
 * @brief 递归构建线段树
 * @param arr 原始数据数组
 * @param segtree 线段树数组指针
 * @param low 当前节点代表区间的左边界
 * @param high 当前节点代表区间的右边界
 * @param pos 当前节点在线段树数组中的索引
 */
void ConsTree(const std::vector<int64_t> &arr, std::vector<int64_t> *segtree,
              uint64_t low, uint64_t high, uint64_t pos) {
    if (low == high) {
        (*segtree)[pos] = arr[low];
        return;
    }

    uint64_t mid = low + (high - low) / 2;
    ConsTree(arr, segtree, low, mid, 2 * pos + 1);       // 递归建左子树
    ConsTree(arr, segtree, mid + 1, high, 2 * pos + 2);   // 递归建右子树
    (*segtree)[pos] = (*segtree)[2 * pos + 1] + (*segtree)[2 * pos + 2]; // 汇总和
}

/**
 * @brief 区间和查询函数（带懒惰标记下传）
 * @param segtree 线段树数组指针
 * @param lazy 懒惰标记数组指针
 * @param qlow 查询区间左边界
 * @param qhigh 查询区间右边界
 * @param low 当前节点区间左边界
 * @param high 当前节点区间右边界
 * @param pos 当前节点索引
 * @returns 查询区间的和
 */
int64_t query(std::vector<int64_t> *segtree, std::vector<int64_t> *lazy,
              uint64_t qlow, uint64_t qhigh, uint64_t low, uint64_t high,
              uint64_t pos) {
    // 1. 无交集直接返回 0
    if (low > high || qlow > high || low > qhigh) {
        return 0;
    }

    // 2. 下传懒惰标记（Pushdown）
    if ((*lazy)[pos] != 0) {
        (*segtree)[pos] += (*lazy)[pos] * (high - low + 1); // 贡献增量

        if (low != high) { // 非叶子节点，向下传递标记
            (*lazy)[2 * pos + 1] += (*lazy)[pos];
            (*lazy)[2 * pos + 2] += (*lazy)[pos];
        }
        (*lazy)[pos] = 0; // 清空当前节点标记
    }

    // 3. 当前区间被查询区间完全覆盖
    if (qlow <= low && qhigh >= high) {
        return (*segtree)[pos];
    }

    // 4. 部分相交，拆分查询
    uint64_t mid = low + (high - low) / 2;
    return query(segtree, lazy, qlow, qhigh, low, mid, 2 * pos + 1) +
           query(segtree, lazy, qlow, qhigh, mid + 1, high, 2 * pos + 2);
}

/**
 * @brief 区间值更新函数（在 [start, end] 区间累加值 delta，带懒惰标记处理）
 */
void update(std::vector<int64_t> *segtree, std::vector<int64_t> *lazy,
            int64_t start, int64_t end, int64_t delta, uint64_t low,
            uint64_t high, uint64_t pos) {
    if (low > high) {
        return;
    }

    // 1. 下传现有的懒惰标记
    if ((*lazy)[pos] != 0) {
        (*segtree)[pos] += (*lazy)[pos] * (high - low + 1);

        if (low != high) {
            (*lazy)[2 * pos + 1] += (*lazy)[pos];
            (*lazy)[2 * pos + 2] += (*lazy)[pos];
        }
        (*lazy)[pos] = 0;
    }

    // 2. 完全无交集，直接返回
    if (start > high || end < low) {
        return;
    }

    // 3. 完全覆盖，打上新的懒惰标记并返回（贪心终点）
    if (start <= low && end >= high) {
        (*segtree)[pos] += delta * (high - low + 1);

        if (low != high) {
            (*lazy)[2 * pos + 1] += delta;
            (*lazy)[2 * pos + 2] += delta;
        }
        return;
    }

    // 4. 部分交集，下分递归并回溯更新当前节点和
    uint64_t mid = low + (high - low) / 2;
    update(segtree, lazy, start, end, delta, low, mid, 2 * pos + 1);
    update(segtree, lazy, start, end, delta, mid + 1, high, 2 * pos + 2);
    (*segtree)[pos] = (*segtree)[2 * pos + 1] + (*segtree)[2 * pos + 2];
}

/**
 * @brief 单元自测用例
 */
static void test() {
    auto max = static_cast<int64_t>(2 * std::pow(2, std::ceil(std::log2(7))) - 1);
    assert(max == 15);

    std::vector<int64_t> arr{1, 2, 3, 4, 5, 6, 7}, lazy(max, 0), segtree(max, 0);
    ConsTree(arr, &segtree, 0, 7 - 1, 0);

    // 查询区间 [1, 5] (2+3+4+5+6 = 20)
    assert(query(&segtree, &lazy, 1, 5, 0, 7 - 1, 0) == 20);

    // 区间 [2, 4] 加上 1 (原数据为 3,4,5 -> 变为 4,5,6)
    update(&segtree, &lazy, 2, 4, 1, 0, 7 - 1, 0);
    assert(query(&segtree, &lazy, 1, 5, 0, 7 - 1, 0) == 2 + 4 + 5 + 6 + 6); // 23

    // 区间 [0, 6] 加上 -2
    update(&segtree, &lazy, 0, 6, -2, 0, 7 - 1, 0);
    assert(query(&segtree, &lazy, 0, 4, 0, 7 - 1, 0) == -1 + 0 + 2 + 3 + 4); // 8
    
    std::cout << "All Segment Tree self-tests passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测

    std::cout << "Enter number of elements: ";
    uint64_t n = 0;
    if (!(std::cin >> n) || n == 0) {
        return 0; // 防御 0 元素崩溃
    }

    // 计算线段树数组所需要的最大尺寸 2 * 2^(ceil(log2(n))) - 1
    auto max = static_cast<uint64_t>(2 * std::pow(2, std::ceil(std::log2(n))) - 1);
    std::vector<int64_t> arr(n, 0), lazy(max, 0), segtree(max, 0);

    int choice = 0;
    std::cout << "\nDo you wish to enter each number?:\n"
                 "1: Yes\n"
                 "0: No (default initialize them to 0)\n";
    std::cin >> choice;
    
    if (choice == 1) {
        std::cout << "Enter " << n << " numbers:\n";
        // 核心修复：更正 1-indexed 为 0-indexed 的输入循环，根除越界写入 Bug
        for (uint64_t i = 0; i < n; i++) {
            std::cout << i + 1 << ": ";
            std::cin >> arr[i];
        }
    }

    ConsTree(arr, &segtree, 0, n - 1, 0);

    do {
        std::cout << "\nMake your choice:\n"
                     "1: Range update (input)\n"
                     "2: Range query (output)\n"
                     "0: Exit\n";
        std::cin >> choice;

        if (choice == 1) {
            std::cout << "Enter 1-indexed lower bound, upper bound & value:\n";
            uint64_t p = 1, q = 1;
            int64_t v = 0;
            std::cin >> p >> q >> v;
            update(&segtree, &lazy, p - 1, q - 1, v, 0, n - 1, 0);
        } else if (choice == 2) {
            std::cout << "Enter 1-indexed lower bound & upper bound:\n";
            uint64_t p = 1, q = 1;
            std::cin >> p >> q;
            std::cout << "Range Sum: " << query(&segtree, &lazy, p - 1, q - 1, 0, n - 1, 0) << "\n";
        }
    } while (choice > 0);

    return 0;
}
