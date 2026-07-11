/**
 * @file
 * @brief Linear regression example using [Ordinary least squares](https://en.wikipedia.org/wiki/Ordinary_least_squares) (普通最小二乘法线性回归算法实现)
 *
 * @details
 * 最小二乘法（OLS）是一种在误差平方和最小的原则下，为给定数据点寻找最佳函数匹配的方法。
 * 在线性回归中，我们试图建立特征矩阵 $X$ 和目标值 $Y$ 之间的线性映射关系：
 * $Y = X \beta + \epsilon$
 * 其中 $\beta$ 为待求解的回归系数向量。
 *
 * OLS 的闭式解（Normal Equation）公式为：
 * $\beta = (X^T X)^{-1} X^T Y$
 *
 * 时间复杂度: $O(F^3 + N \cdot F^2)$，其中 $N$ 是样本数，$F$ 是特征维度（求逆矩阵的时间复杂度为 $O(F^3)$）。
 * 空间复杂度: $O(N \cdot F + F^2)$。
 *
 * @note
 * 【测试自动挂起挂起与空矩阵解引用极值 Bug 审计与修复】：
 * 1. **非交互环境下 cin 挂起阻塞 Bug**：在 `main` 阶段，
 *    原程序在完成单元测试后，会直接请求输入 `F`（特征数）和 `N`（样本数）。在自动化构建或流水线测试中，
 *    这会导致程序永久挂起挂起。
 *    **修复**：像其他算法一样引入交互模式开关，若未选择则默认跳过交互过程直接安全退出。
 * 2. **空矩阵解引用崩溃 Bug**：在 `get_transpose` 函数中，
 *    如果传入的特征矩阵 `A` 为空，执行 `A[0].size()` 会产生越界甚至空指针解引用崩溃。
 *    **修复**：在各矩阵运算前，强制添加 `A.empty() || A[0].empty()` 等维度判空逻辑并防御性返回。
 *
 * \author [Krishna Vedala](https://github.com/kvedala)
 */

#include <cassert>
#include <cmath>    
#include <iomanip>  
#include <iostream>
#include <vector>

/**
 * @brief 输出流运算符重载，用于美化打印矩阵
 */
template <typename T>
std::ostream &operator<<(std::ostream &out,
                         std::vector<std::vector<T>> const &v) {
    const int width = 10;
    const char separator = ' ';

    for (size_t row = 0; row < v.size(); row++) {
        for (size_t col = 0; col < v[row].size(); col++) {
            out << std::left << std::setw(width) << std::setfill(separator)
                << v[row][col];
        }
        out << std::endl;
    }

    return out;
}

/**
 * @brief 输出流运算符重载，用于美化打印一维向量
 */
template <typename T>
std::ostream &operator<<(std::ostream &out, std::vector<T> const &v) {
    const int width = 15;
    const char separator = ' ';

    for (size_t row = 0; row < v.size(); row++) {
        out << std::left << std::setw(width) << std::setfill(separator)
            << v[row];
    }

    return out;
}

/**
 * @brief 检查一个二维矩阵是否是方阵
 * @return true 是方阵, false 否
 */
template <typename T>
inline bool is_square(std::vector<std::vector<T>> const &A) {
    if (A.empty()) return false;
    size_t N = A.size();
    for (size_t i = 0; i < N; i++) {
        if (A[i].size() != N) {
            return false;
        }
    }
    return true;
}

/**
 * @brief 运算符重载：矩阵乘法 (A * B)
 */
template <typename T>
std::vector<std::vector<T>> operator*(std::vector<std::vector<T>> const &A,
                                      std::vector<std::vector<T>> const &B) {
    if (A.empty() || B.empty() || A[0].empty() || B[0].empty()) {
        return {};
    }
    size_t N_A = A.size();
    size_t N_B = B[0].size();

    std::vector<std::vector<T>> result(N_A);

    if (A[0].size() != B.size()) {
        std::cerr << "Number of columns in A != Number of rows in B ("
                  << A[0].size() << ", " << B.size() << ")" << std::endl;
        return result;
    }

    for (size_t row = 0; row < N_A; row++) {
        std::vector<T> v(N_B);
        for (size_t col = 0; col < N_B; col++) {
            v[col] = static_cast<T>(0);
            for (size_t j = 0; j < B.size(); j++) {
                v[col] += A[row][j] * B[j][col];
            }
        }
        result[row] = v;
    }

    return result;
}

/**
 * @brief 运算符重载：矩阵与一维向量乘法 (A * B)
 */
template <typename T>
std::vector<T> operator*(std::vector<std::vector<T>> const &A,
                         std::vector<T> const &B) {
    if (A.empty() || B.empty() || A[0].empty()) {
        return {};
    }
    size_t N_A = A.size();

    std::vector<T> result(N_A);

    if (A[0].size() != B.size()) {
        std::cerr << "Number of columns in A != Number of rows in B ("
                  << A[0].size() << ", " << B.size() << ")" << std::endl;
        return result;
    }

    for (size_t row = 0; row < N_A; row++) {
        result[row] = static_cast<T>(0);
        for (size_t j = 0; j < B.size(); j++) {
            result[row] += A[row][j] * B[j];
        }
    }

    return result;
}

/**
 * @brief 运算符重载：标量乘一维向量 (scalar * A)
 */
template <typename T>
std::vector<float> operator*(float const scalar, std::vector<T> const &A) {
    size_t N_A = A.size();
    std::vector<float> result(N_A);
    for (size_t row = 0; row < N_A; row++) {
        result[row] = A[row] * static_cast<float>(scalar);
    }
    return result;
}

/**
 * @brief 运算符重载：一维向量乘标量 (A * scalar)
 */
template <typename T>
std::vector<float> operator*(std::vector<T> const &A, float const scalar) {
    size_t N_A = A.size();
    std::vector<float> result(N_A);
    for (size_t row = 0; row < N_A; row++) {
        result[row] = A[row] * static_cast<float>(scalar);
    }
    return result;
}

/**
 * @brief 运算符重载：一维向量除以标量 (A / scalar)
 */
template <typename T>
std::vector<float> operator/(std::vector<T> const &A, float const scalar) {
    return (1.f / scalar) * A;
}

/**
 * @brief 运算符重载：向量减法 (A - B)
 */
template <typename T>
std::vector<T> operator-(std::vector<T> const &A, std::vector<T> const &B) {
    size_t N = A.size();
    std::vector<T> result(N);

    if (B.size() != N) {
        std::cerr << "Vector dimensions should be identical!" << std::endl;
        return A;
    }

    for (size_t row = 0; row < N; row++) {
        result[row] = A[row] - B[row];
    }

    return result;
}

/**
 * @brief 运算符重载：向量加法 (A + B)
 */
template <typename T>
std::vector<T> operator+(std::vector<T> const &A, std::vector<T> const &B) {
    size_t N = A.size();
    std::vector<T> result(N);

    if (B.size() != N) {
        std::cerr << "Vector dimensions should be identical!" << std::endl;
        return A;
    }

    for (size_t row = 0; row < N; row++) {
        result[row] = A[row] + B[row];
    }

    return result;
}

/**
 * @brief 使用高斯-约旦消元变换，求非奇异方阵的逆矩阵
 */
template <typename T>
std::vector<std::vector<float>> get_inverse(
    std::vector<std::vector<T>> const &A) {
    size_t N = A.size();

    std::vector<std::vector<float>> inverse(N);
    for (size_t row = 0; row < N; row++) {
        inverse[row] = std::vector<float>(N);
        for (size_t col = 0; col < N; col++) {
            inverse[row][col] = (row == col) ? 1.f : 0.f;
        }
    }

    if (!is_square(A)) {
        std::cerr << "A must be a square matrix!" << std::endl;
        return inverse;
    }

    std::vector<std::vector<float>> temp(N);
    for (size_t row = 0; row < N; row++) {
        std::vector<float> v(N);
        for (size_t col = 0; col < N; col++) {
            v[col] = static_cast<float>(A[row][col]);
        }
        temp[row] = v;
    }

    // 执行高斯消元初等行变换
    for (size_t row = 0; row < N; row++) {
        for (size_t row2 = row; row2 < N && temp[row][row] == 0; row2++) {
            temp[row] = temp[row] + temp[row2];
            inverse[row] = inverse[row] + inverse[row2];
        }

        for (size_t col2 = row; col2 < N && temp[row][row] == 0; col2++) {
            for (size_t row2 = 0; row2 < N; row2++) {
                temp[row2][row] = temp[row2][row] + temp[row2][col2];
                inverse[row2][row] = inverse[row2][row] + inverse[row2][col2];
            }
        }

        if (temp[row][row] == 0) {
            std::cerr << "Low-rank matrix, no inverse!" << std::endl;
            return inverse;
        }

        auto divisor = static_cast<float>(temp[row][row]);
        temp[row] = temp[row] / divisor;
        inverse[row] = inverse[row] / divisor;

        for (size_t row2 = 0; row2 < N; row2++) {
            if (row2 == row) {
                continue;
            }
            float factor = temp[row2][row];
            temp[row2] = temp[row2] - factor * temp[row];
            inverse[row2] = inverse[row2] - factor * inverse[row];
        }
    }

    return inverse;
}

/**
 * @brief 二维矩阵转置
 */
template <typename T>
std::vector<std::vector<T>> get_transpose(
    std::vector<std::vector<T>> const &A) {
    // 核心修复：防卫性尺寸判空校验，规避 get_transpose 空指针崩溃
    if (A.empty() || A[0].empty()) {
        return {};
    }
    std::vector<std::vector<T>> result(A[0].size());

    for (size_t row = 0; row < A[0].size(); row++) {
        std::vector<T> v(A.size());
        for (size_t col = 0; col < A.size(); col++) {
            v[col] = A[col][row];
        }
        result[row] = v;
    }
    return result;
}

/**
 * @brief 执行最小二乘回归拟合，计算多项式回归系数
 * @param X 训练集样本特征矩阵
 * @param Y 训练集样本输出向量
 * @return 拟合出的模型系数（最后一个维度为常数偏差偏置项）
 */
template <typename T>
std::vector<float> fit_OLS_regressor(std::vector<std::vector<T>> const &X,
                                     std::vector<T> const &Y) {
    if (X.empty()) {
        return {};
    }
    std::vector<std::vector<T>> X2 = X;
    for (size_t i = 0; i < X2.size(); i++) {
        X2[i].push_back(1); // 加上全 1 列，用于求得偏置项常数
    }
    std::vector<std::vector<T>> Xt = get_transpose(X2);
    std::vector<std::vector<T>> tmp = get_inverse(Xt * X2);
    std::vector<std::vector<float>> out = tmp * Xt;

    return out * Y;
}

/**
 * @brief 预测新输入特征集对应的线性回归拟合值
 */
template <typename T>
std::vector<float> predict_OLS_regressor(std::vector<std::vector<T>> const &X,
                                          std::vector<float> const &beta) {
    if (X.empty() || beta.empty()) {
        return {};
    }
    std::vector<float> result(X.size());

    for (size_t rows = 0; rows < X.size(); rows++) {
        result[rows] = beta[X[0].size()]; // 提取偏置常量
        for (size_t cols = 0; cols < X[0].size(); cols++) {
            result[rows] += beta[cols] * X[rows][cols];
        }
    }
    return result;
}

/**
 * @brief 单元自测用例
 */
void ols_test() {
    /* 测试一：二次回归曲线拟合测试 x^2 - 5 */
    std::cout << "Test 1 (quadratic function)....";
    std::vector<std::vector<float>> data1(
        {{-5, 25, -125}, {-1, 1, -1}, {0, 0, 0}, {1, 1, 1}, {6, 36, 216}});
    std::vector<float> Y1({20, -4, -5, -4, 31});
    std::vector<float> beta1 = fit_OLS_regressor(data1, Y1);
    std::vector<std::vector<float>> test_data1(
        {{-2, 4, -8}, {2, 4, 8}, {-10, 100, -1000}, {10, 100, 1000}});
    std::vector<float> expected1({-1, -1, 95, 95});
    std::vector<float> out1 = predict_OLS_regressor(test_data1, beta1);
    for (size_t rows = 0; rows < out1.size(); rows++) {
        assert(std::abs(out1[rows] - expected1[rows]) < 0.01);
    }
    std::cout << "passed\n";

    /* 测试二：三次回归拟合测试 x^3 + x^2 - 100 */
    std::cout << "Test 2 (cubic function)....";
    std::vector<std::vector<float>> data2(
        {{-5, 25, -125}, {-1, 1, -1}, {0, 0, 0}, {1, 1, 1}, {6, 36, 216}});
    std::vector<float> Y2({-200, -100, -100, -98, 152});
    std::vector<float> beta2 = fit_OLS_regressor(data2, Y2);
    std::vector<std::vector<float>> test_data2(
        {{-2, 4, -8}, {2, 4, 8}, {-10, 100, -1000}, {10, 100, 1000}});
    std::vector<float> expected2({-104, -88, -1000, 1000});
    std::vector<float> out2 = predict_OLS_regressor(test_data2, beta2);
    for (size_t rows = 0; rows < out2.size(); rows++) {
        assert(std::abs(out2[rows] - expected2[rows]) < 0.01);
    }
    std::cout << "passed\n";

    std::cout << std::endl;  
}

/**
 * @brief 主函数
 */
int main() {
    ols_test();

    // 核心修复：引入自动化交互开关，杜绝自动化测试场景永久挂起
    std::cout << "Would you like to run interactive mode? (1 for Yes, 0 for No): ";
    int run_interactive = 0;
    if (!(std::cin >> run_interactive) || run_interactive == 0) {
        return 0;
    }

    size_t N = 0, F = 0;

    std::cout << "Enter number of features: ";
    std::cin >> F;
    std::cout << "Enter number of samples: ";
    std::cin >> N;

    std::vector<std::vector<float>> data(N);
    std::vector<float> Y(N);

    std::cout
        << "Enter training data. Per sample, provide features and one output."
        << std::endl;

    for (size_t rows = 0; rows < N; rows++) {
        std::vector<float> v(F);
        std::cout << "Sample# " << rows + 1 << ": ";
        for (size_t cols = 0; cols < F; cols++) {
            std::cin >> v[cols];
        }
        data[rows] = v;
        std::cin >> Y[rows];
    }

    std::vector<float> beta = fit_OLS_regressor(data, Y);
    std::cout << std::endl << std::endl << "beta:" << beta << std::endl;

    size_t T = 0;
    std::cout << "Enter number of test samples: ";
    std::cin >> T;
    std::vector<std::vector<float>> data2(T);

    for (size_t rows = 0; rows < T; rows++) {
        std::cout << "Sample# " << rows + 1 << ": ";
        std::vector<float> v(F);
        for (size_t cols = 0; cols < F; cols++) {
            std::cin >> v[cols];
        }
        data2[rows] = v;
    }

    std::vector<float> out = predict_OLS_regressor(data2, beta);
    for (size_t rows = 0; rows < T; rows++) {
        std::cout << out[rows] << std::endl;
    }

    return 0;
}
