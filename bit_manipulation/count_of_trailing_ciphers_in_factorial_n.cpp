/**
 * @file
 * @brief [Count the number of ciphers in n! implementation](https://www.tutorialspoint.com/count-trailing-zeros-in-factorial-of-a-number-in-cplusplus) (计算 n! 末尾的 0 的个数)
 * @details
 * 输入一个整数 n，寻找 n!（n 的阶乘）计算结果的末尾有多少个连续的 0。
 *
 * ### 勒让德定理 (Legendre's Formula) 数学原理
 * 1. 阶乘末尾的 0 是由质因数 2 和 5 相乘（2 * 5 = 10）得到的。
 * 2. 在任意数的阶乘 n! 的质因数分解中，因数 2 的出现次数必然远多于因数 5。
 * 3. 因此，末尾的 0 的个数完全取决于因数 5 的出现次数。
 * 4. 因数 5 的个数可以通过勒让德定理公式求得：
 *    Count = floor(n / 5) + floor(n / 25) + floor(n / 125) + ...
 *    循环累加商，直至 5 的幂次方大于 n 即可。
 * 
 * @author [Swastika Gupta](https://github.com/Swastyy)
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
 * @namespace count_of_trailing_ciphers_in_factorial_n
 * @brief 计算阶乘末尾 0 个数的相关命名空间
 */
namespace count_of_trailing_ciphers_in_factorial_n {
/**
 * @brief 利用勒让德定理计算 n! 的尾随 0 的个数
 * @param n 阶乘数
 * @return 连续的 0 的总个数
 */
uint64_t numberOfCiphersInFactorialN(uint64_t n) {
    uint64_t count = 0; // 统计 5 的质因数总个数

    // 循环遍历 5, 25, 125... 计算 n/i 的整数商并累加
    for (uint64_t i = 5; n / i >= 1; i *= 5) {
        count += n / i;
    }

    return count;
}
}  // namespace count_of_trailing_ciphers_in_factorial_n
}  // namespace bit_manipulation

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1：395! 末尾应有 97 个 0
    std::cout << "1st test ";
    assert(bit_manipulation::count_of_trailing_ciphers_in_factorial_n::
               numberOfCiphersInFactorialN(395) == 97);
    std::cout << "passed" << std::endl;

    // 测试 2：977! 末尾应有 242 个 0
    std::cout << "2nd test ";
    assert(bit_manipulation::count_of_trailing_ciphers_in_factorial_n::
               numberOfCiphersInFactorialN(977) == 242);
    std::cout << "passed" << std::endl;

    // 测试 3：871! 末尾应有 215 个 0
    std::cout << "3rd test ";
    assert(bit_manipulation::count_of_trailing_ciphers_in_factorial_n::
               numberOfCiphersInFactorialN(871) == 215);
    std::cout << "passed" << std::endl;

    // 测试 4：239! 末尾应有 57 个 0
    std::cout << "4th test ";
    assert(bit_manipulation::count_of_trailing_ciphers_in_factorial_n::
               numberOfCiphersInFactorialN(239) == 57);
    std::cout << "passed" << std::endl;

    // 测试 5：0! 末尾有 0 个 0
    std::cout << "5th test ";
    assert(bit_manipulation::count_of_trailing_ciphers_in_factorial_n::
               numberOfCiphersInFactorialN(0) == 0);
    std::cout << "passed" << std::endl;
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行自测
    return 0;
}
