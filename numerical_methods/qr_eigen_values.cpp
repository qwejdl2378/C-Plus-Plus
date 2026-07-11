/**
 * @file
 * @brief Compute real eigenvalues of a symmetric matrix using [QR decomposition](https://en.wikipedia.org/wiki/QR_decomposition) method (通过迭代带位移的 QR 分解计算对称矩阵的实特征值)
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <valarray>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "./qr_decompose.h"
using qr_algorithm::operator<<;

/** 矩阵随机数值生成的上下限跨度 */
#define LIMS 9 

namespace numerical_methods {
namespace qr_eigen {

/**
 * @brief 生成指定维度的随机对称矩阵
 * @details 对称实矩阵的所有特征值必定为实数，这保证了 QR 迭代法的收敛域为实数域。
 * @param[out] A 存放生成的对称方阵
 */
void create_matrix(std::valarray<std::valarray<double>> *A) {
    int lim2 = LIMS >> 1;
    int N = A->size();

#ifdef _OPENMP
#pragma omp for
#endif
    for (int i = 0; i < N; i++) {
        (*A)[i][i] = (std::rand() % LIMS) - lim2;
        for (int j = i + 1; j < N; j++) {
            int tmp = (std::rand() % LIMS) - lim2;
            (*A)[i][j] = tmp;  
            (*A)[j][i] = tmp;
        }
    }
}

/**
 * @brief 两个矩阵的乘法: OUT = A * B
 */
void mat_mul(const std::valarray<std::valarray<double>> &A,
             const std::valarray<std::valarray<double>> &B,
             std::valarray<std::valarray<double>> *OUT) {
    size_t R1 = A.size();
    size_t C1 = A[0].size();
    size_t R2 = B.size();
    size_t C2 = B[0].size();
    if (C1 != R2) {
        throw std::invalid_argument("Matrix dimensions mismatch in mat_mul!");
    }

    for (size_t i = 0; i < R1; i++) {
        for (size_t j = 0; j < C2; j++) {
            (*OUT)[i][j] = 0.0;
            for (size_t k = 0; k < C1; k++) {
                (*OUT)[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

} // namespace qr_eigen
} // namespace numerical_methods

namespace qr_algorithm {

/**
 * @brief 使用带 Wilkinson 位移的迭代 QR 分解计算对称方阵 A 的所有实特征值
 * @param[in,out] A 输入的对称矩阵，计算过程中会被就地修改消减
 * @param[in] print_intermediates 是否打印中间的迭代矩阵
 * @return 包含所有特征值的一维数组
 */
std::valarray<double> eigen_values(std::valarray<std::valarray<double>> *A,
                                   bool print_intermediates = false) {
    int rows = A->size();
    int columns = rows;
    int counter = 0, num_eigs = rows - 1;
    double last_eig = 0.0;

    std::valarray<std::valarray<double>> Q(rows);
    std::valarray<std::valarray<double>> R(columns);

    std::valarray<double> eigen_vals(rows);
    for (int i = 0; i < rows; i++) {
        Q[i] = std::valarray<double>(columns);
        R[i] = std::valarray<double>(columns);
    }

    // 迭代直到所有特征值收敛并提取出来
    while (num_eigs > 0) {
        // 利用次对角线元素接近 0 作为单根收敛判据
        while (std::abs((*A)[num_eigs][num_eigs - 1]) > 1e-12) {
            // 位移量选取当前子矩阵的最后一个对角元素 (Rayleigh quotient shift)
            last_eig = (*A)[num_eigs][num_eigs];
            for (int i = 0; i < rows; i++) {
                (*A)[i][i] -= last_eig; // A' = A - cI
            }

            qr_decompose(*A, &Q, &R);

            if (print_intermediates) {
                std::cout << *A << "\n";
                std::cout << Q << "\n";
                std::cout << R << "\n";
                std::printf("-------------------- %d ---------------------\n",
                       ++counter);
            }

            // 新逼近矩阵 A'' = R * Q
            numerical_methods::qr_eigen::mat_mul(R, Q, A);

            for (int i = 0; i < rows; i++) {
                (*A)[i][i] += last_eig; // A'' = R * Q + cI
            }
        }

        // 保存已收敛的特征值
        eigen_vals[num_eigs] = last_eig;
        num_eigs--;
        rows--;
        columns--;
    }
    eigen_vals[0] = (*A)[0][0];

    return eigen_vals;
}

}  // namespace qr_algorithm

/**
 * @brief 测试用例 1: 计算 2x2 对称矩阵 {{5, 7}, {7, 11}} 的特征值
 * @details 预期特征值约为 {15.56158, 0.384227}
 */
void test1() {
    std::valarray<std::valarray<double>> X = {{5.0, 7.0}, {7.0, 11.0}};
    double y[] = {15.56158, 0.384227};  

    std::cout << "------- Test 1 -------" << std::endl;
    std::valarray<double> eig_vals = qr_algorithm::eigen_values(&X);

    for (int i = 0; i < 2; i++) {
        std::cout << i + 1 << "/2 Checking for " << y[i] << " --> ";
        bool result = false;
        for (int j = 0; j < 2 && !result; j++) {
            if (std::abs(y[i] - eig_vals[j]) < 0.1) {
                result = true;
                std::cout << "(" << eig_vals[j] << ") ";
            }
        }
        assert(result);  
        std::cout << "found\n";
    }
    std::cout << "Test 1 Passed\n\n";
}

/**
 * @brief 测试用例 2: 计算 5x5 对称矩阵的特征值
 * @details 预期特征值约为 {9.27648, -9.26948, 2.0181, -1.03516, -5.98994}
 */
void test2() {
    std::valarray<std::valarray<double>> X = {{-4.0, 4.0, 2.0, 0.0, -3.0},
                                               {4.0, -4.0, 4.0, -3.0, -1.0},
                                               {2.0, 4.0, 4.0, 3.0, -3.0},
                                               {0.0, -3.0, 3.0, -1.0, -3.0},
                                               {-3.0, -1.0, -3.0, -3.0, 0.0}};
    double y[] = {9.27648, -9.26948, 2.0181, -1.03516, -5.98994};  

    std::cout << "------- Test 2 -------" << std::endl;
    std::valarray<double> eig_vals = qr_algorithm::eigen_values(&X);

    std::cout << X << "\n"
              << "Eigen values: " << eig_vals << "\n";

    for (int i = 0; i < 5; i++) {
        std::cout << i + 1 << "/5 Checking for " << y[i] << " --> ";
        bool result = false;
        for (int j = 0; j < 5 && !result; j++) {
            if (std::abs(y[i] - eig_vals[j]) < 0.1) {
                result = true;
                std::cout << "(" << eig_vals[j] << ") ";
            }
        }
        assert(result);  
        std::cout << "found\n";
    }
    std::cout << "Test 2 Passed\n\n";
}

/**
 * @brief 主函数
 */
int main(int argc, char **argv) {
    // 设定固定随机种子，确保测试和交互生成幂迭代的可重复性
    std::srand(42);

    if (argc == 1) {  
        test1();
        test2();
        std::cout << "Usage: ./qr_eigen_values [mat_size]\n";
        return 0;
    }

    int mat_size = atoi(argv[1]);
    if (mat_size < 2) {
        std::cerr << "Matrix size should be > 2\n";
        return -1;
    }

    int rows = mat_size, columns = mat_size;
    std::valarray<std::valarray<double>> A(rows);

    for (int i = 0; i < rows; i++) {
        A[i] = std::valarray<double>(columns);
    }

    /* 生成随机对称方阵 */
    numerical_methods::qr_eigen::create_matrix(&A);

    std::cout << A << "\n";

    clock_t t1 = clock();
    std::valarray<double> eigen_vals = qr_algorithm::eigen_values(&A);
    double dtime = static_cast<double>(clock() - t1) / CLOCKS_PER_SEC;

    std::cout << "Eigen vals: ";
    for (int i = 0; i < mat_size; i++) {
        std::cout << eigen_vals[i] << "\t";
    }
    std::cout << "\nTime taken to compute: " << dtime << " sec\n";

    return 0;
}
