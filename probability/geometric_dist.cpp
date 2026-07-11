/**
 * @file
 * @brief [Geometric Distribution](https://en.wikipedia.org/wiki/Geometric_distribution) calculations (几何分布计算实现)
 *
 * @details
 * 几何分布用于对伯努利试验进行建模，表示第一次成功发生时所需要的试验次数 $k$。
 * 本实现采用第一种定义：在第 $k$ 次试验中发生首次成功的试验次数分布，其定义域为 $k \in \{1, 2, 3, \ldots\}$。
 *
 * ### 概率质量函数 (PMF)
 * $P(X = k) = (1 - p)^{k-1} p$
 *
 * ### 累积分布函数 (CDF)
 * $P(X \le k) = 1 - (1 - p)^k$
 *
 * 期望值: $\mu = \frac{1}{p}$
 * 方差: $\sigma^2 = \frac{1 - p}{p^2}$
 *
 * 时间复杂度: $O(1)$
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【极值溢出与样本越界校验防卫】：
 * 1. **除零与非法概率值防御**：如果成功概率 $p \le 0$ 或 $p > 1$，期望与方差计算会出现除以零（分母为 $p$ 或 $p^2$）或无意义概率。
 *    **修复**：在构造函数中加入对概率 $p$ 的严格范围校验 `p > 0.0f && p <= 1.0f`，非法时抛出 `std::invalid_argument`。
 * 2. **反函数计算下溢防卫**：在 `inverse_cumulative_distribution` 中计算 `log(1 - cdf)` 时，
 *    如果 $cdf = 1.0$，会导致 $\ln(0)$ 产生负无穷大；若 $p = 1.0$，分母 $\ln(1-p)$ 也会触发除以零。
 *    **修复**：添加边界断言及保护，确保不传入非法区间。
 * 3. **生成随机数引擎现代化**：废弃 C 风格的 `rand()`，采用更安全高效的 `<random>` 头文件与 `std::mt19937`，并且采用固定种子规避单元测试的偶发失败。
 *
 * @author [Domenic Zingsheim](https://github.com/DerAndereDomenic)
 */

#include <cassert>   
#include <cmath>     
#include <cstdint>   
#include <iostream>  
#include <limits>    
#include <random>    
#include <vector>    

namespace probability {
namespace geometric_dist {

// 声明全局伪随机数生成器，使用固定种子确保测试稳定性
inline std::mt19937& get_rng() {
    static std::mt19937 generator(42);
    return generator;
}

/**
 * @brief 生成符合均匀分布的 [0, 1] 区间内的随机浮点数
 */
float generate_uniform() {
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    return distribution(get_rng());
}

/**
 * @brief 几何分布模型类
 */
class geometric_distribution {
 private:
    float p;  ///< 每次伯努利试验成功的概率

 public:
    /**
     * @brief 构造函数
     * @param p 成功概率，必须在 (0, 1] 区间内
     */
    explicit geometric_distribution(const float& p) : p(p) {
        // 核心修复：防卫性校验输入概率的合法性，防止发生除以零和未定义行为
        if (p <= 0.0f || p > 1.0f) {
            throw std::invalid_argument("Probability p must be in (0, 1] range!");
        }
    }

    /**
     * @brief 计算几何分布的期望值
     */
    float expected_value() const { return 1.0f / p; }

    /**
     * @brief 计算几何分布的方差
     */
    float variance() const { return (1.0f - p) / (p * p); }

    /**
     * @brief 计算几何分布的标准差
     */
    float standard_deviation() const { return std::sqrt(variance()); }

    /**
     * @brief 计算首次成功发生在第 k 次试验的概率 PMF
     */
    float probability_density(const uint32_t& k) const {
        if (k == 0) return 0.0f;
        if (p == 1.0f) return (k == 1) ? 1.0f : 0.0f;
        return std::pow((1.0f - p), static_cast<float>(k - 1)) * p;
    }

    /**
     * @brief 计算累积分布函数 CDF (试验次数 <= k 的概率)
     */
    float cumulative_distribution(const uint32_t& k) const {
        if (k == 0) return 0.0f;
        if (p == 1.0f) return 1.0f;
        return 1.0f - std::pow((1.0f - p), static_cast<float>(k));
    }

    /**
     * @brief 逆累积分布函数 (给定累积概率，反推大致所需的试验次数)
     */
    float inverse_cumulative_distribution(const float& cdf) const {
        assert(cdf >= 0.0f && cdf < 1.0f && "CDF must be in [0, 1) to avoid log(0) overflow");
        assert(p < 1.0f && "Inverse cumulative distribution is not defined for p = 1");
        return std::log(1.0f - cdf) / std::log(1.0f - p);
    }

    /**
     * @brief 采样抽取一个符合该几何分布的随机样本
     */
    uint32_t draw_sample() const {
        if (p == 1.0f) return 1;
        float uniform_sample = generate_uniform();
        // 避免极值导致 ln(0) 产生 nan/inf
        if (uniform_sample >= 1.0f) {
            uniform_sample = 0.9999f;
        }
        return static_cast<uint32_t>(
                   inverse_cumulative_distribution(uniform_sample)) + 1;
    }

    /**
     * @brief 计算成功试验发生在区间 [min_tries, max_tries] 内的概率
     */
    float range_tries(const uint32_t& min_tries = 1,
                      const uint32_t& max_tries =
                          std::numeric_limits<uint32_t>::max()) const {
        if (min_tries == 0) {
            return 0.0f;
        }
        float cdf_lower = cumulative_distribution(min_tries - 1);
        float cdf_upper = max_tries == std::numeric_limits<uint32_t>::max()
                              ? 1.0f
                              : cumulative_distribution(max_tries);
        return cdf_upper - cdf_lower;
    }
};

}  // namespace geometric_dist
}  // namespace probability

/**
 * @brief 单元自测 - 检验采样拟合程度
 */
void sample_test(
    const probability::geometric_dist::geometric_distribution& dist) {
    uint32_t n_tries = 1000000;
    std::vector<float> tries(n_tries);

    float mean = 0.0f;
    for (uint32_t i = 0; i < n_tries; ++i) {
        tries[i] = static_cast<float>(dist.draw_sample());
        mean += tries[i];
    }

    mean /= static_cast<float>(n_tries);

    float var = 0.0f;
    for (uint32_t i = 0; i < n_tries; ++i) {
        var += (tries[i] - mean) * (tries[i] - mean);
    }
    var /= static_cast<float>(n_tries - 1);

    std::cout << "This value should be near " << dist.expected_value() << ": "
              << mean << std::endl;
    std::cout << "This value should be near " << dist.variance() << ": " << var
              << std::endl;
}

/**
 * @brief 单元自测用例
 */
static void test() {
    probability::geometric_dist::geometric_distribution dist(0.3f);

    const float threshold = 1e-3f;

    std::cout << "Starting tests for p = 0.3..." << std::endl;
    assert(std::abs(dist.expected_value() - 3.33333333f) < threshold);
    assert(std::abs(dist.variance() - 7.77777777f) < threshold);
    assert(std::abs(dist.standard_deviation() - 2.788866755f) < threshold);
    assert(std::abs(dist.probability_density(5) - 0.07203f) < threshold);
    assert(std::abs(dist.cumulative_distribution(6) - 0.882351f) < threshold);
    assert(std::abs(dist.inverse_cumulative_distribution(
                        dist.cumulative_distribution(8)) -
                    8.0f) < threshold);
    assert(std::abs(dist.range_tries() - 1.0f) < threshold);
    assert(std::abs(dist.range_tries(3) - 0.49f) < threshold);
    assert(std::abs(dist.range_tries(5, 11) - 0.2203267f) < threshold);
    std::cout << "All tests passed" << std::endl;
    sample_test(dist);

    dist = probability::geometric_dist::geometric_distribution(0.5f);

    std::cout << "Starting tests for p = 0.5..." << std::endl;
    assert(std::abs(dist.expected_value() - 2.0f) < threshold);
    assert(std::abs(dist.variance() - 2.0f) < threshold);
    assert(std::abs(dist.standard_deviation() - 1.4142135f) < threshold);
    assert(std::abs(dist.probability_density(5) - 0.03125f) < threshold);
    assert(std::abs(dist.cumulative_distribution(6) - 0.984375f) < threshold);
    assert(std::abs(dist.inverse_cumulative_distribution(
                        dist.cumulative_distribution(8)) -
                    8.0f) < threshold);
    assert(std::abs(dist.range_tries() - 1.0f) < threshold);
    assert(std::abs(dist.range_tries(3) - 0.25f) < threshold);
    assert(std::abs(dist.range_tries(5, 11) - 0.062011f) < threshold);
    std::cout << "All tests passed" << std::endl;
    sample_test(dist);

    dist = probability::geometric_dist::geometric_distribution(0.8f);

    std::cout << "Starting tests for p = 0.8..." << std::endl;
    assert(std::abs(dist.expected_value() - 1.25f) < threshold);
    assert(std::abs(dist.variance() - 0.3125f) < threshold);
    assert(std::abs(dist.standard_deviation() - 0.559016f) < threshold);
    assert(std::abs(dist.probability_density(5) - 0.00128f) < threshold);
    assert(std::abs(dist.cumulative_distribution(6) - 0.999936f) < threshold);
    assert(std::abs(dist.inverse_cumulative_distribution(
                        dist.cumulative_distribution(8)) -
                    8.0f) < threshold);
    assert(std::abs(dist.range_tries() - 1.0f) < threshold);
    assert(std::abs(dist.range_tries(3) - 0.04f) < threshold);
    assert(std::abs(dist.range_tries(5, 11) - 0.00159997f) < threshold);
    std::cout << "All tests have successfully passed!" << std::endl;
    sample_test(dist);
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
