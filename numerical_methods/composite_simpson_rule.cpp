/**
 * @file
 * @brief Approximation of definite integrals using [Composite Simpson's Rule](https://en.wikipedia.org/wiki/Simpson%27s_rule#Composite_Simpson's_rule) (复合辛普森公式求积实现)
 *
 * @details
 * 复合辛普森法则是一种用于近似计算定积分 $\int_{a}^{b} f(x) dx$ 的数值积分方法。
 * 它通过将积分区间 $[a, b]$ 划分为 $N$ 个等宽的子区间（其中 $N$ 必须为偶数），每个子区间的宽度为 $h = \frac{b-a}{N}$，
 * 然后在每对相邻子区间上利用二次多项式进行插值逼近。
 *
 * ### 积分评估公式
 * $I \approx \frac{h}{3} \left[ f(x_0) + 4\sum_{i=1,3,\ldots}^{N-1} f(x_i) + 2\sum_{i=2,4,\ldots}^{N-2} f(x_i) + f(x_N) \right]$
 * 简言之，边界点系数为 1，奇数索引点系数为 4，偶数索引点系数为 2。
 *
 * 时间复杂度: $O(N)$
 * 空间复杂度: $O(N)$（可进一步优化至 $O(1)$，为保持直观此处保留数组存储）
 *
 * @note
 * 【区间偶数条件与容器低效 Bug 审计与修复】：
 * 1. **子区间数非偶数校验缺失 Bug**：复合辛普森法则的数学原理要求区间数 $N$ 必须为偶数。
 *    若传入奇数 $N$，公式计算会失效。原程序中缺少对 $N$ 的偶数防卫。
 *    **修复**：在入口处加入 `assert(N % 2 == 0 && "N must be an even number!")` 校验。
 * 2. **数据存储容器极其低效且重复引入头文件**：
 *    - 原程序引入了两次 `#include <cmath>`，且使用 `std::map` 存储点对数据，导致频繁进行动态树分配与 $O(\log N)$ 查找。
 *    - **修复**：移除多余头文件，将 `std::map` 重构为高效且内存连续的 `std::vector`，提供 $O(1)$ 常数级随机访问。
 *
 * @author [ggkogkou](https://github.com/ggkogkou)
 */

#include <cassert>  
#include <cmath>    
#include <cstdint>     
#include <cstdlib>     
#include <functional>  
#include <iostream>    
#include <vector>      

namespace numerical_methods {
namespace simpson_method {

/**
 * @brief 使用复合辛普森法则计算定积分的值
 * @param N 划分的子区间数，必须为偶数且大于 0
 * @param h 步长宽度 (b-a)/N
 * @param a 积分区间左端点 x0
 * @param func 待求积的目标函数
 * @return 积分近似值
 */
double evaluate_by_simpson(std::int32_t N, double h, double a,
                           const std::function<double(double)>& func) {
    // 核心修复：辛普森法则必须要求区间数 N 为正偶数
    assert(N > 0 && N % 2 == 0 && "N must be a positive even number!");

    // 核心修复：重构为 std::vector 以替代低效的 std::map，提供 O(1) 查找速度
    std::vector<double> data_table(N + 1);  
    double xi = a;   

    // 计算并生成节点数据表
    for (std::int32_t i = 0; i <= N; i++) {
        data_table[i] = func(xi);
        xi += h;  
    }

    // 依辛普森系数累加积分贡献：f(x0) + 4*f(x1) + 2*f(x2) + ... + f(xN)
    double evaluate_integral = 0.0;
    for (std::int32_t i = 0; i <= N; i++) {
        if (i == 0 || i == N) {
            evaluate_integral += data_table[i];
        } else if (i % 2 == 1) {
            evaluate_integral += 4.0 * data_table[i];
        } else {
            evaluate_integral += 2.0 * data_table[i];
        }
    }

    // 乘以步长系数 h/3
    evaluate_integral *= h / 3.0;

    // 定积分无法计算的防卫（如区间内包含奇异点导致 NaN）
    assert(!std::isnan(evaluate_integral) &&
           "The definite integral can't be evaluated. Check the validity of your input.\n");
           
    return evaluate_integral;
}

/** 各种自测用的目标测试函数 */
double f(double x) { return std::sqrt(x) + std::log(x); }
double g(double x) { return std::exp(-x) * (4.0 - std::pow(x, 2)); }
double k(double x) { return std::sqrt(2.0 * std::pow(x, 3) + 3.0); }
double l(double x) { return x + std::log(2.0 * x + 1.0); }

}  // namespace simpson_method
}  // namespace numerical_methods

/**
 * @brief 单元自测用例
 */
static void test(std::int32_t N, double h, double a, double b,
                 bool used_argv_parameters) {
    double result_f = numerical_methods::simpson_method::evaluate_by_simpson(
        N, h, a, numerical_methods::simpson_method::f);
    assert((used_argv_parameters || (result_f >= 4.09 && result_f <= 4.10)) &&
           "The result of f(x) is wrong");
    std::cout << "The result of integral f(x) on interval [" << a << ", " << b
              << "] is equal to: " << result_f << std::endl;

    double result_g = numerical_methods::simpson_method::evaluate_by_simpson(
        N, h, a, numerical_methods::simpson_method::g);
    assert((used_argv_parameters || (result_g >= 0.27 && result_g <= 0.28)) &&
           "The result of g(x) is wrong");
    std::cout << "The result of integral g(x) on interval [" << a << ", " << b
              << "] is equal to: " << result_g << std::endl;

    double result_k = numerical_methods::simpson_method::evaluate_by_simpson(
        N, h, a, numerical_methods::simpson_method::k);
    assert((used_argv_parameters || (result_k >= 9.06 && result_k <= 9.07)) &&
           "The result of k(x) is wrong");
    std::cout << "The result of integral k(x) on interval [" << a << ", " << b
              << "] is equal to: " << result_k << std::endl;

    double result_l = numerical_methods::simpson_method::evaluate_by_simpson(
        N, h, a, numerical_methods::simpson_method::l);
    assert((used_argv_parameters || (result_l >= 7.16 && result_l <= 7.17)) &&
           "The result of l(x) is wrong");
    std::cout << "The result of integral l(x) on interval [" << a << ", " << b
              << "] is equal to: " << result_l << std::endl;
}

/**
 * @brief 主函数
 */
int main(int argc, char** argv) {
    std::int32_t N = 16;  
    double a = 1.0, b = 3.0;  
    double h = NAN;       

    bool used_argv_parameters = false;  

    if (argc == 4) {
        N = std::atoi(argv[1]);
        a = std::atof(argv[2]);
        b = std::atof(argv[3]);
        assert(a < b && "a has to be less than b");
        assert(N > 0 && N % 2 == 0 && "N has to be a positive even number!");
        if (N != 16 || a != 1.0 || b != 3.0) {
            used_argv_parameters = true;
        }
        std::cout << "You selected N=" << N << ", a=" << a << ", b=" << b
                  << std::endl;
    } else {
        std::cout << "Default N=" << N << ", a=" << a << ", b=" << b
                  << std::endl;
    }

    h = (b - a) / N;

    test(N, h, a, b, used_argv_parameters);  

    return 0;
}
