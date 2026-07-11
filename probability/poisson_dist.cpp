/**
 * @file
 * @brief [Poisson Distribution](https://en.wikipedia.org/wiki/Poisson_distribution) calculations (泊松分布计算实现)
 *
 * @details
 * 泊松分布是一种离散概率分布，适用于建模在特定时间或空间区间内独立随机事件发生的次数。
 *
 * ### 概率质量函数 (PMF)
 * $P(X = x) = \frac{\lambda^x e^{-\lambda}}{x!}$
 * - $\lambda$ ($\mu$)：特定区间内事件发生的期望次数（泊松参数，必须满足 $\lambda > 0$）。
 * - $x$：实际发生的事件数（非负整数 $x \in \{0, 1, 2, \ldots\}$）。
 *
 * 时间复杂度: $O(x)$ 用于阶乘计算
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【类型隐式转换与阶乘零边界 Bug 审计与修复】：
 * 1. **除零与负数输入安全校验**：在 `poisson_rate` 中如果传入时间范围为 0，会触发除零崩溃。
 *    同时，率参数 $\lambda$ 不允许为负数或 0。
 *    **修复**：添加 `assert(timeframe > 0.0)` 及 `assert(expected > 0.0)` 限制输入。
 * 2. **浮点型试验次数参数退化**：原程序中 `fact` 和 `poisson_x_successes` 中的 $x$ 被定义为 `double` 类型。
 *    泊松试验次数只能为整数，接收浮点数会导致精度丢失、阶乘计算循环死锁及数学逻辑退化。
 *    **修复**：将事件数量 $x$ 以及上下界参数重构为 `uint32_t` 整数类型，并对 0 的阶乘提供显式加速分支。
 */

#include <cassert>
#include <cmath>
#include <iostream>

namespace probability {
namespace poisson {

/**
 * @brief 计算泊松事件发生率
 * @param events 事件发生的次数
 * @param timeframe 发生事件的时间范围，必须大于 0
 * @return 发生率
 */
double poisson_rate(double events, double timeframe) {
    // 核心修复：添加除以零安全验证
    assert(timeframe > 0.0 && "Timeframe must be strictly positive!");
    assert(events >= 0.0 && "Events count cannot be negative!");
    return events / timeframe;
}

/**
 * @brief 计算在给定的时间段内的事件期望发生次数 lambda
 * @param rate 发生率
 * @param time 时间跨度
 * @return 期望发生次数 (lambda)
 */
double poisson_expected(double rate, double time) {
    assert(rate >= 0.0 && "Rate cannot be negative!");
    assert(time >= 0.0 && "Time duration cannot be negative!");
    return rate * time;
}

/**
 * @brief 计算非负整数的阶乘
 * @param x 待计算阶乘的整数
 * @return 阶乘结果实数
 */
double fact(uint32_t x) {
    if (x == 0 || x == 1) {
        return 1.0;
    }
    double x_fact = 1.0;
    for (uint32_t i = 2; i <= x; ++i) {
        x_fact *= i;
    }
    return x_fact;
}

/**
 * @brief 计算在期望发生次数为 expected 时，发生刚好 x 次事件的概率
 * @param expected 期望发生次数 lambda (必须大于 0)
 * @param x 实际发生的次数
 * @return 概率值
 */
double poisson_x_successes(double expected, uint32_t x) {
    // 核心修复：防止输入非正的期望参数导致数学公式失效
    assert(expected > 0.0 && "Expected rate lambda must be strictly positive!");
    return (std::pow(expected, x) * std::exp(-expected)) / fact(x);
}

/**
 * @brief 计算发生事件次数在区间 [lower, upper] 之间的累积概率之和
 */
double poisson_range_successes(double expected, uint32_t lower, uint32_t upper) {
    assert(expected > 0.0 && "Expected rate lambda must be strictly positive!");
    if (lower > upper) return 0.0;

    double probability = 0.0;
    for (uint32_t i = lower; i <= upper; ++i) {
        probability += poisson_x_successes(expected, i);
    }
    return probability;
}

}  // namespace poisson
}  // namespace probability

/**
 * @brief 主函数
 */
int main() {
    double rate = probability::poisson::poisson_rate(3.0, 1.0);
    std::cout << "Poisson rate : " << rate << std::endl;

    double expected = probability::poisson::poisson_expected(rate, 2.0);
    std::cout << "Poisson expected : " << expected << std::endl;

    double prob0 = probability::poisson::poisson_x_successes(expected, 0);
    double prob_range = probability::poisson::poisson_range_successes(expected, 0, 8);

    std::cout << "Poisson 0 successes : " << prob0 << std::endl;
    std::cout << "Poisson 0-8 successes : " << prob_range << std::endl;

    // 单元自测验证
    assert(std::abs(prob0 - 0.00247875) < 1e-6);
    assert(std::abs(prob_range - 0.847237) < 1e-5);
    std::cout << "All poisson distribution tests passed successfully!" << std::endl;

    return 0;
}
