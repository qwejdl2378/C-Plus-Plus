/**
 * @file
 * @brief [Fast Fourier Transform (FFT)](https://medium.com/@aiswaryamathur/understanding-fast-fouriertransform-from-scratch-to-solve-polynomial-multiplication-8018d511162f) implementation (快速傅里叶变换算法实现)
 *
 * @details
 * 快速傅里叶变换（FFT）是计算离散傅里叶变换（DFT）及其逆变换的快速算法。
 * 对于长度为 $N$ 的序列，传统 DFT 的计算复杂度为 $O(N^2)$，而 FFT 利用分治思想（通常采用库利-图基算法），
 * 将其划分为奇数项和偶数项子问题递归求解，将复杂度大幅降低至 $O(N \log N)$。
 *
 * 本实现采用现代 C++ `std::vector` 容器进行重构，规避了原始指针实现中极易发生的内存泄漏和越界隐患。
 *
 * 时间复杂度: $O(N \log N)$，其中 $N$ 为输入序列长度（必须为 2 的幂）。
 * 空间复杂度: $O(N \log N)$ (由于递归调用产生的临时向量空间)
 *
 * @note
 * 【内存泄漏、类型溢出与非 2 的幂输入 Bug 审计与修复】：
 * 1. **原程序大量手动动态内存分配（new/delete）导致的内存泄漏与双重释放隐患**：
 *    原程序在递归中使用原始指针和大量的 `new[]`，手动跟踪释放十分混乱且易在异常时发生内存泄漏。
 *    **修复**：重构为使用智能容器 `std::vector<std::complex<double>>`，由编译器自动进行生命周期管理，彻底根除泄漏隐患。
 * 2. **步长溢出与类型过窄限制 Bug**：
 *    原实现中参数 $n$ 的类型被硬编码为仅 8 位的 `uint8_t`。这导致**最大输入长度被限制在 255 字节**，
 *    一旦传入 256 或更大长度就会发生整型截断和死循环。
 *    **修复**：将序列长度类型修改为通用的 `size_t`。
 * 3. **非 2 的幂输入防卫缺失**：
 *    FFT 分治算法要求输入规模必须为 2 的整数次幂。若传入奇数或非 2 的幂，递归拆分会导致边界越界。
 *    **修复**：加入 `(n & (n - 1)) == 0` 断言校验。
 *
 * @author [Ameya Chawla](https://github.com/ameyachawlaggsipu)
 */

#include <cassert>   
#include <cmath>     
#include <complex>   
#include <iostream>  
#include <vector>    

namespace numerical_methods {
namespace fft {

/**
 * @brief 递归快速傅里叶变换 (FFT) 算法
 * @param p 复数系数向量
 * @return 变换后的复数频域向量
 */
std::vector<std::complex<double>> FastFourierTransform(const std::vector<std::complex<double>> &p) {
    size_t n = p.size();
    if (n == 1) {
        return p;  /// 递归基：单个元素直接返回
    }

    // 核心修复：防卫非 2 的幂次输入
    assert((n > 0) && ((n & (n - 1)) == 0) && "FFT input size N must be a power of 2!");

    const double pi = 2.0 * std::asin(1.0);  
    std::complex<double> om(std::cos(2.0 * pi / n), std::sin(2.0 * pi / n));  

    std::vector<std::complex<double>> pe(n / 2);  /// 偶数项系数
    std::vector<std::complex<double>> po(n / 2);  /// 奇数项系数

    for (size_t j = 0; j < n; j++) {
        if (j % 2 == 0) {
            pe[j / 2] = p[j];
        } else {
            po[j / 2] = p[j];
        }
    }

    // 递归求解子问题
    std::vector<std::complex<double>> ye = FastFourierTransform(pe);
    std::vector<std::complex<double>> yo = FastFourierTransform(po);

    std::vector<std::complex<double>> y(n);  
    std::complex<double> w(1.0, 0.0);

    // 合并子问题结果
    for (size_t i = 0; i < n / 2; i++) {
        y[i] = ye[i] + w * yo[i];
        y[i + n / 2] = ye[i] - w * yo[i];
        w *= om;
    }

    return y;
}

}  // namespace fft
}  // namespace numerical_methods

/**
 * @brief 单元自测用例
 */
static void test() {
    using namespace numerical_methods::fft;

    // 测试用例 1: N = 2
    std::vector<std::complex<double>> t1 = {{1.0, 0.0}, {2.0, 0.0}};
    std::vector<std::complex<double>> r1 = {{3.0, 0.0}, {-1.0, 0.0}};

    // 测试用例 2: N = 4
    std::vector<std::complex<double>> t2 = {{1.0, 0.0}, {2.0, 0.0}, {3.0, 0.0}, {4.0, 0.0}};
    std::vector<std::complex<double>> r2 = {
        {10.0, 0.0}, {-2.0, -2.0}, {-2.0, 0.0}, {-2.0, 2.0}
    };

    auto o1 = FastFourierTransform(t1);
    auto o2 = FastFourierTransform(t2);

    constexpr double epsilon = 1e-11;

    // 校验用例 1
    assert(o1.size() == r1.size());
    for (size_t i = 0; i < o1.size(); i++) {
        assert(std::abs(r1[i].real() - o1[i].real()) < epsilon);
        assert(std::abs(r1[i].imag() - o1[i].imag()) < epsilon);
    }

    // 校验用例 2
    assert(o2.size() == r2.size());
    for (size_t i = 0; i < o2.size(); i++) {
        assert(std::abs(r2[i].real() - o2[i].real()) < epsilon);
        assert(std::abs(r2[i].imag() - o2[i].imag()) < epsilon);
    }

    std::cout << "All FFT tests have successfully passed!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行单元测试
    return 0;
}
