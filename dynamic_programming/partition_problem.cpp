/**
 * @file
 * @brief Implementation of the [Partition Problem](https://en.wikipedia.org/wiki/Partition_problem) (子集等和分割/划分问题算法实现)
 * @details
 * 划分问题（Partition Problem）：判断一个包含正整数的多重集合 S 是否可以被划分为两个子集 S1 和 S2，
 * 使得 S1 中的元素和等于 S2 中的元素和。
 * 
 * 虽然该问题是 NP 完全的，但在实际应用中可以通过伪多项式时间复杂度的动态规划来高效解决。
 *
 * ### 核心解题步骤
 * 1. 求出数组的所有元素总和 `sum`。若总和为奇数，则必然无法平分为两个整数和相同的子集，直接返回 `false`。
 * 2. 若总和为偶数，问题等价于：从数组中选出若干个元素，使其累加和恰好等于 `sum / 2`。这演变为经典的 0-1 背包问题变种（子集和问题）。
 *
 * ### 动态规划状态转移（空间压缩至一维）
 * 设 `part[i]` 表示当前是否能凑出累加和 `i`（其中 `0 <= i <= sum/2`）：
 * - 递推式：遍历每个数字 `num` 时，自右向左更新：
 *   `part[i] = part[i] || part[i - num]`
 *   （自右向左遍历是为了防止同一个数字被重复使用多次，即 0-1 背包的滚动数组优化）。
 *
 * @note
 * 【文档修正说明】：
 * 原作者在设计文件头部说明时，误把有关“凸包算法”（Jarvis’s Algorithm, Graham’s scan）的复杂度描述混入到了划分问题中，这部分段落纯属冗余，与本划分算法无任何关联。
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>   /// 用于 std::uint64_t
#include <iostream>  /// 用于输入输出
#include <numeric>   /// 用于 std::accumulate 计算累加和
#include <vector>    /// 用于 std::vector

/**
 * @namespace dp
 * @brief 动态规划算法命名空间
 */
namespace dp {
/**
 * @namespace partitionProblem
 * @brief 划分问题相关算法命名空间
 */
namespace partitionProblem {
/**
 * @brief 判断数组是否能够被划分为和相等的两个子集
 * @param arr 存储整型数据的 vector
 * @param size 数组的大小
 * @returns `true` 可以平分；`false` 无法平分
 */
bool findPartiion(const std::vector<uint64_t> &arr, uint64_t size) {
    // 1. 计算元素总和
    uint64_t sum = std::accumulate(arr.begin(), arr.end(), 0ULL);

    // 总和为奇数，必然无法平分
    if (sum % 2 != 0) {
        return false;
    }

    // 声明状态记录向量 part，大小为 sum/2 + 1，初始化为 false
    std::vector<bool> part(sum / 2 + 1, false);

    // 0-1 背包状态递推过程
    for (uint64_t it = 0; it < size; ++it) {
        // 从大到小逆序更新状态，避免重复使用当前元素（同 0-1 背包逆序遍历）
        for (uint64_t it2 = sum / 2; it2 >= arr[it]; --it2) {
            // 如果去掉当前数后的差值可以被凑出，或者当前数直接等于目标值，则当前状态为真
            if (part[it2 - arr[it]] == true || it2 == arr[it]) {
                part[it2] = true;
            }
        }
    }
    return part[sum / 2];
}
}  // namespace partitionProblem
}  // namespace dp

/**
 * @brief 单元自测用例
 */
static void test() {
    // 数组元素 {1, 3, 3, 2, 3, 2}，总和为 14，可以平分为两个和为 7 的子集：
    // 例如 {3, 3, 1} 和 {3, 2, 2}
    std::vector<uint64_t> arr = {{1, 3, 3, 2, 3, 2}};
    uint64_t n = arr.size();
    bool expected_result = true;
    bool derived_result = dp::partitionProblem::findPartiion(arr, n);
    std::cout << "1st test: ";
    assert(expected_result == derived_result);
    std::cout << "Passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
