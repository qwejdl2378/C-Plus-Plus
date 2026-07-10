/**
 * @file
 * @brief Implementation to find the non repeating integer in an array of repeating integers. [Single Number](https://leetcode.com/problems/single-number/) (寻找只出现一次的数字算法实现)
 *
 * @details
 * 给定一个非空整数数组，除了某个元素只出现一次以外，其余每个元素均出现两次。
 * 找出那个只出现了一次的元素。
 *
 * ### 异或（XOR）算法原理
 * 依据异或运算的数学性质：
 * 1. 恒等律：`a ^ 0 = a`
 * 2. 归零律：`a ^ a = 0`
 * 3. 交换律与结合律：`a ^ b ^ a = (a ^ a) ^ b = 0 ^ b = b`
 * 将数组中所有元素依次执行异或操作，出现两次的元素会在异或中两两相消归零，
 * 最终剩余的累积异或结果即为那个仅出现一次的数字。
 *
 * 时间复杂度: O(N)，只需遍历一次数组。
 * 空间复杂度: O(1)，仅需一个额外的累加变量。
 *
 * @author [Ravidev Pandey](https://github.com/literalEval)
 */

#include <cassert>   /// 用于 assert 断言
#include <iostream>  /// 用于输入输出
#include <vector>    /// 用于 std::vector

/**
 * @namespace bit_manipulation
 * @brief 位运算算法命名空间
 */
namespace bit_manipulation {
/**
 * @namespace find_non_repeating_integer
 * @brief 只出现一次数字查找相关命名空间
 */
namespace find_non_repeating_integer {
/**
 * @brief 寻找数组中唯一非重复出现的整数
 * @param nums 包含整数数据的 vector 数组
 * @returns 唯一只出现一次的整数
 */
int64_t find_non_repeating_integer(const std::vector<int>& nums) {
    int _xor = 0; // 累积异或值

    for (const int& num: nums) {
        _xor ^= num; // 依次进行异或累积
    }

    return _xor;
}
}  // namespace find_non_repeating_integer
}  // namespace bit_manipulation

/**
 * @brief 单元自测用例
 */
static void test() {
    std::vector<int> nums_one{1, 1, 2, 2, 4, 5, 5};
    std::vector<int> nums_two{203, 3434, 4545, 3434, 4545};
    std::vector<int> nums_three{90, 1, 3, 90, 3};

    // 4 是唯一非重复的数字
    assert(bit_manipulation::find_non_repeating_integer::
               find_non_repeating_integer(nums_one) == 4);
    // 203 是唯一非重复的数字
    assert(bit_manipulation::find_non_repeating_integer::
               find_non_repeating_integer(nums_two) == 203);
    // 1 是唯一非重复的数字
    assert(bit_manipulation::find_non_repeating_integer::
               find_non_repeating_integer(nums_three) == 1);

    std::cout << "All test cases successfully passed!" << std::endl;
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行自测
    return 0;
}
