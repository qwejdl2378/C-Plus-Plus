/**
 * @file
 * @brief [Inverse Fast Fourier Transform (IFFT)](https://www.geeksforgeeks.org/python-inverse-fast-fourier-transformation/) implementation (逆快速傅里叶变换算法实现)
 *
 * @details
 * 逆快速傅里叶变换（IFFT）是用于将频域信号转换为时域信号的算法。
 * 根据数学定义，IFFT 可以非常优雅地通过快速傅里叶变换（FFT）本身来实现，而无需独立编写重复的分治逻辑。
 * 具体数学恒等式如下：
 *
 * $\text{IFFT}(X) = \frac{1}{N} \cdot \left[ \text{FFT}(X^*) \right]^*$
 *
 * 其中 $X^*$ 表示对复数向量 $X$ 取共轭（Real 保持不变，Imag 取相反数）。
 *
 * 时间复杂度: $O(N \log N)$，与 FFT 完全相同。
 * 空间复杂度: $O(N \log N)$
 *
 * @note
 * 【测试断言漏洞与绝对值缺失 Bug 审计与修复】：
 * 1. **原程序测试中未加 std::abs 比较导致无效校验 Bug**：
 *    原程序的测试校验写成了 `assert((r1[i].real() - o1[i].real() < 0.000000000001))`。
 *    **由于未加绝对值 `std::abs`**，只要差值为负数（例如 $1.0 - 2.0 = -1.0$），该不等式就会无条件成立。
 *    这导致即使 IFFT 计算出错误的结果（原算法对 $N=2$ 算出的结果是两倍的错误值 `{2, 4}` 而非期望的 `{1, 2}`），
 *    测试依然能“成功”通过。这是一个极其隐蔽且严重的假测试通过 Bug。
 *    **修复**：使用 `std::abs` 对实部和虚部的绝对误差进行严格校验。
 * 2. **手动内存管理与类型溢出 Bug**：
 *    同 FFT 类似，原 IFFT 同样受限于 `uint8_t` (最大长度限制在 255) 并且充满大量的 `new[]/delete[]` 内存泄漏风险。
 *    **修复**：重构为 `std::vector`，并利用 $\text{IFFT}(X) = \frac{1}{N} \left[\text{FFT}(X^*)\right]^*$ 恒等式调用已修正的 `FastFourierTransform` 模块。
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
// 引用已在 fast_fourier_transform.cpp 中实现的 FFT 接口
std::vector<std::complex<double>> FastFourierTransform(const std::vector<std::complex<double>> &p);
}  // namespace fft

namespace ifft {

/**
 * @brief 逆快速傅里叶变换 (IFFT) 核心函数
 * @param p 频域输入复数向量
 * @return 还原后的时域复数向量
 */
std::vector<std::complex<double>> InverseFastFourierTransform(std::vector<std::complex<double>> p) {
    size_t n = p.size();
    if (n == 0) return {};

    // 1. 对输入向量取共轭 (Conjugate)
    for (size_t i = 0; i < n; ++i) {
        p[i] = std::conj(p[i]);
    }

    // 2. 调用正向 FFT
    std::vector<std::complex<double>> y = numerical_methods::fft::FastFourierTransform(p);

    // 3. 对结果再次取共轭，并除以总长度 N (Divide by N and Conjugate)
    for (size_t i = 0; i < n; ++i) {
        y[i] = std::conj(y[i]) / static_cast<double>(n);
    }

    return y;
}

}  // namespace ifft
}  // namespace numerical_methods

// 为了使本文件可以单独编译进行测试，我们在 anonymous namespace 中复制一份 FFT 实现
namespace {
std::vector<std::complex<double>> local_fft(const std::vector<std::complex<double>> &p) {
    size_t n = p.size();
    if (n == 1) return p;
    assert((n > 0) && ((n & (n - 1)) == 0));
    const double pi = 2.0 * std::asin(1.0);  
    std::complex<double> om(std::cos(2.0 * pi / n), std::sin(2.0 * pi / n));  
    std::vector<std::complex<double>> pe(n / 2), po(n / 2);
    for (size_t j = 0; j < n; j++) {
        if (j % 2 == 0) pe[j / 2] = p[j];
        else po[j / 2] = p[j];
    }
    std::vector<std::complex<double>> ye = local_fft(pe);
    std::vector<std::complex<double>> yo = local_fft(po);
    std::vector<std::complex<double>> y(n);  
    std::complex<double> w(1.0, 0.0);
    for (size_t i = 0; i < n / 2; i++) {
        y[i] = ye[i] + w * yo[i];
        y[i + n / 2] = ye[i] - w * yo[i];
        w *= om;
    }
    return y;
}
}  // namespace

// 如果是单独链接本测试文件，我们将上面的 FFT 代理给 local_fft
namespace numerical_methods {
namespace fft {
std::vector<std::complex<double>> FastFourierTransform(const std::vector<std::complex<double>> &p) {
    return local_fft(p);
}
}
}

/**
 * @brief 单元自测用例
 */
static void test() {
    using namespace numerical_methods::ifft;

    // 测试用例 1: N = 2
    std::vector<std::complex<double>> t1 = {{3.0, 0.0}, {-1.0, 0.0}};
    std::vector<std::complex<double>> r1 = {{1.0, 0.0}, {2.0, 0.0}};

    // 测试用例 2: N = 4
    std::vector<std::complex<double>> t2 = {
        {10.0, 0.0}, {-2.0, -2.0}, {-2.0, 0.0}, {-2.0, 2.0}
    };
    std::vector<std::complex<double>> r2 = {
        {1.0, 0.0}, {2.0, 0.0}, {3.0, 0.0}, {4.0, 0.0}
    };

    auto o1 = InverseFastFourierTransform(t1);
    auto o2 = InverseFastFourierTransform(t2);

    constexpr double epsilon = 1e-11;

    // 核心修复：使用 std::abs 对差值进行严格的精度校验
    assert(o1.size() == r1.size());
    for (size_t i = 0; i < o1.size(); i++) {
        assert(std::abs(r1[i].real() - o1[i].real()) < epsilon);
        assert(std::abs(r1[i].imag() - o1[i].imag()) < epsilon);
    }

    assert(o2.size() == r2.size());
    for (size_t i = 0; i < o2.size(); i++) {
        assert(std::abs(r2[i].real() - o2[i].real()) < epsilon);
        assert(std::abs(r2[i].imag() - o2[i].imag()) < epsilon);
    }

    std::cout << "All IFFT tests have successfully passed!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行单元测试
    return 0;
}
