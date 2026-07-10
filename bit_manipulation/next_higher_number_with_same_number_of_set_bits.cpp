/**
 * @file
 * @brief [Next higher number with same number of set bits](https://www.geeksforgeeks.org/next-higher-number-with-same-number-of-set-bits/) (求下一个含有相同位 '1' 个数的最小大数)
 * @details
 * 给定一个正整数 x，寻找在二进制表示中含有相同数量的 '1' 且比 x 大的下一个最小正整数。
 * 例如：x = 12（二进制为 1100，含有两个 1）。比 12 大且同样包含两个 1 的最小整数是 17（二进制为 10001）。
 * 
 * ### 高斯帕黑客算法 (Gosper's Hack) 步骤解析：
 * 假设 x = 12 (二进制 1100):
 * 1. `rightOne = x & -x` 获取最低位的 1。即 `1100 & 0100 = 0100` (值为 4)。
 * 2. `nextHigherOneBit = x + rightOne` 在最低位的 1 上做加法，这会使得最低连续的一段 1 进位。
 *    即 `1100 + 0100 = 10000` (16)。它把 x 的左边部分和进位高位置为 1。
 * 3. `rightOnesPattern = x ^ nextHigherOneBit` 找出改变的位。
 *    即 `1100 ^ 10000 = 11100`。
 * 4. `rightOnesPattern = rightOnesPattern / rightOne` 将这些改变的位右移，对齐到最右侧。
 *    即 `11100 / 0100 = 00111` (7)。
 * 5. `rightOnesPattern >>= 2` 调整修正，除以 4。因为进位产生了一个多余的 1 且多移动了两位。
 *    即 `00111 >> 2 = 00001` (1)。
 * 6. `next = nextHigherOneBit | rightOnesPattern` 合并两部分，得到结果。
 *    即 `10000 | 00001 = 10001` (17)。
 * 
 * @author [Kunal Nayak](https://github.com/Kunal766)
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
 * @brief 使用 Gosper's Hack 算法计算下一个包含相同个数 1 的最小大数
 * @param x 输入数值
 * @returns 目标大数值
 */
uint64_t next_higher_number(uint64_t x) {
    uint64_t rightOne = 0;
    uint64_t nextHigherOneBit = 0;
    uint64_t rightOnesPattern = 0;
    uint64_t next = 0;

    if (x) {
        // 1. 获取最低位的 1
        rightOne = x & -static_cast<signed>(x);

        // 2. 最低连续的 1 进位，重置低位并设置下一个更高的位
        nextHigherOneBit = x + rightOne;

        // 3. 找出所有发生了改变的二进制位
        rightOnesPattern = x ^ nextHigherOneBit;

        // 4. 右对齐这些改变的二进制位
        rightOnesPattern = (rightOnesPattern) / rightOne;

        // 5. 修正因子：右移两位
        rightOnesPattern >>= 2;

        // 6. 将进位后的高位与右对齐后的低位合并
        next = nextHigherOneBit | rightOnesPattern;
    }

    return next;
}

}  // namespace bit_manipulation

/**
 * @brief 单元自测用例
 */
static void test() {
    // 4  (100)  -> 8 (1000)
    assert(bit_manipulation::next_higher_number(4) == 8);
    // 6  (110)  -> 9 (1001)
    assert(bit_manipulation::next_higher_number(6) == 9);
    // 13 (1101) -> 14 (1110)
    assert(bit_manipulation::next_higher_number(13) == 14);
    // 64 (1000000) -> 128 (10000000)
    assert(bit_manipulation::next_higher_number(64) == 128);
    // 15 (1111) -> 23 (10111)
    assert(bit_manipulation::next_higher_number(15) == 23);
    // 32 -> 64
    assert(bit_manipulation::next_higher_number(32) == 64);
    // 97 -> 98
    assert(bit_manipulation::next_higher_number(97) == 98);
    // 1024 -> 2048
    assert(bit_manipulation::next_higher_number(1024) == 2048);

    std::cout << "All test cases have successfully passed!" << std::endl;
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行自测
    return 0;
}
