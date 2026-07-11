/**
 * @file
 * @brief [Gram-Schmidt Orthogonalization Process](https://en.wikipedia.org/wiki/Gram%E2%80%93Schmidt_process) (施密特正交化算法实现)
 *
 * @details
 * 施密特正交化是将一组线性无关的向量组 $\{u_1, u_2, \ldots, u_k\}$ 转化为一组两两正交的向量组 $\{v_1, v_2, \ldots, v_k\}$ 的算法。
 * 其几何直观是：对每个新向量，减去它在所有已生成正交向量方向上的投影分量，从而只留下正交（垂直）分量。
 *
 * ### 递推计算公式
 * $v_1 = u_1$
 * $v_k = u_k - \sum_{j=1}^{k-1} \text{proj}_{v_j}(u_k) = u_k - \sum_{j=1}^{k-1} \frac{\langle u_k, v_j \rangle}{\langle v_j, v_j \rangle} v_j$
 *
 * 时间复杂度: $O(K^2 \cdot N)$，其中 $K$ 是向量个数，$N$ 是向量维度。
 * 空间复杂度: $O(K \cdot N)$
 *
 * @note
 * 【按值传递导致输出全为 0 的严重缺陷与无效测试 Bug 审计与修复】：
 * 1. **核心输出参数按值传递失效 Bug**：
 *    原程序将输出数组 `std::array<std::array<double, 10>, 20> B` 声明为按值传递（Pass by Value）。
 *    由于没有加引用修饰符 `&`，所有的正交化计算均发生在局部临时拷贝中，函数返回后**调用方的输出变量依然全是 0**。
 *    **修复**：修改函数签名，将 `B` 声明为引用传递类型 `&B`。
 * 2. **单元测试漏洞 Bug**：
 *    原单元测试中，由于按值传递导致输出数组 `b1`, `b2`, `b3` 全为 0。因为 0 向量与任何向量的内积皆为 0.0，
 *    内积判断条件 `dot <= 0.1` 无条件成立，从而使得**错误的算法逻辑也能通过单元测试**。
 *    **修复**：将输出修改为引用传递，并在单元测试中额外增加断言以校验正交化后的首个向量模长或非零性，防止全 0 蒙混过关。
 * 3. **头文件规范化**：移除不规范的 `#include "math.h"`。
 *
 * @author [Akanksha Gupta](https://github.com/Akanksha-Gupta920)
 */

#include <array>     
#include <cassert>   
#include <cmath>     
#include <iostream>  

namespace numerical_methods {
namespace gram_schmidt {

/**
 * @brief 计算两个向量的内积 (Dot Product)
 * @param x 向量 1
 * @param y 向量 2
 * @param c 向量有效维度
 * @return 内积结果
 */
double dot_product(const std::array<double, 10>& x,
                   const std::array<double, 10>& y, int c) {
    double sum = 0.0;
    for (int i = 0; i < c; ++i) {
        sum += x[i] * y[i];
    }
    return sum;
}

/**
 * @brief 计算向量 x 在向量 y 上的投影系数 factor = <x, y> / <y, y>
 * @param x 向量 1
 * @param y 向量 2
 * @param c 向量有效维度
 * @return 投影比例系数
 */
double projection(const std::array<double, 10>& x,
                  const std::array<double, 10>& y, int c) {
    double dot = dot_product(x, y, c);  
    double anorm = dot_product(y, y, c);  
    
    // 如果 y 是 0 向量，防止发生除零异常
    if (std::abs(anorm) < 1e-15) {
        return 0.0;
    }
    return dot / anorm;  
}

/**
 * @brief 格式化输出正交向量组
 */
void display(int r, int c,
             const std::array<std::array<double, 10>, 20>& B) {
    for (int i = 0; i < r; ++i) {
        std::cout << "Vector " << i + 1 << ": ";
        for (int j = 0; j < c; ++j) {
            std::cout << B[i][j] << " ";
        }
        std::cout << '\n';
    }
}

/**
 * @brief 施密特正交化算法核心过程
 * @param r 向量的个数
 * @param c 向量的维度
 * @param A 输入的线性无关向量组
 * @param B [out] 输出的正交向量组（核心修复：改用引用传递）
 */
void gram_schmidt(int r, int c,
                  const std::array<std::array<double, 10>, 20>& A,
                  std::array<std::array<double, 10>, 20>& B) {
    if (c < r) {  
        std::cout << "Dimension of vector is less than number of vectors, hence \n first "
                  << c << " vectors are orthogonalised\n";
        r = c;
    }

    for (int k = 1; k <= r; ++k) {
        if (k == 1) {
            // 第一个向量直接复制
            for (int j = 0; j < c; j++) {
                B[0][j] = A[0][j];  
            }
        } else {
            std::array<double, 10> all_projection{};  
            
            for (int l = 1; l < k; ++l) {
                double factor = projection(A[k - 1], B[l - 1], c);
                for (int i = 0; i < c; ++i) {
                    all_projection[i] += B[l - 1][i] * factor;  
                }
            }
            
            // 减去在之前所有已正交化向量方向上的投影
            for (int i = 0; i < c; ++i) {
                B[k - 1][i] = A[k - 1][i] - all_projection[i];  
            }
        }
    }
}

}  // namespace gram_schmidt
}  // namespace numerical_methods

/**
 * @brief 单元自测用例
 */
static void test() {
    using namespace numerical_methods::gram_schmidt;

    // 测试用例 1
    std::array<std::array<double, 10>, 20> a1 = {
        {{1.0, 0.0, 1.0, 0.0}, {1.0, 1.0, 1.0, 1.0}, {0.0, 1.0, 2.0, 1.0}}};
    std::array<std::array<double, 10>, 20> b1 = {{{0.0}}};
    gram_schmidt(3, 4, a1, b1);

    // 核心修复：添加非 0 校验，确保算法没有产生全 0 矩阵
    assert(std::abs(b1[0][0] - 1.0) < 1e-9);

    int flag = 1;
    for (int i = 0; i < 2; ++i) {
        for (int j = i + 1; j < 3; ++j) {
            double dot1 = std::abs(dot_product(b1[i], b1[j], 4));
            if (dot1 > 0.1) {
                flag = 0;
                break;
            }
        }
    }
    assert(flag == 1);
    std::cout << "Passed Test Case 1\n";

    // 测试用例 2
    std::array<std::array<double, 10>, 20> a2 = {{{3.0, 1.0}, {2.0, 2.0}}};
    std::array<std::array<double, 10>, 20> b2 = {{{0.0}}};
    gram_schmidt(2, 2, a2, b2);

    assert(std::abs(b2[0][0] - 3.0) < 1e-9);
    flag = 1;
    for (int i = 0; i < 1; ++i) {
        for (int j = i + 1; j < 2; ++j) {
            double dot2 = std::abs(dot_product(b2[i], b2[j], 2));
            if (dot2 > 0.1) {
                flag = 0;
                break;
            }
        }
    }
    assert(flag == 1);
    std::cout << "Passed Test Case 2\n";

    // 测试用例 3
    std::array<std::array<double, 10>, 20> a3 = {{{1.0, 2.0, 2.0}, {-4.0, 3.0, 2.0}}};
    std::array<std::array<double, 10>, 20> b3 = {{{0.0}}};
    gram_schmidt(2, 3, a3, b3);

    assert(std::abs(b3[0][0] - 1.0) < 1e-9);
    flag = 1;
    for (int i = 0; i < 1; ++i) {
        for (int j = i + 1; j < 2; ++j) {
            double dot3 = std::abs(dot_product(b3[i], b3[j], 3));
            if (dot3 > 0.1) {
                flag = 0;
                break;
            }
        }
    }
    assert(flag == 1);
    std::cout << "Passed Test Case 3\n";
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自动化单元测试，杜绝阻塞
    return 0;
}
