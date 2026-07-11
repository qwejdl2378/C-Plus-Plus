/**
 * @file
 * @brief Test suite for [LU decomposition](https://en.wikipedia.org/wiki/LU_decomposition) of a square matrix (矩阵 LU 分解及行列式计算测试)
 * @author [Krishna Vedala](https://github.com/kvedala)
 */
#include <cassert>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <cmath>

#include "./lu_decomposition.h"

/**
 * @brief 重载流输出运算符以打印矩阵
 */
template <typename T>
std::ostream &operator<<(std::ostream &out, matrix<T> const &v) {
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
 * @brief 测试随机方阵的 LU 分解
 */
void test1() {
    int mat_size = 3;  
    const int range = 50;
    const int range2 = range >> 1;

    // 核心修复：使用固定的随机种子，保证每次运行生成的测试方阵一致且可重复
    std::srand(42);

    matrix<double> A(mat_size);
    matrix<double> L(mat_size);  
    matrix<double> U(mat_size);  
    for (int i = 0; i < mat_size; i++) {
        A[i] = std::valarray<double>(mat_size);
        L[i] = std::valarray<double>(mat_size);
        U[i] = std::valarray<double>(mat_size);
        for (int j = 0; j < mat_size; j++) {
            A[i][j] = static_cast<double>(std::rand() % range - range2);
        }
    }

    std::clock_t start_t = std::clock();
    int res = lu_decomposition(A, &L, &U);
    std::clock_t end_t = std::clock();
    
    std::cout << "Time taken: "
              << static_cast<double>(end_t - start_t) / CLOCKS_PER_SEC << "\n";

    std::cout << "A = \n" << A << "\n";
    if (res == 0) {
        std::cout << "L = \n" << L << "\n";
        std::cout << "U = \n" << U << "\n";
        
        // 自测校验：L * U 应当重新组合为 A
        matrix<double> LU(mat_size);
        for (int i = 0; i < mat_size; ++i) {
            LU[i] = std::valarray<double>(mat_size);
            for (int j = 0; j < mat_size; ++j) {
                double val = 0.0;
                for (int k = 0; k < mat_size; ++k) {
                    val += L[i][k] * U[k][j];
                }
                LU[i][j] = val;
                assert(std::abs(LU[i][j] - A[i][j]) < 1e-9);
            }
        }
        std::cout << "LU decomposition verification passed!\n";
    } else {
        std::cout << "Matrix is singular, cannot decompose.\n";
    }
}

/**
 * @brief 测试利用 LU 分解求行列式的正确性
 */
void test2() {
    std::cout << "Determinant test 1...";
    matrix<int> A1({{1, 2, 3}, {4, 9, 6}, {7, 8, 9}});
    assert(std::abs(determinant_lu(A1) - (-48.0)) < 1e-9);
    std::cout << "passed\n";

    std::cout << "Determinant test 2...";
    matrix<int> A2({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}});
    assert(std::abs(determinant_lu(A2) - 0.0) < 1e-9);
    std::cout << "passed\n";

    std::cout << "Determinant test 3...";
    matrix<float> A3({{1.2f, 2.3f, 3.4f}, {4.5f, 5.6f, 6.7f}, {7.8f, 8.9f, 9.0f}});
    assert(std::abs(determinant_lu(A3) - 3.63) < 1e-2); // 浮点数精度放宽
    std::cout << "passed\n";
}

/**
 * @brief 主函数
 */
int main() {
    test1();
    test2();
    return 0;
}
