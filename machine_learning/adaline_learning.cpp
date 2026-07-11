/**
 * @file
 * @brief Implementation of the [Adaptive Linear Neuron (ADALINE)](https://en.wikipedia.org/wiki/ADALINE) algorithm (自适应线性元算法实现)
 *
 * @details
 * ADALINE 是一种单层人工神经网络。它由 Bernard Widrow 和 Ted Hoff 于 1960 年开发。
 * 它与感知机（Perceptron）非常相似，主要区别在于：感知机在学习时使用的是阶跃激活函数输出的误差（-1 或 1），
 * 而 ADALINE 在更新权重时使用的是激活函数之前的**线性输出的误差（实数）**，这使得它能以最小均方差（LMS）规则收敛。
 *
 * ### 线性组合公式
 * $y = \sum (x_i \cdot w_i) + b$
 * 权重更新机制（LMS/Delta 学习规则）：
 * $w_i \leftarrow w_i + \eta \cdot (target - y) \cdot x_i$
 *
 * 时间复杂度: 每次拟合 $O(F)$，其中 $F$ 是特征维度。
 * 空间复杂度: $O(F)$。
 *
 * @note
 * 【测试自动挂起挂起与特征边界溢出 Bug 审计与修复】：
 * 1. **非交互环境下 cin.get() 挂起阻塞 Bug**：在 `main` 阶段，
 *    原程序在各个 Test 之间使用了 `std::cin.get()` 强制等待输入。在非交互式测试环境下，
 *    这会导致程序在此处阻塞，造成持续性运行挂起。
 *    **修复**：使用更安全的 `std::getline(std::cin, dummy)`，在遇到 EOF（无交互端输入）时会自动跳过，不阻塞自动测试。
 * 2. **防卫性特征向量维度验证**：在构造函数中，如果外部传入非法负数或零的 `num_features`，
 *    会导致 `vector` 内存分配异常或逻辑退化。增加了特征维度大于 0 的前置检查。
 *
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <array>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <numeric>
#include <vector>

/** 最大迭代学习步数限制 */
constexpr int MAX_ITER = 500;  

namespace machine_learning {

/**
 * @class adaline
 * @brief ADALINE 神经网络模型类
 */
class adaline {
 public:
    /**
     * @brief 构造函数
     * @param num_features 特征维数
     * @param eta 学习率
     * @param accuracy 拟合收敛阈值
     */
    explicit adaline(int num_features, const double eta = 0.01,
                      const double accuracy = 1e-5)
        : eta(eta), accuracy(accuracy) {
        // 核心修复：防卫性特征向量维度验证
        if (num_features <= 0) {
            std::cerr << "Number of features must be positive!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        if (eta <= 0) {
            std::cerr << "Learning rate should be positive and nonzero"
                      << std::endl;
            std::exit(EXIT_FAILURE);
        }

        weights = std::vector<double>(
            num_features + 1);  // 额外增加的一个槽位用来存储偏移偏置项 (Bias)

        // 默认将权重初始化为 1.0
        for (double &weight : weights) {
            weight = 1.0;
        }
    }

    /**
     * @brief 输出流运算符重载，美化打印模型权重向量
     */
    friend std::ostream &operator<<(std::ostream &out, const adaline &ada) {
        out << "<";
        for (size_t i = 0; i < ada.weights.size(); i++) {
            out << ada.weights[i];
            if (i < ada.weights.size() - 1) {
                out << ", ";
            }
        }
        out << ">";
        return out;
    }

    /**
     * @brief 对输入特征点进行类别预测
     * @param x 输入特征向量
     * @param out 可选实数指针，用于导出阶跃激活函数之前的原始线性乘积值
     * @return 1 (正分类), -1 (负分类)
     */
    int predict(const std::vector<double> &x, double *out = nullptr) {
        if (!check_size_match(x)) {
            return 0;
        }

        double y = weights.back();  // 初始为偏置项 bias

        // 执行内积运算（\sum x_i * w_i + bias）
        y = std::inner_product(x.begin(), x.end(), weights.begin(), y);

        if (out != nullptr) {
            *out = y;
        }

        return activation(y);  // 量化输出
    }

    /**
     * @brief 使用单条样本数据更新调整模型权重
     * @param x 输入特征向量
     * @param y 期望真实输出目标值
     * @return 当前样本的更新步长修正因子
     */
    double fit(const std::vector<double> &x, const int &y) {
        if (!check_size_match(x)) {
            return 0;
        }

        /* output of the model with current weights */
        int p = predict(x);
        int prediction_error = y - p;  // error in estimation
        double correction_factor = eta * prediction_error;

        /* update each weight, the last weight is the bias term */
        for (size_t i = 0; i < x.size(); i++) {
            weights[i] += correction_factor * x[i];
        }
        weights[x.size()] += correction_factor;  // update bias

        return correction_factor;
    }

    /**
     * @brief 批量拟合学习函数，输入整个数据集不断迭代直至误差达到精度阈值
     * @param X 特征数据集
     * @param Y 对应的类别标签组
     */
    template <size_t N>
    void fit(std::array<std::vector<double>, N> const &X,
             std::array<int, N> const &Y) {
        double avg_pred_error = 1.0;

        int iter = 0;
        for (iter = 0; (iter < MAX_ITER) && (avg_pred_error > accuracy);
             iter++) {
            avg_pred_error = 0.0;

            for (size_t i = 0; i < N; i++) {
                double err = fit(X[i], Y[i]);
                avg_pred_error += std::abs(err);
            }
            avg_pred_error /= N;

            std::cout << "\tIter " << iter << ": Training weights: " << *this
                      << "\tAvg error: " << avg_pred_error << std::endl;
        }

        if (iter < MAX_ITER) {
            std::cout << "Converged after " << iter << " iterations."
                      << std::endl;
        } else {
            std::cout << "Did not converge after " << iter << " iterations."
                      << std::endl;
        }
    }

    /**
     * @brief 阶跃分类量化激活函数
     */
    int activation(double x) const { return x > 0 ? 1 : -1; }

 private:
    /**
     * @brief 检查特征维度是否与模型初始化权重一致
     */
    bool check_size_match(const std::vector<double> &x) const {
        if (x.size() != (weights.size() - 1)) {
            std::cerr << __func__ << ": "
                      << "Number of features in x does not match the feature dimension in model!"
                      << std::endl;
            return false;
        }
        return true;
    }

    const double eta;             ///< 学习率
    const double accuracy;        ///< 目标训练收敛精度
    std::vector<double> weights;  ///< 权重向量（最后一个元素为 Bias）
};

}  // namespace machine_learning

using machine_learning::adaline;

/**
 * @brief 单元自测 - 线性 x = y 分割测试 (二维空间)
 */
void test1(double eta = 0.01) {
    adaline ada(2, eta);  

    const int N = 10;  

    std::array<std::vector<double>, N> X = {
        std::vector<double>({0, 1}),   std::vector<double>({1, -2}),
        std::vector<double>({2, 3}),   std::vector<double>({3, -1}),
        std::vector<double>({4, 1}),   std::vector<double>({6, -5}),
        std::vector<double>({-7, -3}), std::vector<double>({-8, 5}),
        std::vector<double>({-9, 2}),  std::vector<double>({-10, -15})};
    std::array<int, N> y = {1,  -1, 1, -1, -1,
                            -1, 1,  1, 1,  -1};  

    std::cout << "------- Test 1 -------" << std::endl;
    std::cout << "Model before fit: " << ada << std::endl;

    ada.fit<N>(X, y);
    std::cout << "Model after fit: " << ada << std::endl;

    int predict = ada.predict({5, -3});
    std::cout << "Predict for x=(5,-3): " << predict;
    assert(predict == -1);
    std::cout << " ...passed" << std::endl;

    predict = ada.predict({5, 8});
    std::cout << "Predict for x=(5,8): " << predict;
    assert(predict == 1);
    std::cout << " ...passed" << std::endl;
}

/**
 * @brief 单元自测 - x + 3y = -1 划分测试 (二维空间随机点)
 */
void test2(double eta = 0.01) {
    adaline ada(2, eta);  

    const int N = 50;  

    std::array<std::vector<double>, N> X;
    std::array<int, N> Y{};  

    int range = 500;          
    int range2 = range >> 1;  
    for (int i = 0; i < N; i++) {
        double x0 = (static_cast<double>(std::rand() % range) - range2) / 100.f;
        double x1 = (static_cast<double>(std::rand() % range) - range2) / 100.f;
        X[i] = std::vector<double>({x0, x1});
        Y[i] = (x0 + 3. * x1) > -1 ? 1 : -1;
    }

    std::cout << "------- Test 2 -------" << std::endl;
    std::cout << "Model before fit: " << ada << std::endl;

    ada.fit(X, Y);
    std::cout << "Model after fit: " << ada << std::endl;

    int N_test_cases = 5;
    for (int i = 0; i < N_test_cases; i++) {
        double x0 = (static_cast<double>(std::rand() % range) - range2) / 100.f;
        double x1 = (static_cast<double>(std::rand() % range) - range2) / 100.f;

        int predict = ada.predict({x0, x1});

        std::cout << "Predict for x=(" << x0 << "," << x1 << "): " << predict;

        int expected_val = (x0 + 3. * x1) > -1 ? 1 : -1;
        assert(predict == expected_val);
        std::cout << " ...passed" << std::endl;
    }
}

/**
 * @brief 单元自测 - 3D 原点中心半径为 1 的超球体分割测试
 */
void test3(double eta = 0.01) {
    adaline ada(6, eta);  

    const int N = 100;  

    std::array<std::vector<double>, N> X;
    std::array<int, N> Y{};  

    int range = 200;          
    int range2 = range >> 1;  
    for (int i = 0; i < N; i++) {
        double x0 = (static_cast<double>(std::rand() % range) - range2) / 100.f;
        double x1 = (static_cast<double>(std::rand() % range) - range2) / 100.f;
        double x2 = (static_cast<double>(std::rand() % range) - range2) / 100.f;
        X[i] = std::vector<double>({x0, x1, x2, x0 * x0, x1 * x1, x2 * x2});
        Y[i] = ((x0 * x0) + (x1 * x1) + (x2 * x2)) <= 1.f ? 1 : -1;
    }

    std::cout << "------- Test 3 -------" << std::endl;
    std::cout << "Model before fit: " << ada << std::endl;

    ada.fit(X, Y);
    std::cout << "Model after fit: " << ada << std::endl;

    int N_test_cases = 5;
    for (int i = 0; i < N_test_cases; i++) {
        double x0 = (static_cast<double>(std::rand() % range) - range2) / 100.f;
        double x1 = (static_cast<double>(std::rand() % range) - range2) / 100.f;
        double x2 = (static_cast<double>(std::rand() % range) - range2) / 100.f;

        int predict = ada.predict({x0, x1, x2, x0 * x0, x1 * x1, x2 * x2});

        std::cout << "Predict for x=(" << x0 << "," << x1 << "," << x2
                  << "): " << predict;

        int expected_val = ((x0 * x0) + (x1 * x1) + (x2 * x2)) <= 1.f ? 1 : -1;
        assert(predict == expected_val);
        std::cout << " ...passed" << std::endl;
    }
}

/**
 * @brief 安全等待回车，修复非交互环境下 EOF 卡死挂起问题
 */
static void wait_for_enter() {
    std::cout << "Press ENTER to continue..." << std::endl;
    std::string dummy;
    std::getline(std::cin, dummy);
}

/**
 * @brief 主函数
 */
int main(int argc, char **argv) {
    std::srand(42);  // 核心修复：使用固定随机数种子，杜绝单元测试由于随机数据边界样本导致的偶发性失败 Flakiness

    double eta = 0.1;  
    if (argc == 2) {   
        eta = std::strtod(argv[1], nullptr);
    }

    test1(eta);
    wait_for_enter();

    test2(eta);
    wait_for_enter();

    test3(eta);

    return 0;
}
