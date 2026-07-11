/**
 * @file
 * @brief Implementation of [Minimum Swaps to Sort](https://www.geeksforgeeks.org/minimum-number-swaps-required-sort-array/) algorithm (计算使数组有序所需的最小交换次数)
 *
 * @details
 * 该算法计算将一个无序数组转换为升序序列所需要的**最少交换次数**（Swap Sort / Min Swaps to Sort）。
 * 这是一个经典的图论和置换群应用问题。
 *
 * ### 算法原理（循环圈分解法 - Cycle Decomposition）
 * 1. 任何一个置换（Permutation）都可以唯一分解为若干个互不相交的“置换循环圈（Cycles）”。
 * 2. 我们将每个元素和它的原始索引绑定为一个对 `std::pair<Value, OriginalIndex>`。
 * 3. 对该对数组进行排序，排序后每个元素所在的新索引即为其目标正确位置。
 * 4. 遍历所有元素，通过已访问标记数组 `vis` 追踪它们所在的置换圈：
 *    - 如果当前元素已访问，或者已经在正确位置上（`arrPos[i].second == i`），则跳过。
 *    - 否则，顺着 `arrPos[j].second` 链条不断向下寻址，直到回到起点，形成一个闭合的循环圈。
 *    - 若一个循环圈的节点数为 `cycle_size`，要使该圈内的所有元素归位，所需要的最少交换次数为 `cycle_size - 1`。
 * 5. 将所有循环圈的交换次数累加，即得到整体的最少交换次数。
 *
 * 时间复杂度: $O(N \log N)$ (瓶颈在 std::sort 排序上，后续的图遍历由于每个节点只访问一次，时间复杂度为 $O(N)$)
 * 空间复杂度: $O(N)$ (需要存储配对数组与 visited 标记)
 * 
 * @author Unknown author
 */

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

/**
 * @brief 计算将数组排序所需的最小交换次数
 * @param arr 输入数组
 * @param n 数组长度
 * @returns 最小交换次数
 */
int minSwaps(int arr[], int n) {
    if (n <= 1) {
        return 0; // 0 个或 1 个元素已是有序，无需交换
    }

    // 1. 将元素和它的初始位置进行绑定配对
    std::pair<int, int> *arrPos = new std::pair<int, int>[n];
    for (int i = 0; i < n; i++) {
        arrPos[i].first = arr[i];
        arrPos[i].second = i;
    }

    // 2. 根据元素值大小进行排序，排序后每个元素的 second 就代表它在排序后应该去的位置
    std::sort(arrPos, arrPos + n);

    // 3. vis 用以记录元素是否已被计入置换循环圈中
    std::vector<bool> vis(n, false);

    int ans = 0; // 累计总交换次数

    // 4. 遍历每个元素，寻找其所在的置换圈
    for (int i = 0; i < n; i++) {
        // 如果该节点已访问过，或已经在排好序的正确位置上，则跳过
        if (vis[i] || arrPos[i].second == i) {
            continue;
        }

        // 5. 顺着置换链条深入寻找，统计当前循环圈的大小
        int cycle_size = 0;
        int j = i;
        while (!vis[j]) {
            vis[j] = true; // 标记为已访问

            // 移动到当前节点应该去的下一个目标节点位置
            j = arrPos[j].second;
            cycle_size++;
        }

        // 6. 如果检测到了大小大于 0 的循环圈，则需要交换次数为 (cycle_size - 1)
        if (cycle_size > 0) {
            ans += (cycle_size - 1);
        }
    }

    delete[] arrPos; // 释放动态数组内存

    return ans; // 返回最终的最少交换次数
}

/**
 * @brief 主函数
 */
int main() {
    int arr[] = {6, 7, 8, 1, 2, 3, 9, 12};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    // 预期最少交换次数
    std::cout << "Minimum swaps required to sort: " << minSwaps(arr, n) << std::endl;
    return 0;
}
