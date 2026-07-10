/**
 * @file
 * @brief [Find whether a given number is power of 2](https://www.geeksforgeeks.org/program-to-find-whether-a-given-number-is-power-of-2/) (通过位运算判断数字是否为 2 的幂次方)
 *
 * @details
 * 给定一个正整数，判断它是否为 2 的幂次方数（如 1, 2, 4, 8, 16...）。
 * 
 * ### 位运算原理
 * 1. 2 的幂次方数在二进制表示中具有唯一特征：**仅包含一个比特位 '1'**（例如：4 的二进制是 100，8 的二进制是 1000）。
 * 2. 如果我们将一个 2 的幂次方数减去 1，这个唯一的 '1' 会变成 '0'，而其右侧的所有位都会变成 '1'（例如：4 - 1 = 3，其二进制为 011）。
 * 3. 此时，对两者执行按位与运算：`n & (n - 1)`，结果必然为 0（例如：100 & 011 = 000）。
 * 4. 特判：为了排除 0 和负数，增加 `n > 0` 约束限制。
 *
 * 时间复杂度: O(1)
 * 空间复杂度: O(1)
 * 
 * @author [Prafful Gupta](https://github.com/EcstaticPG-25811)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>   /// 用于 std::int64_t
#include <iostream>  /// 用于输入输出

/**
 * @namespace bit_manipulation
 * @brief 位运算算法命名空间
 */
namespace bit_manipulation {
/**
 * @brief 判断数字 n 是否为 2 的幂次方
 * @param n 待判断的 64 位整数
 * @returns `true` 是 2 的幂；`false` 否则
 */
bool isPowerOfTwo(std ::int64_t n) {
    return n > 0 && !(n & (n - 1));
}
}  // namespace bit_manipulation

/**
 * @brief 单元自测用例
 */
static void test() {
    assert(bit_manipulation::isPowerOfTwo(4) == true);
    assert(bit_manipulation::isPowerOfTwo(6) == false);
    assert(bit_manipulation::isPowerOfTwo(13) == false);
    assert(bit_manipulation::isPowerOfTwo(64) == true);
    assert(bit_manipulation::isPowerOfTwo(15) == false);
    assert(bit_manipulation::isPowerOfTwo(32) == true);
    assert(bit_manipulation::isPowerOfTwo(97) == false);
    assert(bit_manipulation::isPowerOfTwo(1024) == true);
    
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
