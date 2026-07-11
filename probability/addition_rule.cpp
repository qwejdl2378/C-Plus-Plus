/**
 * @file
 * @brief Implementation of the [Addition Rule of Probabilities](https://en.wikipedia.org/wiki/Probability#Summary_of_probabilities) (概率加法公式实现)
 *
 * @details
 * 概率加法公式用于计算两个事件中至少有一个发生的概率，即求并集概率 $P(A \cup B)$ 或记为 $P(A \text{ or } B)$。
 *
 * ### 概率计算公式
 * 1. **独立事件 (Independent Events)**:
 *    两个事件的发生互不影响：
 *    $P(A \cup B) = P(A) + P(B) - P(A) \cdot P(B)$
 * 2. **相关事件 (Dependent Events)**:
 *    一个事件的发生会影响另一个事件发生的概率（引入条件概率）：
 *    $P(A \cup B) = P(A) + P(B) - P(A) \cdot P(B|A)$
 *
 * 时间复杂度: $O(1)$
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【概率值边界合理性防卫】：
 * 1. **概率值越界校验**：输入概率 $P(A)$ 或 $P(B)$ 必须严格落在实数区间 $[0, 1]$ 之间。
 *    **修复**：在输入参数阶段对 $A, B$ 及条件概率进行边界校验断言限制，避免传入负数或超过 1 的概率产生逻辑谬误。
 */

#include <cassert>
#include <iostream>

/**
 * @brief 计算独立事件的并集概率 P(A or B)
 * @param A 事件 A 发生的概率，范围 [0, 1]
 * @param B 事件 B 发生的概率，范围 [0, 1]
 * @return 独立事件 A 或 B 发生的概率
 */
double addition_rule_independent(double A, double B) {
    // 核心修复：防卫性校验输入概率值的合法性
    assert(A >= 0.0 && A <= 1.0 && "Probability A must be in [0, 1] range!");
    assert(B >= 0.0 && B <= 1.0 && "Probability B must be in [0, 1] range!");
    return (A + B) - (A * B);
}

/**
 * @brief 计算相关（依赖）事件的并集概率 P(A or B)
 * @param A 事件 A 发生的概率，范围 [0, 1]
 * @param B 事件 B 发生的概率，范围 [0, 1]
 * @param B_given_A 条件概率 P(B|A)，即在 A 发生的前提下 B 发生的概率，范围 [0, 1]
 * @return 相关事件 A 或 B 发生的概率
 */
double addition_rule_dependent(double A, double B, double B_given_A) {
    // 核心修复：防卫性校验输入概率及条件概率值的合法性
    assert(A >= 0.0 && A <= 1.0 && "Probability A must be in [0, 1] range!");
    assert(B >= 0.0 && B <= 1.0 && "Probability B must be in [0, 1] range!");
    assert(B_given_A >= 0.0 && B_given_A <= 1.0 && "Conditional probability P(B|A) must be in [0, 1] range!");
    return (A + B) - (A * B_given_A);
}

/**
 * @brief 主函数
 */
int main() {
    double A = 0.5;
    double B = 0.25;
    double B_given_A = 0.05;

    double p_ind = addition_rule_independent(A, B);
    double p_dep = addition_rule_dependent(A, B, B_given_A);

    std::cout << "independent P(A or B) = " << p_ind << std::endl;
    std::cout << "dependent P(A or B) = " << p_dep << std::endl;

    // 单元验证
    assert(p_ind == 0.625);
    assert(p_dep == 0.725);
    std::cout << "All addition rule tests passed!" << std::endl;

    return 0;
}
