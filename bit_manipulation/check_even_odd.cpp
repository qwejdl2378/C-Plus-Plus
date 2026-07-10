/**
 * @file
 * @brief Implementation to [Check if a number is Even or Odd using Bitwise Operator] (通过位运算符检查奇偶性)
 *
 * @details
 * 给定一个整数 N，利用位运算快速判断它是奇数还是偶数。
 * 二进制数的最低有效位（LSB, Least Significant Bit）直接决定了其奇偶性：
 * - 如果最低有效位是 0，说明它是偶数。
 * - 如果最低有效位是 1，说明它是奇数。
 *
 * 这可以通过与运算（&）和 1 组合进行高效判断。
 * - 如果 (N & 1) == 0，N 为偶数。
 * - 如果 (N & 1) == 1，N 为奇数。
 *
 * 最坏时间复杂度: O(1)
 * 空间复杂度: O(1)
 *
 * @author [Vedant Mukhedkar](https://github.com/git5v)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>   /// 用于 uint32_t 和 int64_t
#include <iostream>  /// 用于输入输出
#include <string>    /// 用于 std::string

/**
 * @namespace bit_manipulation
 * @brief 位运算算法命名空间
 */
namespace bit_manipulation {
/**
 * @namespace even_odd
 * @brief 奇偶校验位运算相关命名空间
 */
namespace even_odd {

/**
 * @brief 使用按位与判断数字是否为偶数
 * @param N 待检查的数值
 * @returns `true` 偶数；`false` 奇数
 */
bool is_even(std::int64_t N) {
    return (N & 1) == 0;
}

}  // namespace even_odd
}  // namespace bit_manipulation

/**
 * @brief 单元自测用例
 */
static void test() {
    using bit_manipulation::even_odd::is_even;

    // 测试偶数
    assert(is_even(0) == true);
    assert(is_even(2) == true);
    assert(is_even(100) == true);
    assert(is_even(-4) == true);
    assert(is_even(-1000) == true);

    // 测试奇数
    assert(is_even(1) == false);
    assert(is_even(3) == false);
    assert(is_even(101) == false);
    assert(is_even(-5) == false);
    assert(is_even(-999) == false);

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
