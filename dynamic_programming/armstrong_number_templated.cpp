/**
 * @file
 * @brief Checks whether a number is an [Armstrong Number](https://en.wikipedia.org/wiki/Narcissistic_number) or not (检查阿姆斯特朗数/水仙花数)
 *
 * @details
 * 阿姆斯特朗数（Armstrong Number，亦称水仙花数、自恋数）：
 * 一个 n 位非负整数，其各位数字的 n 次方和等于该数本身。
 * 例如：153 是一个 3 位数，且 153 = 1^3 + 5^3 + 3^3，因此它是阿姆斯特朗数。
 *
 * 常见的阿姆斯特朗数包括：0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 153, 370, 371, 407, 1634...
 * 
 * @note
 * 提示：该文件虽被分类在 `dynamic_programming` 命名空间下，但其逻辑仅为基本的算术迭代，未涉及动态规划的记忆化或递推优化。
 *
 * @author [Shivam Singhal](https://github.com/shivhek25)
 * @author [David Leal](https://github.com/Panquesito7)
 */

#include <cassert>   /// 用于 assert 断言
#include <cmath>     /// 用于 std::pow 指数幂计算
#include <iostream>  /// 用于输入输出

/**
 * @namespace dynamic_programming
 * @brief 动态规划算法命名空间
 */
namespace dynamic_programming {

/**
 * @brief 检查一个数是否为阿姆斯特朗数
 * @tparam T 数值类型（支持各类整型）
 * @param number 待检查的数值
 * @returns `true` 是阿姆斯特朗数；`false` 否则
 */
template <typename T>
bool is_armstrong(const T &number) {
    int count = 0, temp = number, result = 0, rem = 0;

    // 第一步：计算数字的位数 (例如 153 为 3 位)
    while (temp != 0) {
        temp /= 10;
        count++;
    }

    // 第二步：累加每个数字的 count 次幂
    temp = number;
    while (temp != 0) {
        rem = temp % 10;
        result += static_cast<T>(std::pow(rem, count));
        temp /= 10;
    }

    // 第三步：判断累加结果是否等于原数
    return result == number;
}
}  // namespace dynamic_programming

/**
 * @brief 单元自测用例
 */
static void tests() {
    assert(dynamic_programming::is_armstrong(153) == true);
    assert(dynamic_programming::is_armstrong(1) == true);
    assert(dynamic_programming::is_armstrong(0) == true);
    assert(dynamic_programming::is_armstrong(370) == true);
    assert(dynamic_programming::is_armstrong(1634) == true);
    assert(dynamic_programming::is_armstrong(580) == false);
    assert(dynamic_programming::is_armstrong(15) == false);
    assert(dynamic_programming::is_armstrong(1024) == false);
    assert(dynamic_programming::is_armstrong(989) == false);
    assert(dynamic_programming::is_armstrong(103) == false);

    std::cout << "All tests have successfully passed!\n";
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    tests();  // 运行自测
    return 0;
}
