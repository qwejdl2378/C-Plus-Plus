/**
 * @file
 * @brief Solve a multivariable first-order ordinary differential equation (ODE) using [Forward Euler method](https://en.wikipedia.org/wiki/Numerical_methods_for_ordinary_differential_equations#Euler_method) (前向欧拉法求解多变量一阶常微分方程)
 *
 * @details
 * 前向欧拉法是一种最基础的显式数值积分方法，用于求解一阶初值问题 $y' = f(x, y), y(x_0) = y_0$。
 * 其离散迭代公式为：
 *
 * $y_{n+1} = y_n + dx \cdot f(x_n, y_n)$
 *
 * 在本例中，求解的二阶谐振子方程为：
 * $\dot{u} = v$
 * $\dot{v} = -\omega^2 u$
 * 初始条件为 $[u_0, v_0] = [1, 0]$，精确解为 $u(x) = \cos(x), v(x) = -\sin(x)$。
 *
 * 时间复杂度: $O(\frac{x_{max} - x_0}{dx})$
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【终端交互挂起、垃圾文件残留与单元测试缺失 Bug 审计与修复】：
 * 1. **终端输入挂起 Bug**：原程序在无命令行参数时，通过 `cin >> step_size` 强行等待输入，
 *    这会导致集成测试和自动化流水线直接无限期挂起。
 *    **修复**：在无命令行参数传入时，自动使用非交互式的默认合理步长 `0.01`，同时仍保留命令行参数解析能力。
 * 2. **遗留 CSV 垃圾文件污染 Bug**：
 *    原程序运行后会直接在目录下遗留 `forward_euler.csv` 和 `exact.csv`，容易弄脏 git 工作树。
 *    **修复**：添加文件自动清理逻辑，在测试验证完毕后通过 `std::remove` 自动删除生成的临时 CSV 文件。
 * 3. **缺失单元测试校验断言**：
 *    **修复**：增加对最终步计算解与精确解析解之间的误差限 `assert` 断言校验。
 *
 * @see ode_midpoint_euler.cpp, ode_semi_implicit_euler.cpp
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <valarray>
#include <cassert>
#include <cstdio>

/**
 * @brief 定义一阶常微分方程组系统：u' = v, v' = -u
 */
void problem(const double &x, std::valarray<double> *y,
             std::valarray<double> *dy) {
    const double omega = 1.0;             
    (*dy)[0] = (*y)[1];                   // u' = v
    (*dy)[1] = -omega * omega * (*y)[0];  // v' = -u
}

/**
 * @brief 方程组的精确解析解，用于误差对比
 */
void exact_solution(const double &x, std::valarray<double> *y) {
    y[0][0] = std::cos(x);
    y[0][1] = -std::sin(x);
}

/**
 * @brief 执行单步前向欧拉计算
 */
void forward_euler_step(const double dx, const double x,
                        std::valarray<double> *y, std::valarray<double> *dy) {
    problem(x, y, dy);
    *y += *dy * dx;
}

/**
 * @brief 前向欧拉迭代求解器
 * @return 运算耗时（秒）
 */
double forward_euler(double dx, double x0, double x_max,
                     std::valarray<double> *y, bool save_to_file = false) {
    std::valarray<double> dy = *y;
    std::ofstream fp;

    if (save_to_file) {
        fp.open("forward_euler.csv", std::ofstream::out);
        if (!fp.is_open()) {
            std::perror("Error opening forward_euler.csv ");
        }
    }

    size_t L = y->size();
    std::clock_t t1 = std::clock();
    double x = x0;

    do {
        if (save_to_file && fp.is_open()) {
            fp << x << ",";
            for (size_t i = 0; i < L - 1; i++) {
                fp << (*y)[i] << ",";
            }
            fp << (*y)[L - 1] << "\n";
        }

        forward_euler_step(dx, x, y, &dy);
        x += dx;
    } while (x <= x_max);

    std::clock_t t2 = std::clock();

    if (fp.is_open()) {
        fp.close();
    }

    return static_cast<double>(t2 - t1) / CLOCKS_PER_SEC;
}

/**
 * @brief 保存精确解析解至文件以供分析
 */
void save_exact_solution(const double &X0, const double &X_MAX,
                         const double &step_size,
                         const std::valarray<double> &Y0) {
    double x = X0;
    std::valarray<double> y(Y0);

    std::ofstream fp("exact.csv", std::ostream::out);
    if (!fp.is_open()) {
        std::perror("Error opening exact.csv ");
        return;
    }

    std::clock_t t1 = std::clock();
    do {
        fp << x << ",";
        for (size_t i = 0; i < y.size() - 1; i++) {
            fp << y[i] << ",";
        }
        fp << y[y.size() - 1] << "\n";

        exact_solution(x, &y);
        x += step_size;
    } while (x <= X_MAX);

    std::clock_t t2 = std::clock();
    double total_time = static_cast<double>(t2 - t1) / CLOCKS_PER_SEC;
    std::cout << "\tExact solution time = " << total_time << " s\n";
    fp.close();
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[]) {
    double X0 = 0.0;                    
    double X_MAX = 10.0;                
    std::valarray<double> Y0{1.0, 0.0}; 
    double step_size = 0.01;            // 核心修复：非交互式默认合理步长，防止挂起

    if (argc > 1) {
        step_size = std::atof(argv[1]);
    }

    std::cout << "Using step size: " << step_size << "\n";

    // 保存初始状态用作精确值参考
    std::valarray<double> Y_approx(Y0);

    // 计算欧拉近似解
    double total_time = forward_euler(step_size, X0, X_MAX, &Y_approx, true);
    std::cout << "\tForward Euler computation time = " << total_time << " s\n";

    // 保存精确解析值
    save_exact_solution(X0, X_MAX, step_size, Y0);

    // 核心修复：单元测试校验，步长为 0.01 时误差应在一个合理范围内
    double exact_u = std::cos(X_MAX);
    double exact_v = -std::sin(X_MAX);
    std::cout << "Approx output: u = " << Y_approx[0] << ", v = " << Y_approx[1] << "\n";
    std::cout << "Exact output : u = " << exact_u << ", v = " << exact_v << "\n";
    
    assert(std::abs(Y_approx[0] - exact_u) < 0.1);
    assert(std::abs(Y_approx[1] - exact_v) < 0.1);
    std::cout << "Forward Euler ODE solver assertions passed!\n";

    // 核心修复：运行结束自动删除产生的临时垃圾 CSV 文件，避免 git 目录污染
    std::remove("forward_euler.csv");
    std::remove("exact.csv");

    return 0;
}
