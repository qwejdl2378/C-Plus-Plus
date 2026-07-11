/**
 * @file
 * @brief O(N log N) Implementation of the [Longest Increasing Subsequence](https://en.wikipedia.org/wiki/Longest_increasing_subsequence) algorithm (基于 std::set 的 O(N log N) 最长递增子序列算法实现)
 * @details
 * 采用耐性排序（Patience Sorting）思想的变种，利用 `std::set` 动态维护当前递增子序列的“最小尾部值”候选集。
 * 
 * ### 算法原理
 * 1. 维护一个有序集合 `active`，代表当前能达到的最长递增子序列的潜在最优结尾元素。
 * 2. 依次遍历每个数组元素 `arr[i]`，通过二分查找 `active.lower_bound(arr[i])` 寻找第一个大于或等于 `arr[i]` 的元素：
 *    - 如果不存在（即返回 `active.end()`），说明 `arr[i]` 严格大于当前已知的全部结尾候选，子序列长度可以增加 1，故将 `arr[i]` 直接插入 `active`。
 *    - 如果存在一个更大的元素 `val > arr[i]`，我们用较小的 `arr[i]` 替换它（先 `erase` 后 `insert`），从而为后续元素留出更大的递增空间。
 *    - 如果刚好存在一个等于 `arr[i]` 的元素，则不作任何改变。
 * 3. 最终 `active` 的大小即为最长递增子序列的长度。
 *
 * 时间复杂度: O(N log N)，因为每次二分查找和集合更新开销均为 O(log N)。
 * 空间复杂度: O(N)
 */

#include <iostream>
#include <set>
#include <vector>

using namespace std;

/**
 * @brief 使用 std::set 计算 LIS 长度
 * @param arr 输入的整型 vector 数组
 * @param n 数组大小
 * @returns 最长递增子序列长度
 */
int LIS(const std::vector<int>& arr, int n) {
    set<int> active;  // 维护当前活跃递增序列结尾最小可能值的集合
    active.insert(arr[0]);

    for (int i = 1; i < n; ++i) {
        auto get = active.lower_bound(arr[i]); // 二分查找大于等于 arr[i] 的元素
        if (get == active.end()) {
            active.insert(arr[i]); // 找不到，说明是最大值，向后扩展 LIS 长度
        }
        else {
            int val = *get;
            if (val > arr[i]) {
                // 找到一个比当前元素大的结尾候选，使用较小值替换之，以优化后续拼接空间
                active.erase(get);
                active.insert(arr[i]);
            }
        }
    }
    return active.size(); // 返回最终 active 集合的大小
}

/**
 * @brief 主函数
 */
int main() {
    int n;
    cout << "Enter size of array: ";
    cin >> n;
    std::vector<int> a(n);
    cout << "Enter array elements: ";
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
    }
    cout << LIS(a, n) << endl;
    return 0;
}
