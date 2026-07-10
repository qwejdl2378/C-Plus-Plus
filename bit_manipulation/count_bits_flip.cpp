/**
 * @file
 * @brief Implementation to [Count number of bits to be flipped to convert A to B] (计算转换数字 A 到 B 所需翻转的比特位数)
 *
 * @details
 * 给定两个数字 A 和 B。我们的任务是计算需要翻转多少个比特位才能将 A 转换成 B。
 *
 * ### 算法原理
 * 1. 异或运算：`A ^ B`。异或操作会使 A 和 B 中数值不同的位在结果中置为 1，相同的位在结果中置为 0。
 * 2. 统计 1 的个数：使用布赖恩·柯尼汉（Brian Kernighan）算法，即 `A = A & (A - 1)` 来清除最低位的 1，直到 A 变零，统计一共清除的次数。
 *    该方法只迭代“1”的个数次，时间复杂度为 O(k)，最坏时间复杂度为 O(log n)。
 *
 * 最坏时间复杂度: O(log n)
 * 空间复杂度: O(1)
 * 
 * @author [Yash Raj Singh](https://github.com/yashrajyash)
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
 * @namespace count_bits_flip
 * @brief 翻转比特数计算相关命名空间
 */
namespace count_bits_flip {
/**
 * @brief 计算转换 A 到 B 需要翻转的比特位数
 * @param A 输入整数一
 * @param B 目标整数二
 * @returns 需要翻转的比特位总数
 */
std::uint64_t countBitsFlip(std::int64_t A, std::int64_t B) {
    int count = 0;  // 记录翻转次数
    
    A = A ^ B;      // 异或操作：找出不相同的位
    while (A) {
        A = A & (A - 1); // 快速消除最低位的 1
        count++;
    }
    return count;
}
}  // namespace count_bits_flip
}  // namespace bit_manipulation

/**
 * @brief 单元自测用例
 */
static void test() {
    // A = 10, B = 20 -> 对应 4 个不同位
    assert(bit_manipulation::count_bits_flip::countBitsFlip(10, 20) == 4);
    // A = 20, B = 25 -> 对应 3 个不同位
    assert(bit_manipulation::count_bits_flip::countBitsFlip(20, 25) == 3);
    // A = 7, B = 10 -> 对应 3 个不同位
    assert(bit_manipulation::count_bits_flip::countBitsFlip(7, 10) == 3);
    // A = 17, B = 25 -> 对应 1 个不同位
    assert(bit_manipulation::count_bits_flip::countBitsFlip(17, 25) == 1);
    // A = 11, B = 8 -> 对应 2 个不同位
    assert(bit_manipulation::count_bits_flip::countBitsFlip(11, 8) == 2);
    // A = 21, B = 22 -> 对应 2 个不同位
    assert(bit_manipulation::count_bits_flip::countBitsFlip(21, 22) == 2);
    
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
