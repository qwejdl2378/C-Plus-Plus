/**
 * @file
 * @brief [Binomial distribution](https://en.wikipedia.org/wiki/Binomial_distribution) calculations (二项分布计算实现)
 *
 * @details
 * 二项分布是 $n$ 个独立的伯努利试验中成功的次数的离散概率分布，其中每次试验的成功概率为 $p$。
 *
 * ### 概率质量函数 (PMF)
 * $P(X = x) = \binom{n}{x} p^x (1-p)^{n-x}$
 * 期望值: $\mu = n \cdot p$
 * 方差: $\sigma^2 = n \cdot p \cdot (1-p)$
 *
 * 时间复杂度: $O(n)$ 用于计算组合数
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【极值与边界除零 Bug 审计与修复】：
 * 1. **选择次数为 0 (r = 0) 时的除零与未定义行为 Bug**：在原 `nCr` 函数中，
 *    如果 $r=0$，分母 `denominator` 初始化为 0，这会导致直接返回 `numerator / 0`，即触发除以零异常。
 *    **修复**：添加标准组合数边界条件：当 $r=0$ 或 $r=n$ 时直接返回 $1.0$；当 $r > n$ 时返回 $0.0$。
 * 2. **浮点型参数用作循环控制变量下溢与死循环风险**：原 `nCr` 接收 `double` 参数并将其隐式转换为 `int` 控制循环。
 *    如果参数是极小小数，会导致非预期的循环条件。
 *    **修复**：参数统一改为合法的整数类型（如 `uint32_t`），防止转换误差或死循环。
 * 3. **输入概率 p 必须在 [0, 1] 范围内**：添加安全断言以防计算结果超出概率区间。
 */

#include <cassert>
#include <cmath>
#include <iostream>

namespace probability {
namespace binomial {

/**
 * @brief 计算组合数 nCr
 * @param n 总个数
 * @param r 选择的个数
 * @return 组合数值
 */
double nCr(uint32_t n, uint32_t r) {
    if (r > n) return 0.0;
    if (r == 0 || r == n) return 1.0;
    if (r > n / 2) r = n - r; // 利用对称性优化计算，防止中间结果溢出

    double result = 1.0;
    for (uint32_t i = 1; i <= r; ++i) {
        result *= (n - r + i);
        result /= i;
    }
    return result;
}

/**
 * @brief 计算二项分布的期望值
 */
double binomial_expected(uint32_t n, double p) {
    assert(p >= 0.0 && p <= 1.0 && "Probability p must be in [0, 1]");
    return n * p;
}

/**
 * @brief 计算二项分布的方差
 */
double binomial_variance(uint32_t n, double p) {
    assert(p >= 0.0 && p <= 1.0 && "Probability p must be in [0, 1]");
    return n * p * (1.0 - p);
}

/**
 * @brief 计算二项分布的标准差
 */
double binomial_standard_deviation(uint32_t n, double p) {
    return std::sqrt(binomial_variance(n, p));
}

/**
 * @brief 计算刚好获得 x 次成功的概率 PMF
 */
double binomial_x_successes(uint32_t n, double p, uint32_t x) {
    assert(p >= 0.0 && p <= 1.0 && "Probability p must be in [0, 1]");
    if (x > n) return 0.0;
    return nCr(n, x) * std::pow(p, x) * std::pow(1.0 - p, n - x);
}

/**
 * @brief 计算在 [lower_bound, upper_bound] 范围内的成功次数概率之和 (CDF 区间)
 */
double binomial_range_successes(uint32_t n, double p, uint32_t lower_bound,
                                uint32_t upper_bound) {
    assert(p >= 0.0 && p <= 1.0 && "Probability p must be in [0, 1]");
    if (lower_bound > upper_bound || lower_bound > n) return 0.0;
    if (upper_bound > n) upper_bound = n;

    double probability = 0.0;
    for (uint32_t i = lower_bound; i <= upper_bound; ++i) {
        probability += binomial_x_successes(n, p, i);
    }
    return probability;
}

}  // namespace binomial
}  // namespace probability

/**
 * @brief 主函数
 */
int main() {
    uint32_t n = 100;
    double p = 0.5;

    std::cout << "expected value : " << probability::binomial::binomial_expected(n, p)
              << std::endl;
    std::cout << "variance : " << probability::binomial::binomial_variance(n, p) << std::endl;
    std::cout << "standard deviation : "
              << probability::binomial::binomial_standard_deviation(n, p) << std::endl;
    std::cout << "exactly 30 successes : " << probability::binomial::binomial_x_successes(n, p, 30)
              << std::endl;
    std::cout << "45 or more successes : "
              << probability::binomial::binomial_range_successes(n, p, 45, 100) << std::endl;

    // 基础单元验证
    assert(probability::binomial::nCr(5, 0) == 1.0);
    assert(probability::binomial::nCr(5, 5) == 1.0);
    assert(probability::binomial::nCr(5, 2) == 10.0);
    assert(probability::binomial::binomial_x_successes(5, 0.5, 0) == 0.03125);
    std::cout << "All binomial distribution tests passed successfully!" << std::endl;

    return 0;
}
