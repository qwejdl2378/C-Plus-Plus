/**
 * @file
 * @brief Implementation of the [Kohonen Self-Organizing Map (SOM)](https://en.wikipedia.org/wiki/Self-organizing_map) for data tracing (Kohonen 自组织映射神经网络实现 - 数据拟合追踪)
 *
 * @details
 * 自组织映射（SOM）追踪算法通过一维的输出权重链条，将高维数据的特征模式逼近拟合，
 * 在样本空间中生成一条贴合输入数据分布形状的权重节点链。它可以用于路径规划、曲线追踪等任务。
 *
 * ### 寻址与竞争机制
 * 与拓扑映射网格类似，输入样本后寻找欧氏距离最近的一维链条节点（BMU），
 * 并对其前后邻域范围 $R$ 内的所有一维链节点执行权重更新。
 *
 * 时间复杂度: 每次迭代 $O(N \cdot P \cdot F)$，其中 $N$ 是样本数，$P$ 是一维链条大小，$F$ 是特征维度。
 * 空间复杂度: $O(P \cdot F)$。
 *
 * @note
 * 【测试遗留临时文件污染与空向量解引用 Bug 审计与修复】：
 * 1. **单元测试遗留 CSV 文件污染 Bug**：在测试执行完毕后，工作空间中会堆积 9 个临时调试 CSV 文件。
 *    **修复**：在 `main` 阶段结束后添加 `std::remove` 对产生的 9 个临时测试数据文件进行闭环回收清理。
 * 2. **D 向量空校验**：在 `update_weights` 中调用 `std::min_element`。如果 `D` 大小为 0，会造成未定义行为。添加了安全大小验证。
 *
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#define _USE_MATH_DEFINES  
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <valarray>
#include <vector>

#ifdef _OPENMP  
#include <omp.h>
#endif

/**
 * @brief 在区间 [a, b] 之间生成一个随机双精度浮点数
 */
double _random(double a, double b) {
    return ((b - a) * (std::rand() % 100) / 100.f) + a;
}

/**
 * @brief 将一个一维/多维数据矩阵保存到 CSV 文件中
 * @return 0 成功, -1 失败
 */
int save_nd_data(const char *fname,
                 const std::vector<std::valarray<double>> &X) {
    if (X.empty()) return 0;
    size_t num_points = X.size();       
    size_t num_features = X[0].size();  

    std::ofstream fp;
    fp.open(fname);
    if (!fp.is_open()) {
        std::cerr << "Error opening file " << fname << "\n";
        return -1;
    }

    for (size_t i = 0; i < num_points; i++) {
        for (size_t j = 0; j < num_features; j++) {
            fp << X[i][j];               
            if (j < num_features - 1) {  
                fp << ",";               
            }
        }
        if (i < num_points - 1) {  
            fp << "\n";            
        }
    }

    fp.close();
    return 0;
}

namespace machine_learning {

/**
 * @brief 使用 Kohonen 一维拓扑学习规则更新邻域内节点的权重
 * @param x 传入的单个样本特征向量
 * @param W [in,out] 一维链权重矩阵
 * @param D 距离临时缓存向量
 * @param alpha 学习率
 * @param R 邻域范围
 */
void update_weights(const std::valarray<double> &x,
                    std::vector<std::valarray<double>> *W,
                    std::valarray<double> *D, double alpha, int R) {
    int j = 0;
    int num_out = static_cast<int>(W->size());  

#ifdef _OPENMP
#pragma omp parallel for private(j)
#endif
    for (j = 0; j < num_out; j++) {
        (*D)[j] = (((*W)[j] - x) * ((*W)[j] - x)).sum();
    }

    // 核心修复：防卫性空数组校验，规避 std::min_element 迭代器空指针解引用崩溃
    if (D->size() == 0) {
        return;
    }
    auto result = std::min_element(std::begin(*D), std::end(*D));
    int d_min_idx = static_cast<int>(std::distance(std::begin(*D), result));

    // 计算一维邻近节点区间范围
    int from_node = std::max(0, d_min_idx - R);
    int to_node = std::min(num_out, d_min_idx + R + 1);

#ifdef _OPENMP
#pragma omp parallel for private(j)
#endif
    for (j = from_node; j < to_node; j++) {
        (*W)[j] += alpha * (x - (*W)[j]);
    }
}

/**
 * @brief 一维 SOM 曲线追踪拟合主算法
 */
void kohonen_som_tracer(const std::vector<std::valarray<double>> &X,
                        std::vector<std::valarray<double>> *W,
                        double alpha_min) {
    int num_samples = static_cast<int>(X.size());  
    int num_out = static_cast<int>(W->size());  
    int R = num_out >> 2, iter = 0;
    double alpha = 1.f;

    std::valarray<double> D(num_out);

    do {
        for (int sample = 0; sample < num_samples; sample++) {
            update_weights(X[sample], W, &D, alpha, R);
        }

        // 每 10 代将更新的邻域半径缩小
        if (iter % 10 == 0 && R > 1) {
            R--;
        }

        alpha -= 0.01;
        iter++;
    } while (alpha > alpha_min);
}

}  // namespace machine_learning

using machine_learning::kohonen_som_tracer;

/**
 * @brief 生成圆周附近的测试随机数据点分布
 */
void test_circle(std::vector<std::valarray<double>> *data) {
    const size_t N = data->size();
    const double R = 0.75, dr = 0.3;
    double a_t = 0., b_t = 2.f * M_PI;  
    double a_r = R - dr, b_r = R + dr;  

    for (size_t i = 0; i < N; i++) {
        double r = _random(a_r, b_r);      
        double theta = _random(a_t, b_t);  
        (*data)[i][0] = r * cos(theta);    
        (*data)[i][1] = r * sin(theta);
    }
}

/**
 * @brief 测试 1：圆环周分布拟合追踪
 */
void test1() {
    int N = 500;
    int features = 2;
    int num_out = 50;
    std::vector<std::valarray<double>> X(N);
    std::vector<std::valarray<double>> W(num_out);
    for (int i = 0; i < std::max(num_out, N); i++) {
        if (i < N) {  
            X[i] = std::valarray<double>(features);
        }
        if (i < num_out) {  
            W[i] = std::valarray<double>(features);
            for (int j = 0; j < features; j++) {
                W[i][j] = _random(-1, 1);
            }
        }
    }

    test_circle(&X);  
    save_nd_data("test1.csv", X);    
    save_nd_data("w11.csv", W);      
    kohonen_som_tracer(X, &W, 0.1);  
    save_nd_data("w12.csv", W);      
}

/**
 * @brief 生成 Gerono 无穷大字形线（Lamniscate）的随机特征数据点
 */
void test_lamniscate(std::vector<std::valarray<double>> *data) {
    const size_t N = data->size();
    const double dr = 0.2;

    for (size_t i = 0; i < N; i++) {
        double dx = _random(-dr, dr);     
        double dy = _random(-dr, dr);     
        double theta = _random(0, M_PI);  
        (*data)[i][0] = dx + cos(theta);  
        (*data)[i][1] = dy + sin(2. * theta) / 2.f;
    }
}

/**
 * @brief 测试 2：双叶 Lamniscate 曲线分布拟合追踪
 */
void test2() {
    int N = 500;
    int features = 2;
    int num_out = 20;
    std::vector<std::valarray<double>> X(N);
    std::vector<std::valarray<double>> W(num_out);
    for (int i = 0; i < std::max(num_out, N); i++) {
        if (i < N) {  
            X[i] = std::valarray<double>(features);
        }
        if (i < num_out) {  
            W[i] = std::valarray<double>(features);
            for (int j = 0; j < features; j++) {
                W[i][j] = _random(-1, 1);
            }
        }
    }

    test_lamniscate(&X);              
    save_nd_data("test2.csv", X);     
    save_nd_data("w21.csv", W);       
    kohonen_som_tracer(X, &W, 0.01);  
    save_nd_data("w22.csv", W);       
}

/**
 * @brief 生成空间八个三维聚类数据点
 */
void test_3d_classes(std::vector<std::valarray<double>> *data) {
    const size_t N = data->size();
    const double R = 0.1;  
    const int num_classes = 8;
    const std::array<const std::array<double, 3>, num_classes> centres = {
        std::array<double, 3>({.5, .5, .5}),    
        std::array<double, 3>({.5, .5, -.5}),   
        std::array<double, 3>({.5, -.5, .5}),   
        std::array<double, 3>({.5, -.5, -.5}),  
        std::array<double, 3>({-.5, .5, .5}),   
        std::array<double, 3>({-.5, .5, -.5}),  
        std::array<double, 3>({-.5, -.5, .5}),  
        std::array<double, 3>({-.5, -.5, -.5})  
    };

    for (size_t i = 0; i < N; i++) {
        int cls = std::rand() % num_classes;  
        (*data)[i][0] = _random(centres[cls][0] - R, centres[cls][0] + R);
        (*data)[i][1] = _random(centres[cls][1] - R, centres[cls][1] + R);
        (*data)[i][2] = _random(centres[cls][2] - R, centres[cls][2] + R);
    }
}

/**
 * @brief 测试 3：3维多类别的空间连通链条追踪
 */
void test3() {
    int N = 200;
    int features = 3;
    int num_out = 20;
    std::vector<std::valarray<double>> X(N);
    std::vector<std::valarray<double>> W(num_out);
    for (int i = 0; i < std::max(num_out, N); i++) {
        if (i < N) {  
            X[i] = std::valarray<double>(features);
        }
        if (i < num_out) {  
            W[i] = std::valarray<double>(features);
            for (int j = 0; j < features; j++) {
                W[i][j] = _random(-1, 1);
            }
        }
    }

    test_3d_classes(&X);              
    save_nd_data("test3.csv", X);     
    save_nd_data("w31.csv", W);       
    kohonen_som_tracer(X, &W, 0.01);  
    save_nd_data("w32.csv", W);       
}

/**
 * @brief 统计时间辅助函数
 */
double get_clock_diff(clock_t start_t, clock_t end_t) {
    return static_cast<double>(end_t - start_t) / CLOCKS_PER_SEC;
}

/**
 * @brief 主函数
 */
int main() {
#ifdef _OPENMP
    std::cout << "Using OpenMP based parallelization\n";
#else
    std::cout << "NOT using OpenMP based parallelization\n";
#endif

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    std::clock_t start_clk = std::clock();
    test1();
    auto end_clk = std::clock();
    std::cout << "Test 1 completed in " << get_clock_diff(start_clk, end_clk)
              << " sec\n";

    start_clk = std::clock();
    test2();
    end_clk = std::clock();
    std::cout << "Test 2 completed in " << get_clock_diff(start_clk, end_clk)
              << " sec\n";

    start_clk = std::clock();
    test3();
    end_clk = std::clock();
    std::cout << "Test 3 completed in " << get_clock_diff(start_clk, end_clk)
              << " sec\n";

    // 核心修复：自动回收生成的 9 个 CSV 文件，保持工作区干净清爽
    std::remove("test1.csv");
    std::remove("w11.csv");
    std::remove("w12.csv");
    std::remove("test2.csv");
    std::remove("w21.csv");
    std::remove("w22.csv");
    std::remove("test3.csv");
    std::remove("w31.csv");
    std::remove("w32.csv");

    std::cout
        << "(Note: Calculated times include: creating test sets, training "
           "model, writing files to disk, and cleaning up.)\n\n";
    return 0;
}
