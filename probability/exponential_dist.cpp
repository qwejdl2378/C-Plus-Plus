/**
 * @file
 * @brief [Exponential Distribution](https://en.wikipedia.org/wiki/Exponential_distribution) calculations (指数分布计算实现)
 *
 * @details
 * 指数分布是一种连续概率分布，常用于建模独立随机事件发生的时间间隔，例如放射性衰变、呼叫中心两次通话之间的时间等。
 *
 * ### 概率密度函数 (PDF)
 * $f(x; \lambda) = \lambda e^{-\lambda x}$ ($x \ge 0$)
 *
 * ### 累积分布函数 (CDF)
 * $F(x; \lambda) = 1 - e^{-\lambda x}$ ($x \ge 0$)
 *
 * 期望值: $\mu = \frac{1}{\lambda}$
 * 方差: $\sigma^2 = \frac{1}{\lambda^2}$
 *
 * 时间复杂度: $O(1)$
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【命名空间混淆与接口缺失 Bug 审计与修复】：
 * 1. **命名空间嵌套拷贝黏贴 Bug**：原实现错误地在 `exponential_dist.cpp` 内部定义了
 *    `namespace geometric_dist`（几何分布命名空间），导致调用和符号逻辑与几何分布混淆。
 *    **修复**：修改为匹配文件功能的 `namespace exponential_dist`。
 * 2. **缺少核心概率密度与累积分布函数**：作为概率分布实现文件，原程序只有期望和方差，缺乏真正的 PDF 和 CDF 实现。
 *    **修复**：新增 `exponential_pdf(x, lambda)` 和 `exponential_cdf(x, lambda)`，使本分布模型完整。
 * 3. **参数有效性校验**：对率参数 $\lambda$ 增加了必须大于 0 的校验及防卫（抛出 `std::invalid_argument`）。
 *
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <cassert>    
#include <cmath>      
#include <iostream>   
#include <stdexcept>  
#include <string>     

namespace probability {
/**
 * @namespace exponential_dist
 * @brief 指数分布相关函数
 */
namespace exponential_dist {

/**
 * @brief 计算指数分布的期望值
 * @param lambda 率参数 (Rate parameter), 必须大于 0
 * @return 期望值 1 / lambda
 */
double exponential_expected(double lambda) {
    if (lambda <= 0.0) {
        throw std::invalid_argument("lambda must be greater than 0");
    }
    return 1.0 / lambda;
}

/**
 * @brief 计算指数分布的方差
 * @param lambda 率参数, 必须大于 0
 * @return 方差 1 / (lambda^2)
 */
double exponential_var(double lambda) {
    if (lambda <= 0.0) {
        throw std::invalid_argument("lambda must be greater than 0");
    }
    return 1.0 / std::pow(lambda, 2);
}

/**
 * @brief 计算指数分布的标准差
 * @param lambda 率参数, 必须大于 0
 * @return 标准差 1 / lambda
 */
double exponential_std(double lambda) {
    if (lambda <= 0.0) {
        throw std::invalid_argument("lambda must be greater than 0");
    }
    return 1.0 / lambda;
}

/**
 * @brief 计算概率密度函数 PDF 值
 * @param x 自变量 (发生间隔时间)
 * @param lambda 率参数, 必须大于 0
 * @return 概率密度值
 */
double exponential_pdf(double x, double lambda) {
    if (lambda <= 0.0) {
        throw std::invalid_argument("lambda must be greater than 0");
    }
    if (x < 0.0) {
        return 0.0;
    }
    return lambda * std::exp(-lambda * x);
}

/**
 * @brief 计算累积分布函数 CDF 值
 * @param x 自变量
 * @param lambda 率参数, 必须大于 0
 * @return 累积概率值
 */
double exponential_cdf(double x, double lambda) {
    if (lambda <= 0.0) {
        throw std::invalid_argument("lambda must be greater than 0");
    }
    if (x < 0.0) {
        return 0.0;
    }
    return 1.0 - std::exp(-lambda * x);
}

}  // namespace exponential_dist
}  // namespace probability

/**
 * @brief 单元自测用例
 */
static void test() {
    double lambda_1 = 1.0;
    double expected_1 = 1.0;
    double var_1 = 1.0;
    double std_1 = 1.0;

    double lambda_2 = 2.0;
    double expected_2 = 0.5;
    double var_2 = 0.25;
    double std_2 = 0.5;

    double lambda_3 = 3.0;
    double expected_3 = 0.333333;
    double var_3 = 0.111111;
    double std_3 = 0.333333;

    double lambda_4 = 0.0;     
    double lambda_5 = -2.3;  

    const double threshold = 1e-5;

    std::cout << "Test for lambda = 1 \n";
    assert(std::abs(expected_1 - probability::exponential_dist::exponential_expected(lambda_1)) < threshold);
    assert(std::abs(var_1 - probability::exponential_dist::exponential_var(lambda_1)) < threshold);
    assert(std::abs(std_1 - probability::exponential_dist::exponential_std(lambda_1)) < threshold);
    assert(std::abs(0.367879 - probability::exponential_dist::exponential_pdf(1.0, lambda_1)) < 1e-4);
    assert(std::abs(0.632120 - probability::exponential_dist::exponential_cdf(1.0, lambda_1)) < 1e-4);
    std::cout << "ALL TEST PASSED\n\n";

    std::cout << "Test for lambda = 2 \n";
    assert(std::abs(expected_2 - probability::exponential_dist::exponential_expected(lambda_2)) < threshold);
    assert(std::abs(var_2 - probability::exponential_dist::exponential_var(lambda_2)) < threshold);
    assert(std::abs(std_2 - probability::exponential_dist::exponential_std(lambda_2)) < threshold);
    std::cout << "ALL TEST PASSED\n\n";

    std::cout << "Test for lambda = 3 \n";
    assert(std::abs(expected_3 - probability::exponential_dist::exponential_expected(lambda_3)) < threshold);
    assert(std::abs(var_3 - probability::exponential_dist::exponential_var(lambda_3)) < threshold);
    assert(std::abs(std_3 - probability::exponential_dist::exponential_std(lambda_3)) < threshold);
    std::cout << "ALL TEST PASSED\n\n";

    std::cout << "Test for lambda = 0 \n";
    try {
        probability::exponential_dist::exponential_expected(lambda_4);
        assert(false && "Should have thrown an exception!");
    } catch (const std::invalid_argument& err) {
        assert(std::string(err.what()) == "lambda must be greater than 0");
    }
    std::cout << "ALL TEST PASSED\n\n";

    std::cout << "Test for lambda = -2.3 \n";
    try {
        probability::exponential_dist::exponential_var(lambda_5);
        assert(false && "Should have thrown an exception!");
    } catch (const std::invalid_argument& err) {
        assert(std::string(err.what()) == "lambda must be greater than 0");
    }
    std::cout << "ALL TEST PASSED\n\n";
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
