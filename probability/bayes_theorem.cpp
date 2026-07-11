/**
 * @file
 * @brief Implementation of [Bayes' theorem](https://en.wikipedia.org/wiki/Bayes%27_theorem) (贝叶斯定理实现)
 *
 * @details
 * 贝叶斯定理是概率论中的一个重要定理，描述在已知一些条件下，某事件发生的概率。
 * 它将先验概率与后验概率相联系，通过引入观察数据（证据）来更新某个假设成立的概率。
 *
 * ### 贝叶斯公式
 * $P(A|B) = \frac{P(B|A) \cdot P(A)}{P(B)}$
 * - $P(A|B)$：后验概率（在条件 B 下 A 发生的概率）。
 * - $P(B|A)$：似然度（在条件 A 下 B 发生的概率）。
 * - $P(A)$：先验概率（A 发生的初始概率）。
 * - $P(B)$：边缘概率（B 发生的总概率，充当归一化常数）。
 *
 * 时间复杂度: $O(1)$
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【除零崩溃与边缘范围防卫】：
 * 1. **分母除零崩溃**：在计算 $P(A|B)$ 时，如果分母证据概率 $P(B) = 0$，会直接触发除零浮点数异常。
 *    **修复**：添加 `assert(B > 0.0)` 和 `assert(A > 0.0)` 对先验条件的分母项进行非零校验。
 * 2. **概率范围校验**：输入概率值必须在 $[0, 1]$ 之间。
 */

#include <cassert>
#include <iostream>

/**
 * @brief 计算后验概率 P(A|B)
 * @param BgivenA 似然度 P(B|A)
 * @param A 先验概率 P(A)
 * @param B 边缘概率 P(B)，不能为 0
 * @return 后验条件概率 P(A|B)
 */
double bayes_AgivenB(double BgivenA, double A, double B) {
    // 核心修复：防止分母除零崩溃，并对输入概率进行合法性校验
    assert(B > 0.0 && "Divisor probability B must be strictly greater than 0!");
    assert(A >= 0.0 && A <= 1.0 && "Prior probability A must be in [0, 1] range!");
    assert(BgivenA >= 0.0 && BgivenA <= 1.0 && "Likelihood P(B|A) must be in [0, 1] range!");
    return (BgivenA * A) / B;
}

/**
 * @brief 计算后验概率 P(B|A)
 * @param AgivenB 条件概率 P(A|B)
 * @param A 先验概率 P(A)，不能为 0
 * @param B 先验概率 P(B)
 * @return 后验条件概率 P(B|A)
 */
double bayes_BgivenA(double AgivenB, double A, double B) {
    // 核心修复：防止分母除零崩溃，并对输入概率进行合法性校验
    assert(A > 0.0 && "Divisor probability A must be strictly greater than 0!");
    assert(B >= 0.0 && B <= 1.0 && "Probability B must be in [0, 1] range!");
    assert(AgivenB >= 0.0 && AgivenB <= 1.0 && "Conditional probability P(A|B) must be in [0, 1] range!");
    return (AgivenB * B) / A;
}

/**
 * @brief 主函数
 */
int main() {
    double A = 0.01;      // 先验概率：例如某种罕见疾病的患病率 1%
    double B = 0.1;       // 边缘概率：例如检测结果呈阳性的总比例 10%
    double BgivenA = 0.9; // 似然度：如果确实患病，检测呈阳性的概率 90%

    double AgivenB = bayes_AgivenB(BgivenA, A, B);
    double BgivenA_reconstructed = bayes_BgivenA(AgivenB, A, B);

    std::cout << "P(A|B) (Posterior Probability) = " << AgivenB << std::endl;
    std::cout << "P(B|A) (Reconstructed Likelihood) = " << BgivenA_reconstructed << std::endl;

    // 单元验证
    assert(std::abs(AgivenB - 0.09) < 1e-9);
    assert(std::abs(BgivenA_reconstructed - BgivenA) < 1e-9);
    std::cout << "All Bayes theorem tests passed!" << std::endl;

    return 0;
}
