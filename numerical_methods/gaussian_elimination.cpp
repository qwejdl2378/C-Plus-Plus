/**
 * @file
 * @brief Solve linear equation systems using [Gaussian Elimination with Partial Pivoting](https://en.wikipedia.org/wiki/Gaussian_elimination) (带部分列主元消去法的高斯消元法实现)
 *
 * @details
 * 高斯消元法是线性代数中求解线性方程组 $Ax = b$ 的经典数值算法。
 * 为了提高数值稳定性并防止除零异常，本实现引入了**部分列主元消去法**（Partial Pivoting）：
 * 1. 在第 $k$ 步消元前，在第 $k$ 列的第 $k$ 行及以下的行中，找出绝对值最大的元素作为主元。
 * 2. 交换当前行与主元所在的行。
 * 3. 执行消元，使主元下方的元素全化为 0，从而形成上三角矩阵。
 * 4. 进行回代（Back Substitution）计算出未知数解向量。
 *
 * 时间复杂度: $O(N^3)$，其中 $N$ 是未知数的个数（矩阵阶数）。
 * 空间复杂度: $O(N^2)$
 *
 * @note
 * 【除零崩溃、无主元与交互挂起 Bug 审计与修复】：
 * 1. **未行交换的主元除零崩溃 Bug**：原实现缺少列主元交换逻辑，一旦遇到对角线项 `mat[step][step] == 0`，
 *    消元比例 `a = mat[i+1][step] / mat[step][step]` 会直接触发除零崩溃或产生 `inf/nan`。
 *    对于病态矩阵或对角线为 0 的合法可解方程组，原程序会直接算错或挂掉。
 *    **修复**：加入列主元选择与行交换逻辑。在主元接近 0（低于精度阈值）时，判定矩阵奇异并抛出 `std::runtime_error` 异常。
 * 2. **手动管理二维动态数组极易泄漏**：原实现手动管理 `double**` 动态数组，极易因异常发生指针泄漏。
 *    **修复**：重构为 `std::vector<std::vector<double>>` 自动管理生命周期。
 * 3. **自动化测试交互挂起 Bug**：移除 `cin` 阻塞，改用 assert 自动对比解向量以通过流水线。
 */

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace numerical_methods {
namespace gaussian_elimination {

/**
 * @brief 使用列主元高斯消元法求解 Ax = b
 * @param mat 增广矩阵，大小为 N * (N + 1)
 * @return 解向量 x，大小为 N
 */
std::vector<double> solve(std::vector<std::vector<double>> mat) {
    size_t n = mat.size();
    if (n == 0) return {};
    
    // 校验增广矩阵维度是否为 N * (N + 1)
    for (size_t i = 0; i < n; ++i) {
        assert(mat[i].size() == n + 1 && "Matrix must be augmented (N * (N + 1))!");
    }

    // 1. 消元过程 (Forward Elimination)
    for (size_t step = 0; step < n; ++step) {
        // 核心修复：选择当前列中绝对值最大的行作为主元行 (Partial Pivoting)
        size_t max_row = step;
        for (size_t i = step + 1; i < n; ++i) {
            if (std::abs(mat[i][step]) > std::abs(mat[max_row][step])) {
                max_row = i;
            }
        }

        // 奇异矩阵校验（若主元绝对值接近 0，说明无唯一解或无解）
        if (std::abs(mat[max_row][step]) < 1e-9) {
            throw std::runtime_error("The matrix is singular or near-singular. No unique solution exists!");
        }

        // 交换当前行与最大主元行
        if (max_row != step) {
            std::swap(mat[step], mat[max_row]);
        }

        // 执行消元
        for (size_t i = step + 1; i < n; ++i) {
            double factor = mat[i][step] / mat[step][step];
            for (size_t j = step; j <= n; ++j) {
                mat[i][j] -= factor * mat[step][j];
            }
        }
    }

    // 2. 回代过程 (Back Substitution)
    std::vector<double> x(n, 0.0);
    for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
        double sum = 0.0;
        for (size_t j = static_cast<size_t>(i) + 1; j < n; ++j) {
            sum += mat[i][j] * x[j];
        }
        x[i] = (mat[i][n] - sum) / mat[i][i];
    }

    return x;
}

}  // namespace gaussian_elimination
}  // namespace numerical_methods

/**
 * @brief 单元自测用例
 */
static void test() {
    using namespace numerical_methods::gaussian_elimination;

    // 求解方程组:
    //  2x + y - z = 8
    // -3x - y + 2z = -11
    // -2x + y + 2z = -3
    // 增广矩阵如下：
    std::vector<std::vector<double>> mat1 = {
        {2.0, 1.0, -1.0, 8.0},
        {-3.0, -1.0, 2.0, -11.0},
        {-2.0, 1.0, 2.0, -3.0}
    };

    std::vector<double> sol1 = solve(mat1);

    // 预期解为: x = 2, y = 3, z = -1
    std::vector<double> expected1 = {2.0, 3.0, -1.0};
    constexpr double epsilon = 1e-6;

    assert(sol1.size() == expected1.size());
    for (size_t i = 0; i < sol1.size(); ++i) {
        assert(std::abs(sol1[i] - expected1[i]) < epsilon);
    }

    // 测试主元本身为 0 的情况（若无行交换会崩溃）
    //  0x + y = 2
    //  x  + y = 3
    std::vector<std::vector<double>> mat2 = {
        {0.0, 1.0, 2.0},
        {1.0, 1.0, 3.0}
    };
    std::vector<double> sol2 = solve(mat2);
    std::vector<double> expected2 = {1.0, 2.0};
    assert(sol2.size() == expected2.size());
    for (size_t i = 0; i < sol2.size(); ++i) {
        assert(std::abs(sol2[i] - expected2[i]) < epsilon);
    }

    std::cout << "All Gaussian elimination tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行自动化单元自测
    return 0;
}
