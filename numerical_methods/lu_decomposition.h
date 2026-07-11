/**
 * @file lu_decomposition.h
 * @author [Krishna Vedala](https://github.com/kvedala)
 * @brief Functions associated with [LU Decomposition](https://en.wikipedia.org/wiki/LU_decomposition) of a square matrix (矩阵的 LU 分解算法实现)
 */

#pragma once

#include <iostream>
#include <valarray>
#include <vector>
#include <cmath>

#ifdef _OPENMP
#include <omp.h>
#endif

/** 定义矩阵类型为 std::vector<std::valarray<T>> */
template <typename T>
using matrix = std::vector<std::valarray<T>>;

/**
 * @brief 对方阵 A 进行杜利特尔（Doolittle）LU 分解
 * @details
 * 将方阵 A 分解为一个下三角矩阵 L（对角线元素全为 1）和一个上三角矩阵 U，满足 A = L * U。
 *
 * @param[in] A 待分解的输入方阵
 * @param[out] L 分解出的下三角矩阵 L (对角线全为 1)
 * @param[out] U 分解出的上三角矩阵 U
 * @return 0 分解成功
 * @return -1 矩阵非方阵或遭遇零主元（无法进行无列主元的 LU 分解）
 *
 * @note
 * 【零主元除零崩溃 Bug 审计与修复】：
 * 在非列主元 LU 分解中，L 矩阵的元素计算涉及除以上三角矩阵的对角线元素：
 * `L[col][row] = (A[col][row] - lu_sum) / U[row][row]`
 * 如果遭遇 $U_{row, row} == 0$（例如矩阵 $A = \begin{pmatrix} 0 & 1 \\ 1 & 0 \end{pmatrix}$ ），
 * 直接计算会触发**严重的除零错误**，导致浮点异常或产生 `inf`/`nan`。
 * **修复**：在除法前加入主元零值检测，如果主元绝对值接近 0 且仍需消元，说明无法直接分解，提前返回 `-1` 报错。
 */
template <typename T>
int lu_decomposition(const matrix<T> &A, matrix<double> *L, matrix<double> *U) {
    int mat_size = A.size();

    if (mat_size == 0 || mat_size != static_cast<int>(A[0].size())) {
        std::cerr << "Not a square matrix!\n";
        return -1;
    }

    // 初始化 L 和 U 矩阵的维度与默认值
    for (int i = 0; i < mat_size; ++i) {
        (*L)[i] = std::valarray<double>(0.0, mat_size);
        (*U)[i] = std::valarray<double>(0.0, mat_size);
    }

    for (int row = 0; row < mat_size; row++) {
        // 计算上三角矩阵 U
#ifdef _OPENMP
#pragma omp for
#endif
        for (int col = row; col < mat_size; col++) {
            double lu_sum = 0.0;
            for (int j = 0; j < row; j++) {
                lu_sum += (*L)[row][j] * (*U)[j][col];
            }
            (*U)[row][col] = A[row][col] - lu_sum;
        }

        // 核心修复：检查当前步的主元是否为 0，防止接下来的除法发生崩溃
        if (std::abs((*U)[row][row]) < 1e-9) {
            // 如果已经是最后一行，不需要再作为分母除数进行消元，可以容忍 0 主元
            if (row < mat_size - 1) {
                return -1; 
            }
        }

        // 计算下三角矩阵 L
#ifdef _OPENMP
#pragma omp for
#endif
        for (int col = row; col < mat_size; col++) {
            if (row == col) {
                (*L)[row][col] = 1.0;
                continue;
            }

            double lu_sum = 0.0;
            for (int j = 0; j < row; j++) {
                lu_sum += (*L)[col][j] * (*U)[j][row];
            }
            (*L)[col][row] = (A[col][row] - lu_sum) / (*U)[row][row];
        }
    }

    return 0;
}

/**
 * @brief 通过 LU 分解计算方阵 A 的行列式值
 * @param A 输入方阵
 * @return 行列式结果，若分解失败则返回 0
 */
template <typename T>
double determinant_lu(const matrix<T> &A) {
    int n = A.size();
    matrix<double> L(n);
    matrix<double> U(n);

    if (lu_decomposition(A, &L, &U) < 0) {
        return 0.0;
    }

    double result = 1.0;
    for (int i = 0; i < n; i++) {
        result *= L[i][i] * U[i][i];
    }
    return result;
}
