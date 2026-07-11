/**
 * @file
 * @brief Implementation of splitting large integers into individual digits (大整数数位拆分算法实现)
 *
 * @details
 * 数位拆分（Digit Separation）是基础的算术操作。
 * 在贪心算法或数论算法中，经常需要对数字进行逐位分解处理（例如大数计算、水仙花数判定、数字特征提取等）。
 * 它体现了“局部拆解”的思想，每次通过除以 10 和取模 10 两个局部操作，逐步剥离出当前数位上的数值。
 *
 * ### 核心操作
 * 1. **逆序拆分 (digitSeparationReverseOrder)**：
 *    - 利用 `largeNumber % 10` 得到最低位的数字。
 *    - 使用 `std::abs` 保证负数也能提取出正的数位值。
 *    - 利用 `largeNumber /= 10` 舍弃最低位。
 *    - 循环直到数值变成 0。
 * 2. **顺序输出 (digitSeparationForwardOrder)**：
 *    - 先提取逆序数位，再通过 `std::reverse` 翻转容器得到正常的高位到低位的数位数组。
 *
 * 时间复杂度: $O(\log_{10} N)$ (数位个数与十进制对数成正比)
 * 空间复杂度: $O(\log_{10} N)$ (用于保存各数位的容器)
 * 
 * @author [Muhammad Junaid Khalid](https://github.com/mjk22071998)
 */

#include <algorithm>  /// 用于 std::reverse
#include <cassert>    /// 用于 assert 断言
#include <cmath>      /// 用于 std::abs
#include <cstdint>   /// 用于 std::int64_t
#include <iostream>   /// 用于标准输出
#include <vector>     /// 用于 std::vector

/**
 * @namespace greedy_algorithms
 * @brief 贪心算法命名空间
 */
namespace greedy_algorithms {

/**
 * @brief 数位拆分类
 */
class DigitSeparation {
 public:
    /**
     * @brief 默认构造函数
     */
    DigitSeparation() = default;

    /**
     * @brief 将大整数拆分为逆序（从低位到高位）的个位数数组
     * @param largeNumber 待拆分的 64 位大整数
     * @return 逆序排列的数位 vector
     */
    std::vector<std::int64_t> digitSeparationReverseOrder(
        std::int64_t largeNumber) const {
        std::vector<std::int64_t> result;
        
        if (largeNumber != 0) {
            while (largeNumber != 0) {
                // 取模 10 并求绝对值，确保负数提取出的数位也是正数 [0-9]
                // 注意：在 largeNumber 为 INT64_MIN 时，对求模结果取绝对值安全，因为 -8 取绝对值为 8
                result.push_back(std::abs(largeNumber % 10));
                largeNumber /= 10; // 去掉最低位
            }
        } else {
            result.push_back(0); // 边界处理：输入本身为 0 时返回 {0}
        }
        return result;
    }

    /**
     * @brief 将大整数拆分为顺序（从高位到低位）的个位数数组
     * @param largeNumber 待拆分的 64 位大整数
     * @return 正常顺序排列的数位 vector
     */
    std::vector<std::int64_t> digitSeparationForwardOrder(
        std::int64_t largeNumber) const {
        // 先获取逆序数位数组
        std::vector<std::int64_t> result =
            digitSeparationReverseOrder(largeNumber);
        // 翻转得到正常顺序
        std::reverse(result.begin(), result.end());
        return result;
    }
};

}  // namespace greedy_algorithms

/**
 * @brief 单元自测用例
 */
static void tests() {
    greedy_algorithms::DigitSeparation ds;

    // 测试 1：正整数
    std::int64_t number = 1234567890;
    std::vector<std::int64_t> expectedReverse = {0, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    std::vector<std::int64_t> expectedForward = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    std::vector<std::int64_t> reverseOrder = ds.digitSeparationReverseOrder(number);
    assert(reverseOrder == expectedReverse);
    std::vector<std::int64_t> forwardOrder = ds.digitSeparationForwardOrder(number);
    assert(forwardOrder == expectedForward);

    // 测试 2：个位数
    number = 5;
    expectedReverse = {5};
    expectedForward = {5};
    reverseOrder = ds.digitSeparationReverseOrder(number);
    assert(reverseOrder == expectedReverse);
    forwardOrder = ds.digitSeparationForwardOrder(number);
    assert(forwardOrder == expectedForward);

    // 测试 3：0
    number = 0;
    expectedReverse = {0};
    expectedForward = {0};
    reverseOrder = ds.digitSeparationReverseOrder(number);
    assert(reverseOrder == expectedReverse);
    forwardOrder = ds.digitSeparationForwardOrder(number);
    assert(forwardOrder == expectedForward);

    // 测试 4：超大整数
    number = 987654321012345;
    expectedReverse = {5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    expectedForward = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5};
    reverseOrder = ds.digitSeparationReverseOrder(number);
    assert(reverseOrder == expectedReverse);
    forwardOrder = ds.digitSeparationForwardOrder(number);
    assert(forwardOrder == expectedForward);

    // 测试 5：负整数
    number = -987654321012345;
    expectedReverse = {5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    expectedForward = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5};
    reverseOrder = ds.digitSeparationReverseOrder(number);
    assert(reverseOrder == expectedReverse);
    forwardOrder = ds.digitSeparationForwardOrder(number);
    assert(forwardOrder == expectedForward);

    std::cout << "DigitSeparation tests passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    tests(); // 运行测试
    return 0;
}
