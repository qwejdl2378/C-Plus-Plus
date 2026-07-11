/**
 * @file
 * @brief C++ program for maximum contiguous circular sum problem using [Kadane's Algorithm](https://en.wikipedia.org/wiki/Maximum_subarray_problem) (基于卡登算法求解最大环形子数组和)
 * @details
 * 最大环形子数组和：给定一个环形整数数组（即首尾相连），寻找具有最大和的连续子数组。
 *
 * ### 核心解题思路
 * 环形子数组的最大和可能以以下两种情况出现：
 * 1. **非环形跨越**：最大和子数组在数组中间部分，不需要跨越首尾。直接通过常规的 Kadane 算法求得 `max_so_far`。
 * 2. **环形跨越**：最大和子数组跨越了数组的末尾和开头。
 *    这种情况下，未被选中的剩余部分必然是一个**连续且具有最小和的子数组**。
 *    因此，跨越首尾的最大和等于 `total_sum - min_subarray_sum`。我们只需通过修改 Kadane 算法求得 `min_so_far`（最小子数组和）即可。
 * 最终的最大环形和即为 `max(max_so_far, total_sum - min_so_far)`。
 *
 * ### 特殊边界情况
 * 如果数组中全为负数，则 `min_so_far` 会等于整个数组的总和 `sum`。此时 `sum - min_so_far = 0` 相当于选空数组，但在非空子数组限制下，必须返回数组中的最大单个元素值，即直接返回 `max_so_far`。
 *
 * 时间复杂度: O(N)
 * 空间复杂度: O(1)
 */

#include <cassert>     /// 用于 assert 断言
#include <iostream>   /// 用于输入输出
#include <vector>     /// 用于 std::vector

/**
 * @namespace dynamic_programming
 * @brief 动态规划算法命名空间
 */
namespace dynamic_programming {
/**
 * @brief 计算最大连续环形子数组和
 * @param arr 传入的整型数据 vector
 * @returns 最大子数组和
 */
int maxCircularSum(std::vector<int>& arr) {
    // 边界情况：只有一个元素
    if (arr.size() == 1)
        return arr[0];
  
    // 1. 计算数组的全部元素之和
    int sum = 0;
    for (int i = 0; i < arr.size(); i++) {
        sum += arr[i];
    }
  
    // 初始化卡登算法状态变量
    int current_max = arr[0], max_so_far = arr[0];
    int current_min = arr[0], min_so_far = arr[0];
  
    // 一次遍历，同时执行求最大子数组和与最小子数组和的 Kadane 算法
    for (int i = 1; i < arr.size(); i++) {
        // 卡登最大子数组和递推
        current_max = std::max(current_max + arr[i], arr[i]);
        max_so_far = std::max(max_so_far, current_max);
  
        // 卡登最小子数组和递推
        current_min = std::min(current_min + arr[i], arr[i]);
        min_so_far = std::min(min_so_far, current_min);
    }
  
    // 如果所有元素都是负数，返回最大负数元素
    if (min_so_far == sum)
        return max_so_far;
  
    // 对比非环形最大值与环形最大值，返回较大者
    return std::max(max_so_far, sum - min_so_far);
}
}  // namespace dynamic_programming

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1
    // 输入: arr[] = {8, -8, 9, -9, 10, -11, 12}
    // 预期输出: 22 (选择跨首尾的 12, 8, -8, 9, -9, 10)
    std::vector<int> arr = {8, -8, 9, -9, 10, -11, 12}; 
    assert(dynamic_programming::maxCircularSum(arr) == 22);

    // 测试 2
    arr = {8, -8, 10, -9, 10, -11, 12};
    assert(dynamic_programming::maxCircularSum(arr) == 23);

    std::cout << "All tests have successfully passed!\n";
}

/**
 * @brief 主函数
 */
int main() {
     test();  // 运行自测
     return 0;
}
