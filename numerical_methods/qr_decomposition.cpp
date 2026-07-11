/**
 * @file
 * @brief Automated test and program for [QR decomposition](https://en.wikipedia.org/wiki/QR_decomposition) of a matrix (矩阵 QR 分解的主测试程序)
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <array>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cassert>
#include <valarray>

#include "./qr_decompose.h"

using qr_algorithm::qr_decompose;
using qr_algorithm::operator<<;

/**
 * @brief 对 QR 分解进行自动化测试验证
 * @details
 * 1. 验证 Q * R 是否重新恢复为原始矩阵 A。
 * 2. 验证 Q 矩阵是否正交，即 Q^T * Q = I。
 */
static void test() {
    std::cout << "Starting automated QR decomposition test...\n";

    size_t ROWS = 3;
    size_t COLUMNS = 3;

    std::valarray<std::valarray<double>> A = {
        std::valarray<double>({12.0, -51.0, 4.0}),
        std::valarray<double>({6.0, 167.0, -68.0}),
        std::valarray<double>({-4.0, 24.0, -41.0})
    };

    std::valarray<std::valarray<double>> Q(ROWS);
    std::valarray<std::valarray<double>> R(COLUMNS);
    for (size_t i = 0; i < ROWS; i++) {
        Q[i] = std::valarray<double>(COLUMNS);
    }
    for (size_t i = 0; i < COLUMNS; i++) {
        R[i] = std::valarray<double>(COLUMNS);
    }

    // 执行 QR 分解
    qr_decompose(A, &Q, &R);

    std::cout << "Original Matrix A:\n" << A << "\n";
    std::cout << "Orthogonal Matrix Q:\n" << Q << "\n";
    std::cout << "Upper Triangular Matrix R:\n" << R << "\n";

    constexpr double epsilon = 1e-9;

    // 1. 验证 Q * R == A
    for (size_t i = 0; i < ROWS; i++) {
        for (size_t j = 0; j < COLUMNS; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < COLUMNS; k++) {
                sum += Q[i][k] * R[k][j];
            }
            assert(std::abs(sum - A[i][j]) < epsilon && "Q * R must be equal to A!");
        }
    }

    // 2. 验证 Q^T * Q == I (单位矩阵)
    for (size_t i = 0; i < COLUMNS; i++) {
        for (size_t j = 0; j < COLUMNS; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < ROWS; k++) {
                sum += Q[k][i] * Q[k][j]; // Q^T 的第 i 行（Q 的第 i 列）与 Q 的第 j 列乘积
            }
            double expected = (i == j) ? 1.0 : 0.0;
            assert(std::abs(sum - expected) < epsilon && "Q must be orthogonal (Q^T * Q = I)!");
        }
    }

    std::cout << "QR decomposition automated test passed successfully!\n";
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[]) {
    // 核心修复：无参数时默认执行自动化单元测试，避免在流水线上因 std::cin 等待而挂起
    if (argc == 1) {
        test();
        return 0;
    }

    unsigned int ROWS = 0, COLUMNS = 0;
    std::cout << "Enter the number of rows and columns: ";
    if (!(std::cin >> ROWS >> COLUMNS)) {
        return 0;
    }

    std::cout << "Enter matrix elements row-wise:\n";

    std::valarray<std::valarray<double>> A(ROWS);
    std::valarray<std::valarray<double>> Q(ROWS);
    std::valarray<std::valarray<double>> R(COLUMNS);
    for (size_t i = 0; i < std::max(ROWS, COLUMNS); i++) {
        if (i < ROWS) {
            A[i] = std::valarray<double>(COLUMNS);
            Q[i] = std::valarray<double>(COLUMNS);
        }
        if (i < COLUMNS) {
            R[i] = std::valarray<double>(COLUMNS);
        }
    }

    for (unsigned int i = 0; i < ROWS; i++) {
        for (unsigned int j = 0; j < COLUMNS; j++) {
            std::cin >> A[i][j];
        }
    }

    std::cout << "Matrix A:\n" << A << "\n";

    clock_t t1 = clock();
    qr_decompose(A, &Q, &R);
    double dtime = static_cast<double>(clock() - t1) / CLOCKS_PER_SEC;

    std::cout << "Q:\n" << Q << "\n";
    std::cout << "R:\n" << R << "\n";
    std::cout << "Time taken to compute: " << dtime << " sec\n ";

    return 0;
}
