/**
 * @file
 * @brief Returns the [Hamming distance](https://en.wikipedia.org/wiki/Hamming_distance) between two integers (计算汉明距离)
 *
 * @details
 * 汉明距离（Hamming Distance）表示两个（相同长度）数据对应位不同的数量。
 * 1. 对于两个整数，可以通过计算它们的异或值 `a ^ b`，异或结果中为 1 的位即表示该二进制位不同。统计异或值中 1 的个数即为汉明距离。
 * 2. 对于两个等长的字符串，汉明距离等于对应字符不相同的数量。
 *
 * @author [Ravishankar Joshi](https://github.com/ravibitsgoa)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>   /// 用于 std::uint64_t
#include <iostream>  /// 用于输入输出
#include <string>    /// 用于 std::string

/**
 * @namespace bit_manipulation
 * @brief 位运算算法命名空间
 */
namespace bit_manipulation {
/**
 * @namespace hamming_distance
 * @brief 汉明距离计算相关命名空间
 */
namespace hamming_distance {
/**
 * @brief 计算数字二进制表示中 1 的个数（逐位移动法）
 * @param value 输入数值
 * @returns 1 的个数统计值
 */
uint64_t bitCount(uint64_t value) {
    uint64_t count = 0;
    while (value) {       // 循环直到所有位都为 0
        if (value & 1) {  // 检查最低位是否为 1
            count++;
        }
        value >>= 1;  // 右移 1 位，清除最低位
    }
    return count;
}

/**
 * @brief 计算两个整数之间的汉明距离
 * @param a 整数一
 * @param b 整数二
 * @returns 两个整数二进制表示中不同位的个数
 */
uint64_t hamming_distance(uint64_t a, uint64_t b) { return bitCount(a ^ b); }

/**
 * @brief 计算两个等长字符串之间的汉明距离
 * @param a 字符串一
 * @param b 字符串二
 * @returns 两个字符串中字符不相同的个数
 */
uint64_t hamming_distance(const std::string& a, const std::string& b) {
    assert(a.size() == b.size()); // 汉明距离要求两个字符串长度必须相等
    size_t n = a.size();
    uint64_t count = 0;
    for (size_t i = 0; i < n; i++) {
        count += (b[i] != a[i]); // 统计字符不同处的总个数
    }
    return count;
}
}  // namespace hamming_distance
}  // namespace bit_manipulation

/**
 * @brief 单元自测用例
 */
static void test() {
    // 整数汉明距离测试
    assert(bit_manipulation::hamming_distance::hamming_distance(11, 2) == 2);
    assert(bit_manipulation::hamming_distance::hamming_distance(2, 0) == 1);
    assert(bit_manipulation::hamming_distance::hamming_distance(11, 0) == 3);

    // 字符串汉明距离测试
    assert(bit_manipulation::hamming_distance::hamming_distance("1101", "1111") == 1);
    assert(bit_manipulation::hamming_distance::hamming_distance("1111", "1111") == 0);
    assert(bit_manipulation::hamming_distance::hamming_distance("0000", "1111") == 4);

    assert(bit_manipulation::hamming_distance::hamming_distance("alpha", "alphb") == 1);
    assert(bit_manipulation::hamming_distance::hamming_distance("abcd", "abcd") == 0);
    assert(bit_manipulation::hamming_distance::hamming_distance("dcba", "abcd") == 4);
    
    std::cout << "All test cases successfully passed!" << std::endl;
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();           // 运行自测
    uint64_t a = 11;  // 二进制 1011
    uint64_t b = 2;   // 二进制 0010

    std::cout << "Hamming distance between " << a << " and " << b << " is "
              << bit_manipulation::hamming_distance::hamming_distance(a, b)
              << std::endl;
    return 0;
}
