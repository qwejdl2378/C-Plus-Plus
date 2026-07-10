/**
 * @file
 * @brief Implementation of the [Subset Sum](https://en.wikipedia.org/wiki/Subset_sum_problem) problem. (子集和问题求解)
 * @details
 * 给定一个数组和一个目标和值。本算法寻找所有累加和等于目标和值的**子集**（子集可以不连续），并返回此类子集的个数。
 * 本程序采用指数级时间复杂度 $O(2^N \cdot N)$ 的位掩码（Bitmask）暴力生成法来实现。
 *
 * @author [Swastika Gupta](https://github.com/Swastyy)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>
#include <iostream>  /// 用于输入输出
#include <vector>    /// 用于 std::vector

/**
 * @brief 回溯算法命名空间
 * @namespace backtracking
 */
namespace backtracking {
/**
 * @brief 子集和算法命名空间
 * @namespace subset_sum
 */
namespace subset_sum {
/**
 * @brief 计算和为指定值 sum 的所有子集数量的主函数（位掩码实现）
 * @param sum 目标子集和
 * @param in_arr 传入的元素数组
 * @returns 累加和为目标值 sum 的子集个数
 */
uint64_t number_of_subsets(int32_t sum, const std::vector<int32_t> &in_arr) {
    int32_t nelement = in_arr.size();
    uint64_t count_of_subset = 0;

    // 数组长度为 N，则共有 2^N 个可能的子集。
    // 使用变量 i 从 0 循环到 2^N - 1，i 的二进制形式中第 j 位为 1 代表选中了原数组中的第 j 个元素。
    for (int32_t i = 0; i < (1 << (nelement)); i++) {
        int32_t check = 0; // 当前子集的元素和
        for (int32_t j = 0; j < nelement; j++) {
            // 检测二进制 i 中第 j 位是否为 1
            if (i & (1 << j)) {
                check += (in_arr[j]);
            }
        }
        // 如果当前子集的和等于目标和，统计数加一
        if (check == sum) {
            count_of_subset++;
        }
    }
    return count_of_subset;
}
}  // namespace subset_sum
}  // namespace backtracking

/**
 * @brief 自测用例集
 */
static void test() {
    // 测试用例 1
    std::cout << "1st test ";
    std::vector<int32_t> array1 = {-7, -3, -2, 5, 8};
    assert(backtracking::subset_sum::number_of_subsets(0, array1) ==
           2);  // first argument in subset_sum function is the required sum and
                // second is the input array
    std::cout << "passed" << std::endl;

    // 测试用例 2
    std::cout << "2nd test ";
    std::vector<int32_t> array2 = {1, 2, 3, 3};
    assert(backtracking::subset_sum::number_of_subsets(6, array2) ==
           3);  // here we are expecting 3 subsets which sum up to 6 i.e.
                // {(1,2,3),(1,2,3),(3,3)}
    std::cout << "passed" << std::endl;

    // 测试用例 3
    std::cout << "3rd test ";
    std::vector<int32_t> array3 = {1, 1, 1, 1};
    assert(backtracking::subset_sum::number_of_subsets(1, array3) ==
           4);  // here we are expecting 4 subsets which sum up to 1 i.e.
                // {(1),(1),(1),(1)}
    std::cout << "passed" << std::endl;

    // 测试用例 4
    std::cout << "4th test ";
    std::vector<int32_t> array4 = {3, 3, 3, 3};
    assert(backtracking::subset_sum::number_of_subsets(6, array4) ==
           6);  // here we are expecting 6 subsets which sum up to 6 i.e.
                // {(3,3),(3,3),(3,3),(3,3),(3,3),(3,3)}
    std::cout << "passed" << std::endl;

    // 测试用例 5
    std::cout << "5th test ";
    std::vector<int32_t> array5 = {};
    assert(backtracking::subset_sum::number_of_subsets(6, array5) ==
           0);  // here we are expecting 0 subsets which sum up to 6 i.e. we
                // cannot select anything from an empty array
    std::cout << "passed" << std::endl;
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行测试用例
    return 0;
}
