/**
 * @file
 * @brief Implementation to [From the right, set the Kth bit in the binary representation of N] (将数字 N 的第 K 个二进制位设为 1)
 *
 * @details
 * 给定一个正整数 N 和一个偏移量 K。我们要将 N 的二进制表示中从右侧算起的第 K 个 bit 位强制置为 1（最低有效位 LSB 的位置是 0，第二低位是 1，依此类推）。
 * 
 * ### 位运算原理
 * 1. 首先构建一个只有第 K 位为 1，其余位为 0 的掩码 (Mask)：`pos = 1 << k`。
 * 2. 对原数 N 和掩码执行按位或运算：`N | pos`。这能确保第 K 位被设为 1，而 N 的其他二进制位保持不变。
 *
 * 时间复杂度: O(1)
 * 空间复杂度: O(1)
 * 
 * @author Aman Raj (https://github.com/aman2000raj)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>   /// 用于 std::uint64_t 和 std::int64_t
#include <iostream>  /// 用于输入输出

/**
 * @namespace bit_manipulation
 * @brief 位运算算法命名空间
 */
namespace bit_manipulation {
/**
 * @namespace set_kth_bit
 * @brief 设置第 K 个比特位相关命名空间
 */
namespace set_kth_bit {
/**
 * @brief 将 N 的第 k 个二进制位设为 1
 * @param N 待修改的 64 位有符号数
 * @param k 目标比特位的索引（0 起始，即右数第 k 位）
 * @returns 修改后的数值
 */
std::uint64_t setKthBit(std ::int64_t N, std ::int64_t k) {
    int pos = 1 << k;  // 构造只有第 k 位为 1 的掩码
    return N | pos;    // 通过按位或操作设置该位为 1
}
}  // namespace set_kth_bit
}  // namespace bit_manipulation

/**
 * @brief 单元自测用例
 */
static void test() {
    // 10 (二进制 1010)，设置第 2 位 -> 14 (二进制 1110)
    assert(bit_manipulation::set_kth_bit::setKthBit(10, 2) == 14);
    // 25 (二进制 11001)，设置第 1 位 -> 27 (二进制 11011)
    assert(bit_manipulation::set_kth_bit::setKthBit(25, 1) == 27);
    // 400001 (二进制 1100001101010000001)，设置第 5 位 -> 400033
    assert(bit_manipulation::set_kth_bit::setKthBit(400001, 5) == 400033);
    // 123 (二进制 1111011)，设置第 3 位（本来就是1） -> 123
    assert(bit_manipulation::set_kth_bit::setKthBit(123, 3) == 123);

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
