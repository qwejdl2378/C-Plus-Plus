/**
 * @file
 * @brief Library functions to compute [QR decomposition](https://en.wikipedia.org/wiki/QR_decomposition) of a given matrix (矩阵 QR 分解算法的辅助函数库)
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#ifndef NUMERICAL_METHODS_QR_DECOMPOSE_H_
#define NUMERICAL_METHODS_QR_DECOMPOSE_H_

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <valarray>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace qr_algorithm {

/**
 * @brief 重载流输出运算符以方便打印二维矩阵
 */
template <typename T>
std::ostream &operator<<(std::ostream &out,
                         std::valarray<std::valarray<T>> const &v) {
    const int width = 12;
    const char separator = ' ';

    out.precision(4);
    for (size_t row = 0; row < v.size(); row++) {
        for (size_t col = 0; col < v[row].size(); col++) {
            out << std::right << std::setw(width) << std::setfill(separator)
                << v[row][col];
        }
        out << std::endl;
    }

    return out;
}

/**
 * @brief 重载流输出运算符以方便打印一维向量
 */
template <typename T>
std::ostream &operator<<(std::ostream &out, std::valarray<T> const &v) {
    const int width = 10;
    const char separator = ' ';

    out.precision(4);
    for (size_t row = 0; row < v.size(); row++) {
        out << std::right << std::setw(width) << std::setfill(separator)
            << v[row];
    }
    return out;
}

/**
 * @brief 计算两个一维向量的内积 (Dot Product)
 */
template <typename T>
inline double vector_dot(const std::valarray<T> &a, const std::valarray<T> &b) {
    return (a * b).sum();
}

/**
 * @brief 计算向量的模长 (Magnitude)
 */
template <typename T>
inline double vector_mag(const std::valarray<T> &a) {
    double dot = vector_dot(a, a);
    return std::sqrt(dot);
}

/**
 * @brief 计算向量 a 在向量 b 上的投影向量 (Projection Vector)
 */
template <typename T>
std::valarray<T> vector_proj(const std::valarray<T> &a,
                             const std::valarray<T> &b) {
    double num = vector_dot(a, b);
    double deno = vector_dot(b, b);

    // 引入机器 $\epsilon$ 防卫除以零异常
    if (deno <= std::numeric_limits<double>::epsilon()) {
        std::cerr << "[" << __func__ << "] Possible division by zero\n";
        return a;  
    }

    double scalar = num / deno;
    return b * scalar;
}

/**
 * @brief 使用施密特正交化方法对矩阵 A 进行 QR 分解 (A = Q * R)
 * @details
 * 将 $M \times N$ 的实矩阵 A 分解为正交矩阵 Q（$M \times N$）和上三角矩阵 R（$N \times N$）。
 *
 * @param[in] A 待分解的目标矩阵
 * @param[out] Q 分解得到的正交矩阵 Q
 * @param[out] R 分解得到的上三角矩阵 R
 *
 * @note
 * 【模长为 0 除零崩溃 Bug 审计与修复】：
 * 在施密特正交化迭代中，每一列投影计算完成后需要做单位化：
 * `Q[j][i] = tmp_vector[j] / mag`
 * 如果矩阵 $A$ 包含线性相关的列向量，经过正交投影后可能退化为 0 向量，此时 `mag` 极度接近 0。
 * 直接除以 `mag` 会造成严重的除零浮点数异常。
 * **修复**：对计算出的模长进行防卫校验。若 `mag < 1e-15`，则将 `mag` 保护性赋值为 1.0，避免除零并维持计算稳定性。
 */
template <typename T>
void qr_decompose(
    const std::valarray<std::valarray<T>> &A, 
    std::valarray<std::valarray<T>> *Q,       
    std::valarray<std::valarray<T>> *R        
) {
    std::size_t ROWS = A.size();        
    std::size_t COLUMNS = A[0].size();  
    std::valarray<T> col_vector(ROWS);
    std::valarray<T> col_vector2(ROWS);
    std::valarray<T> tmp_vector(ROWS);

    for (size_t i = 0; i < COLUMNS; i++) {
        R[0][i] = 0.0; 

#ifdef _OPENMP
#pragma omp for
#endif
        for (size_t j = 0; j < ROWS; j++) {
            tmp_vector[j] = A[j][i]; 
            col_vector[j] = A[j][i];
        }
        for (size_t j = 0; j < i; j++) {
            for (size_t k = 0; k < ROWS; k++) {
                col_vector2[k] = Q[0][k][j];
            }
            col_vector2 = vector_proj(col_vector, col_vector2);
            tmp_vector -= col_vector2;
        }

        double mag = vector_mag(tmp_vector);

        // 核心修复：防止列向量线性相关导致模长为 0 触发的除零崩溃
        if (mag < 1e-15) {
            mag = 1.0; 
        }

#ifdef _OPENMP
#pragma omp for
#endif
        for (size_t j = 0; j < ROWS; j++) {
            Q[0][j][i] = tmp_vector[j] / mag;
        }

#ifdef _OPENMP
#pragma omp for
#endif
        for (size_t kk = 0; kk < ROWS; kk++) {
            col_vector[kk] = Q[0][kk][i];
        }

#ifdef _OPENMP
#pragma omp for
#endif
        for (size_t k = i; k < COLUMNS; k++) {
            for (size_t kk = 0; kk < ROWS; kk++) {
                col_vector2[kk] = A[kk][k];
            }
            R[0][i][k] = (col_vector * col_vector2).sum();
        }
    }
}

}  // namespace qr_algorithm

#endif  // NUMERICAL_METHODS_QR_DECOMPOSE_H_
