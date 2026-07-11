/**
 * @file
 * @brief Implementation of the [Subset Sum Problem](https://en.wikipedia.org/wiki/Subset_sum_problem) (子集和问题的记忆化递归算法实现)
 *
 * @details
 * 子集和问题（Subset Sum Problem）：给定一个整数数组和一个目标和 `targetSum`，
 * 判断数组中是否存在一个子集，其元素累加和恰好等于 `targetSum`。
 *
 * ### 智能设计（支持负数与负目标和）
 * 1. 传统的子集和算法通常采用二维布尔矩阵 `dp[n][target]`，这要求元素和目标和必须为非负数，否则无法作为数组下标索引。
 * 2. **本实现的亮点**：采用 `std::vector<std::unordered_map<int, bool>>` 作为备忘录（Memory Cache）。
 *    通过使用 `unordered_map`，允许 `targetSum` 为任意正数、负数或零，并且无需做下标偏移转换，依然能保持 $O(1)$ 平均时间的查找性能。
 *
 * ### 动态规划递推关系（自顶向下）
 * 设递归函数 `subset_sum_recursion(index, current_target)` 表示从索引 `index` 开始，能否凑出和 `current_target`：
 * - 边界状态：
 *   - 若 `current_target == 0`，找到合法子集，返回 `true`。
 *   - 若 `index == arr.size()`，遍历结束仍未凑出，返回 `false`。
 * - 递推关系：
 *   包含当前元素：`subset_sum_recursion(index + 1, current_target - arr[index])`
 *   或 不含当前元素：`subset_sum_recursion(index + 1, current_target)`
 *   只要有一者为真即可。
 *
 * 时间复杂度: O(N * |targetSum|)
 * 空间复杂度: O(N * |targetSum|)
 *
 * @author [KillerAV](https://github.com/KillerAV)
 */

#include <cassert>        /// 用于 assert 断言
#include <iostream>       /// 用于输入输出
#include <unordered_map>  /// 用于 std::unordered_map
#include <vector>         /// 用于 std::vector

/**
 * @namespace dynamic_programming
 * @brief 动态规划算法命名空间
 */
namespace dynamic_programming {
/**
 * @namespace subset_sum
 * @brief 子集和相关算法命名空间
 */
namespace subset_sum {
/**
 * @brief 记忆化递归子集和求解逻辑
 * @param arr 输入数据数组
 * @param targetSum 剩余所需达成的目标和
 * @param dp 记忆化哈希映射向量
 * @param index 当前决策的元素索引
 * @returns 是否存在合法子集
 */
bool subset_sum_recursion(const std::vector<int> &arr, int targetSum,
                           std::vector<std::unordered_map<int, bool>> *dp,
                           int index = 0) {
    if (targetSum == 0) {
        return true; // 成功凑齐目标和
    }
    if (index == arr.size()) {
        return false; // 数组元素已经用光，无法凑齐
    }

    // 检查缓存中是否已经存有该状态的计算结果
    if ((*dp)[index].count(targetSum)) {
        return (*dp)[index][targetSum];
    }

    // 分支探索：选择包含当前数 或 放弃当前数
    bool ans =
        subset_sum_recursion(arr, targetSum - arr[index], dp, index + 1) ||
        subset_sum_recursion(arr, targetSum, dp, index + 1);
    
    (*dp)[index][targetSum] = ans; // 写入备忘录中
    return ans;
}

/**
 * @brief 自顶向下入口函数
 * @param arr 输入数据数组
 * @param targetSum 目标和（可为负）
 * @returns 是否可以凑出目标和
 */
bool subset_sum_problem(const std::vector<int> &arr, const int targetSum) {
    size_t n = arr.size();
    // 使用 unordered_map 代替传统的矩阵以完美兼容负数索引
    std::vector<std::unordered_map<int, bool>> dp(n);
    return subset_sum_recursion(arr, targetSum, &dp);
}
}  // namespace subset_sum
}  // namespace dynamic_programming

/**
 * @brief 单元自测用例
 */
static void test() {
    // 准备测试数据
    std::vector<std::vector<int>> custom_input_arr(3);
    custom_input_arr[0] = std::vector<int>{1, -10, 2, 31, -6}; // 包含负数
    custom_input_arr[1] = std::vector<int>{2, 3, 4};
    custom_input_arr[2] = std::vector<int>{0, 1, 0, 1, 0};

    std::vector<int> custom_input_target_sum(3);
    custom_input_target_sum[0] = -14; // 负数目标和
    custom_input_target_sum[1] = 10;
    custom_input_target_sum[2] = 2;

    std::vector<int> calculated_output(3);

    for (int i = 0; i < 3; i++) {
        calculated_output[i] =
            dynamic_programming::subset_sum::subset_sum_problem(
                custom_input_arr[i], custom_input_target_sum[i]);
    }

    // 预期结果：
    // Test 1: {-10, 2, -6} 和为 -14 -> true
    // Test 2: 无法凑出 10 -> false
    // Test 3: {1, 1} 和为 2 -> true
    std::vector<bool> expected_output{true, false, true};

    for (int i = 0; i < 3; i++) {
        assert(expected_output[i] == calculated_output[i]);
    }

    std::cout << "All tests passed successfully!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
