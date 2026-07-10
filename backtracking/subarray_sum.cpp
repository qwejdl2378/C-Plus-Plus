/**
 * @file
 * @brief [Subset-sum](https://en.wikipedia.org/wiki/Subset_sum_problem) (only continuous subsets) problem (连续子数组和问题求解)
 * @details 给定一个数组和一个目标和值。本算法寻找所有累加和等于目标和值的**连续子数组**，并返回此类子数组的数量。
 * 该实现方法的时间复杂度为 O(n)，空间复杂度为 O(n)（采用前缀和与哈希表优化）。
 * 注：尽管此文件放在回溯目录下，但该特定实现采用了更高效的前缀和双指针/哈希映射机制。
 *
 * @author [Swastika Gupta](https://github.com/Swastyy)
 */

#include <cassert>        /// 用于 assert 断言
#include <cstdint>
#include <iostream>       /// 用于输入输出
#include <unordered_map>  /// 用于无序哈希映射表
#include <vector>         /// 用于 std::vector

/**
 * @brief 回溯算法命名空间
 * @namespace backtracking
 */
namespace backtracking {
/**
 * @brief 连续子数组和算法命名空间
 * @namespace subarray_sum
 */
namespace subarray_sum {
/**
 * @brief 计算和为指定值 sum 的连续子数组数量的主函数
 * @param sum 需要查找的子数组目标累加和
 * @param in_arr 传入的源数组
 * @returns 累加和为目标值 sum 的连续子数组总个数
 */
uint64_t subarray_sum(int64_t sum, const std::vector<int64_t> &in_arr) {
    int64_t nelement = in_arr.size();
    int64_t count_of_subset = 0; // 符合条件的子数组总数
    int64_t current_sum = 0;     // 从首元素到当前遍历位置的累加前缀和
    
    // 哈希表 sumarray 用于存储每个前缀和出现的频率
    // 键（Key）：前缀和的值
    // 值（Value）：该前缀和出现的频次
    std::unordered_map<int64_t, int64_t> sumarray;  

    for (int64_t i = 0; i < nelement; i++) {
        current_sum += in_arr[i]; // 累加当前元素，更新前缀和

        // 情况 1：如果当前前缀和刚好等于目标值 sum，说明从 index 0 到当前位置的子数组符合条件
        if (current_sum == sum) {
            count_of_subset++;
        }
        
        // 情况 2：如果在之前的某个位置，前缀和的值为 (current_sum - sum)
        // 那么这两个位置之间的连续子数组的和必定为：current_sum - (current_sum - sum) = sum。
        // 我们从哈希表中查找 (current_sum - sum) 出现的次数，并将其累加到结果中。
        if (sumarray.find(current_sum - sum) != sumarray.end()) {
            count_of_subset += (sumarray[current_sum - sum]);
        }
        
        // 将当前前缀和放入哈希表中，频次加一
        sumarray[current_sum]++;
    }
    return count_of_subset;
}
}  // namespace subarray_sum
}  // namespace backtracking

/**
 * @brief 自测用例集
 */
static void test() {
    // 测试用例 1
    std::vector<int64_t> array1 = {-7, -3, -2, 5, 8};
    // 目标和为 0，符合条件的连续子数组为 {-3, -2, 5}，数量应为 1
    assert(backtracking::subarray_sum::subarray_sum(0, array1) == 1);
    std::cout << "1st test passed" << std::endl;

    // 测试用例 2
    std::vector<int64_t> array2 = {1, 2, 3, 3};
    // 目标和为 6，符合条件的子数组为 {1, 2, 3} 和 {3, 3}，数量应为 2
    assert(backtracking::subarray_sum::subarray_sum(6, array2) == 2);
    std::cout << "2nd test passed" << std::endl;

    // 测试用例 3
    std::vector<int64_t> array3 = {1, 1, 1, 1};
    // 目标和为 1，数量应为 4 ({1}, {1}, {1}, {1})
    assert(backtracking::subarray_sum::subarray_sum(1, array3) == 4);
    std::cout << "3rd test passed" << std::endl;

    // 测试用例 4
    std::vector<int64_t> array4 = {3, 3, 3, 3};
    // 目标和为 6，符合条件的是 {3, 3} (位置0,1)、{3, 3} (位置1,2)、{3, 3} (位置2,3)，数量应为 3
    assert(backtracking::subarray_sum::subarray_sum(6, array4) == 3);
    std::cout << "4th test passed" << std::endl;

    // 测试用例 5
    std::vector<int64_t> array5 = {};
    // 空数组，数量应为 0
    assert(backtracking::subarray_sum::subarray_sum(6, array5) == 0);
    std::cout << "5th test passed" << std::endl;
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行测试用例
    return 0;
}
