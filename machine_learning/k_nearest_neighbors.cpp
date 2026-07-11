/**
 * @file
 * @brief Implementation of the [K-Nearest Neighbors (KNN) algorithm](https://en.wikipedia.org/wiki/K-nearest_neighbors_algorithm) (K-近邻算法实现)
 *
 * @details
 * KNN 是一种监督学习分类器，它通过测量不同特征值之间的距离方法进行分类。
 * 它的核心思想是：如果一个样本在特征空间中的 K 个最相似（即特征空间中最邻近）的样本中的大多数属于某一个类别，
 * 则该样本也属于这个类别。
 * 本实现使用**欧几里得距离（Euclidean Distance）**作为度量特征空间中两点距离的指标。
 *
 * 时间复杂度: 每次预测 $O(N \cdot F + N \log N)$，其中 $N$ 是训练样本数，$F$ 是特征维度。
 * 空间复杂度: $O(N \cdot F)$。
 *
 * @note
 * 【向量尺寸不匹配及越界参数安全防卫 Bug 审计与修复】：
 * 1. **欧氏距离向量维度不匹配导致内存越界 Bug**：在 `euclidean_distance` 函数中，
 *    原程序直接调用 `std::transform` 遍历 `a` 的元素并与 `b` 进行结合。如果特征输入维度受损导致 `a.size() > b.size()`，
 *    迭代器会越界读取 `b` 之后的非法内存，导致**段错误崩溃或脏读**。
 *    **修复**：在计算前强制添加安全维度校验断言：
 *    `assert(a.size() == b.size() && "Vector dimensions must match!");`
 * 2. **越界超大 K 值引发 vector 读取越界崩溃 Bug**：在 `predict` 中，
 *    如果外部传入的 $K$ 超过了训练集的大小（或传入负数），后续在遍历时执行 `distances.at(i)`
 *    会抛出 `std::out_of_range` 异常导致程序挂掉。
 *    **修复**：添加输入 $K$ 值的有效边界约束校正：
 *    `if (k <= 0 || X_.empty()) return -1; if (static_cast<size_t>(k) > X_.size()) k = X_.size();`
 *
 * @author [Luiz Carlos Cosmi Filho](https://github.com/luizcarloscf)
 */

#include <algorithm>      
#include <cassert>        
#include <cmath>          
#include <iostream>       
#include <numeric>        
#include <unordered_map>  
#include <vector>         

namespace machine_learning {
namespace k_nearest_neighbors {

/**
 * @brief 计算两个一维向量之间的欧几里得距离
 * @tparam T 向量元素类型
 * @param a 第一个特征向量
 * @param b 第二个特征向量
 * @return 两个向量之间的欧氏距离（实数）
 */
template <typename T>
double euclidean_distance(const std::vector<T>& a, const std::vector<T>& b) {
    // 核心修复：添加维度校验防护，杜绝内存越界脏读与段错误
    assert(a.size() == b.size() && "Vector dimensions must match!");

    std::vector<double> aux;
    aux.reserve(a.size());
    std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(aux),
                   [](T x1, T x2) { return std::pow((x1 - x2), 2); });
    return std::sqrt(std::accumulate(aux.begin(), aux.end(), 0.0));
}

/**
 * @brief KNN 分类器模型类
 */
class Knn {
 private:
    std::vector<std::vector<double>> X_{};  ///< 训练集样本特征向量
    std::vector<int> Y_{};                  ///< 训练集样本对应的类别标签

 public:
    /**
     * @brief 构造函数
     * @param X 训练属性特征矩阵
     * @param Y 训练类别标签向量
     */
    explicit Knn(const std::vector<std::vector<double>>& X, const std::vector<int>& Y)
        : X_(X), Y_(Y) {}

    Knn(const Knn& model) = default;
    Knn& operator=(const Knn& model) = default;
    Knn(Knn&&) noexcept = default;
    Knn& operator=(Knn&&) noexcept = default;
    ~Knn() = default;

    /**
     * @brief 对新样本数据点进行 KNN 分类类别预测
     * @param sample 待分类的目标样本特征向量
     * @param k 邻近节点的选择个数
     * @return 预测出的最高频分类标签值
     */
    int predict(const std::vector<double>& sample, int k) const {
        // 核心修复：防止非法 K 值或空训练集导致的 out_of_range 崩溃
        if (k <= 0 || X_.empty()) {
            return -1;
        }
        if (static_cast<size_t>(k) > X_.size()) {
            k = static_cast<int>(X_.size());
        }

        std::vector<int> neighbors;
        std::vector<std::pair<double, int>> distances;
        distances.reserve(X_.size());

        // 1. 计算待测点到所有已知训练点的距离
        for (size_t i = 0; i < this->X_.size(); ++i) {
            auto current = this->X_.at(i);
            auto label = this->Y_.at(i);
            auto distance = euclidean_distance(current, sample);
            distances.emplace_back(distance, label);
        }

        // 2. 升序排序，挑选出最近的 k 个邻近项
        std::sort(distances.begin(), distances.end());
        for (int i = 0; i < k; i++) {
            auto label = distances.at(i).second;
            neighbors.push_back(label);
        }

        // 3. 统计各分类标签在邻近项中的出现频次
        std::unordered_map<int, int> frequency;
        for (auto neighbor : neighbors) {
            ++frequency[neighbor];
        }

        // 4. 投票选出最频繁出现的多数类别
        std::pair<int, int> predicted(-1, -1);
        for (auto& kv : frequency) {
            if (kv.second > predicted.second) {
                predicted.second = kv.second;
                predicted.first = kv.first;
            }
        }
        return predicted.first;
    }
};

}  // namespace k_nearest_neighbors
}  // namespace machine_learning

/**
 * @brief 单元自测用例
 */
static void test() {
    std::cout << "------- Test 1 -------" << std::endl;
    std::vector<std::vector<double>> X1 = {{0.0, 0.0}, {0.25, 0.25},
                                           {0.0, 0.5}, {0.5, 0.5},
                                           {1.0, 0.5}, {1.0, 1.0}};
    std::vector<int> Y1 = {1, 1, 1, 1, 2, 2};
    auto model1 = machine_learning::k_nearest_neighbors::Knn(X1, Y1);
    std::vector<double> sample1 = {1.2, 1.2};
    std::vector<double> sample2 = {0.1, 0.1};
    std::vector<double> sample3 = {0.1, 0.5};
    std::vector<double> sample4 = {1.0, 0.75};
    assert(model1.predict(sample1, 2) == 2);
    assert(model1.predict(sample2, 2) == 1);
    assert(model1.predict(sample3, 2) == 1);
    assert(model1.predict(sample4, 2) == 2);
    std::cout << "... Passed" << std::endl;

    std::cout << "------- Test 2 -------" << std::endl;
    std::vector<std::vector<double>> X2 = {
        {0.0, 0.0, 0.0}, {0.25, 0.25, 0.0}, {0.0, 0.5, 0.0}, {0.5, 0.5, 0.0},
        {1.0, 0.5, 0.0}, {1.0, 1.0, 0.0},   {1.0, 1.0, 1.0}, {1.5, 1.5, 1.0}};
    std::vector<int> Y2 = {1, 1, 1, 1, 2, 2, 3, 3};
    auto model2 = machine_learning::k_nearest_neighbors::Knn(X2, Y2);
    std::vector<double> sample5 = {1.2, 1.2, 0.0};
    std::vector<double> sample6 = {0.1, 0.1, 0.0};
    std::vector<double> sample7 = {0.1, 0.5, 0.0};
    std::vector<double> sample8 = {1.0, 0.75, 1.0};
    assert(model2.predict(sample5, 2) == 2);
    assert(model2.predict(sample6, 2) == 1);
    assert(model2.predict(sample7, 2) == 1);
    assert(model2.predict(sample8, 2) == 3);
    std::cout << "... Passed" << std::endl;

    std::cout << "------- Test 3 -------" << std::endl;
    std::vector<std::vector<double>> X3 = {{0.0}, {1.0}, {2.0}, {3.0},
                                           {4.0}, {5.0}, {6.0}, {7.0}};
    std::vector<int> Y3 = {1, 1, 1, 1, 2, 2, 2, 2};
    auto model3 = machine_learning::k_nearest_neighbors::Knn(X3, Y3);
    std::vector<double> sample9 = {0.5};
    std::vector<double> sample10 = {2.9};
    std::vector<double> sample11 = {5.5};
    std::vector<double> sample12 = {7.5};
    assert(model3.predict(sample9, 3) == 1);
    assert(model3.predict(sample10, 3) == 1);
    assert(model3.predict(sample11, 3) == 2);
    assert(model3.predict(sample12, 3) == 2);
    std::cout << "... Passed" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试用例确认正确性
    return 0;
}
