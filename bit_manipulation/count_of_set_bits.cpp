/**
 * @file
 * @brief Implementation to [count number of set bits of a number] (计算二进制表示中包含的 1 的个数)
 *
 * @details
 * 给定一个无符号 64 位整数，计算其二进制表示中 set bit（即数字 '1'）的总个数。
 * 
 * ### 柯尼汉算法原理 (Brian Kernighan's Algorithm)
 * 采用 `n & (n - 1)` 运算。每次该操作都会将 `n` 中最低位的 '1' 消除并变为 '0'。
 * 循环直至 `n` 变零，循环执行的次数就是二进制中 1 的总个数。
 * 例如：如果数值 13 (二进制 1101) 含有 3 个 1，则循环体只需运行 3 次即可，效率优于普通的逐位右移比较。
 *
 * 最坏时间复杂度: O(log n)，即与数字的二进制位数成比例，但实际仅与 1 的个数 K 相关，为 O(K)。
 * 空间复杂度: O(1)
 * 
 * @author [Swastika Gupta](https://github.com/Swastyy)
 * @author [Prashant Thakur](https://github.com/prashant-th18)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>   /// 用于 std::uint64_t
#include <iostream>  /// 用于输入输出

/**
 * @namespace bit_manipulation
 * @brief 位运算算法命名空间
 */
namespace bit_manipulation {
/**
 * @namespace count_of_set_bits
 * @brief 包含 1 的个数计算相关命名空间
 */
namespace count_of_set_bits {
/**
 * @brief 计算数字 n 的二进制中 1 的个数
 * @param n 输入的无符号 64 位整数
 * @returns 1 的个数统计值
 */
std::uint64_t countSetBits(std::uint64_t n) {
    std::uint64_t count = 0;  // 累加 1 的计数

    while (n != 0) {
        ++count;
        n = (n & (n - 1)); // 消除二进制表示中最右边的 '1'
    }
    return count;
}
}  // namespace count_of_set_bits
}  // namespace bit_manipulation

/**
 * @brief 单元自测用例
 */
static void test() {
    // 4  (二进制 0100) -> 1
    assert(bit_manipulation::count_of_set_bits::countSetBits(4) == 1);
    // 6  (二进制 0110) -> 2
    assert(bit_manipulation::count_of_set_bits::countSetBits(6) == 2);
    // 13 (二进制 1101) -> 3
    assert(bit_manipulation::count_of_set_bits::countSetBits(13) == 3);
    // 9  (二进制 1001) -> 2
    assert(bit_manipulation::count_of_set_bits::countSetBits(9) == 2);
    // 15 (二进制 1111) -> 4
    assert(bit_manipulation::count_of_set_bits::countSetBits(15) == 4);
    // 25 (二进制 11001) -> 3
    assert(bit_manipulation::count_of_set_bits::countSetBits(25) == 3);
    // 97 (二进制 1100001) -> 3
    assert(bit_manipulation::count_of_set_bits::countSetBits(97) == 3);
    // 31 (二进制 11111) -> 5
    assert(bit_manipulation::count_of_set_bits::countSetBits(31) == 5);
    
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
