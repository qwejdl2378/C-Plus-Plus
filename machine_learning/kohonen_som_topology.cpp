/**
 * @file
 * @brief Implementation of the [Kohonen Self-Organizing Map (SOM)](https://en.wikipedia.org/wiki/Self-organizing_map) for topological mapping (Kohonen 自组织映射神经网络实现 - 拓扑结构映射)
 *
 * @details
 * 自组织映射（SOM）是一种无监督学习神经网络。它通过高维数据点的学习，在低维（通常是二维）平面上生成一个有结构的拓扑映射。
 * 该映射能够保留高维输入特征点之间的相对几何位置关系。
 *
 * ### 算法步骤
 * 1. **初始化**：给网格中的每个输出节点赋予一个随机的高维权重向量。
 * 2. **计算距离（正向传播）**：输入一个训练样本，计算它到网格中所有节点的欧氏距离。
 * 3. **竞争（寻找 BMU）**：选择距离最近的节点作为获胜节点（Best Matching Unit, BMU）。
 * 4. **合作与自适应（更新权重）**：更新获胜节点及其邻域内所有节点的权重，使得它们向当前输入样本靠近。
 *    邻域大小 $R$ 和学习率 $\alpha$ 随迭代次数逐渐衰减。
 *
 * 时间复杂度: 每次迭代 $O(N \cdot P \cdot Q \cdot F)$，其中 $N$ 是样本数，$P \times Q$ 是网格大小，$F$ 是特征维度。
 * 空间复杂度: $O(P \cdot Q \cdot F)$。
 *
 * @note
 * 【测试遗留临时文件污染与空向量解引用 Bug 审计与修复】：
 * 1. **单元测试遗留大量 CSV 文件污染 Bug**：在程序运行 `test1()`, `test2()`, `test3()` 时，
 *    会产生 `test1.csv`, `w11.csv`, `w12.csv` 等多达 9 个 CSV 文件。它们残留在 workspace 中，造成代码库污染与 git 变动。
 *    **修复**：在 `main` 函数成功执行测试后，调用 `std::remove` 对产生的 9 个临时测试数据文件进行闭环自动清理。
 * 2. **空 valarray 解引用防卫**：在 `get_min_2d` 中对 `valarray` 调用 `std::min_element`。如果 `X[i]` 大小为 0，
 *    解引用结果迭代器会造成未定义行为。添加了安全大小验证。
 *
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#define _USE_MATH_DEFINES  
#include <algorithm>
#include <array>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <cstring>
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
 * @brief 将一个二维数据矩阵保存到 CSV 文件中
 * @return 0 成功, -1 失败
 */
int save_2d_data(const char *fname,
                 const std::vector<std::valarray<double>> &X) {
    if (X.empty()) return 0;
    size_t num_points = X.size();       
    size_t num_features = X[0].size();  

    std::ofstream fp;
    fp.open(fname);
    if (!fp.is_open()) {
        std::cerr << "Error opening file " << fname << ": "
                  << std::strerror(errno) << "\n";
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

/**
 * @brief 查找二维矩阵中的最小值及其对应的行列索引
 * @param X 待查找的矩阵
 * @param val [out] 输出最小值
 * @param x_idx [out] 行索引
 * @param y_idx [out] 列索引
 */
void get_min_2d(const std::vector<std::valarray<double>> &X, double *val,
                int *x_idx, int *y_idx) {
    val[0] = INFINITY;  
    size_t N = X.size();

    for (size_t i = 0; i < N; i++) {  
        // 核心修复：防卫性空数组校验，规避 std::min_element 空指针解引用崩溃
        if (X[i].size() == 0) {
            continue;
        }
        auto result = std::min_element(std::begin(X[i]), std::end(X[i]));
        double d_min = *result;
        std::ptrdiff_t j = std::distance(std::begin(X[i]), result);

        if (d_min < val[0]) {  
            x_idx[0] = static_cast<int>(i);
            y_idx[0] = static_cast<int>(j);
            val[0] = d_min;
        }
    }
}

namespace machine_learning {
/** 最小阈值，用于判定收敛 */
constexpr double MIN_DISTANCE = 1e-4;

/**
 * @brief 计算 U-matrix 并将其写入磁盘文件
 * @return 0 成功, -1 失败
 */
int save_u_matrix(const char *fname,
                  const std::vector<std::vector<std::valarray<double>>> &W) {
    std::ofstream fp(fname);
    if (!fp) {  
        std::cerr << "File error (" << fname << "): " << std::strerror(errno)
                  << std::endl;
        return -1;
    }

    unsigned int R = 1;

    for (size_t i = 0; i < W.size(); i++) {         
        for (size_t j = 0; j < W[0].size(); j++) {  
            double distance = 0.f;

            int from_x = std::max<int>(0, static_cast<int>(i) - R);
            int to_x = std::min<int>(W.size(), static_cast<int>(i) + R + 1);
            int from_y = std::max<int>(0, static_cast<int>(j) - R);
            int to_y = std::min<int>(W[0].size(), static_cast<int>(j) + R + 1);
            int l = 0, m = 0;
#ifdef _OPENMP
#pragma omp parallel for reduction(+ : distance) private(l, m)
#endif
            for (l = from_x; l < to_x; l++) {      
                for (m = from_y; m < to_y; m++) {  
                    auto d = W[i][j] - W[l][m];
                    double d2 = std::pow(d, 2).sum();
                    distance += std::sqrt(d2);
                }
            }

            distance /= (R * R);          
            fp << distance;             
            if (j < W[0].size() - 1) {  
                fp << ',';              
            }
        }
        if (i < W.size() - 1) {  
            fp << '\n';          
        }
    }

    fp.close();
    return 0;
}

/**
 * @brief 使用 Kohonen 学习规则更新邻域内节点的权重
 * @param X 传入的单个样本特征向量
 * @param W [in,out] 权重张量
 * @param D 临时距离缓存表
 * @param alpha 当前学习率
 * @param R 当前邻域半径
 * @return 样本到获胜节点的欧氏距离
 */
double update_weights(const std::valarray<double> &X,
                      std::vector<std::vector<std::valarray<double>>> *W,
                      std::vector<std::valarray<double>> *D, double alpha,
                      int R) {
    int x = 0, y = 0;
    int num_out_x = static_cast<int>(W->size());       
    int num_out_y = static_cast<int>((*W)[0].size());  
    double d_min = 0.f;

#ifdef _OPENMP
#pragma omp parallel for private(x, y)
#endif
    for (x = 0; x < num_out_x; x++) {
        for (y = 0; y < num_out_y; y++) {
            (*D)[x][y] = 0.f;
            auto d = ((*W)[x][y] - X);
            (*D)[x][y] = (d * d).sum();
            (*D)[x][y] = std::sqrt((*D)[x][y]);
        }
    }

    // 寻找 BMU 节点
    int d_min_x = 0, d_min_y = 0;
    get_min_2d(*D, &d_min, &d_min_x, &d_min_y);

    int from_x = std::max(0, d_min_x - R);
    int to_x = std::min(num_out_x, d_min_x + R + 1);
    int from_y = std::max(0, d_min_y - R);
    int to_y = std::min(num_out_y, d_min_y + R + 1);

    // 调整邻域内节点权重
#ifdef _OPENMP
#pragma omp parallel for private(x, y)
#endif
    for (x = from_x; x < to_x; x++) {
        for (y = from_y; y < to_y; y++) {
            double d2 =
                (d_min_x - x) * (d_min_x - x) + (d_min_y - y) * (d_min_y - y);
            double scale_factor = std::exp(-d2 / (2.f * alpha * alpha));

            (*W)[x][y] += (X - (*W)[x][y]) * alpha * scale_factor;
        }
    }
    return d_min;
}

/**
 * @brief 自组织映射网络主算法
 */
void kohonen_som(const std::vector<std::valarray<double>> &X,
                 std::vector<std::vector<std::valarray<double>>> *W,
                 double alpha_min) {
    size_t num_samples = X.size();  
    size_t num_out = W->size();  
    size_t R = num_out >> 2, iter = 0;
    double alpha = 1.f;

    std::vector<std::valarray<double>> D(num_out);
    for (size_t i = 0; i < num_out; i++) {
        D[i] = std::valarray<double>(num_out);
    }

    double dmin = 1.f;        
    double past_dmin = 1.f;   
    double dmin_ratio = 1.f;  

    for (; alpha > 0 && dmin_ratio > 1e-5; alpha -= 1e-4, iter++) {
        for (size_t sample = 0; sample < num_samples; sample++) {
            dmin += update_weights(X[sample], W, &D, alpha, static_cast<int>(R));
        }

        if (iter % 300 == 0 && R > 1) {
            R--;
        }

        dmin /= num_samples;

        dmin_ratio = (past_dmin - dmin) / past_dmin;
        if (dmin_ratio < 0) {
            dmin_ratio = 1.f;
        }
        past_dmin = dmin;

        std::cout << "iter: " << iter << "\t alpha: " << alpha << "\t R: " << R
                  << "\t d_min: " << dmin_ratio << "\r";
    }
    std::cout << "\n";
}

}  // namespace machine_learning

using machine_learning::kohonen_som;
using machine_learning::save_u_matrix;

/**
 * @brief 生成 4 个空间二维聚类的随机特征数据
 */
void test_2d_classes(std::vector<std::valarray<double>> *data) {
    const size_t N = data->size();
    const double R = 0.3;  
    const int num_classes = 4;
    std::array<std::array<double, 2>, num_classes> centres = {
        std::array<double, 2>({.5, .5}),   
        std::array<double, 2>({.5, -.5}),  
        std::array<double, 2>({-.5, .5}),  
        std::array<double, 2>({-.5, -.5})  
    };

    for (size_t i = 0; i < N; i++) {
        int cls = std::rand() % num_classes;
        (*data)[i][0] = _random(centres[cls][0] - R, centres[cls][0] + R);
        (*data)[i][1] = _random(centres[cls][1] - R, centres[cls][1] + R);
    }
}

/**
 * @brief 测试 1：2维空间的 Kohonen SOM
 */
void test1() {
    int N = 300;
    int features = 2;
    int num_out = 30;
    std::vector<std::valarray<double>> X(N);
    std::vector<std::vector<std::valarray<double>>> W(num_out);
    for (int i = 0; i < std::max(num_out, N); i++) {
        if (i < N) {  
            X[i] = std::valarray<double>(features);
        }
        if (i < num_out) {  
            W[i] = std::vector<std::valarray<double>>(num_out);
            for (int k = 0; k < num_out; k++) {
                W[i][k] = std::valarray<double>(features);
                for (int j = 0; j < features; j++) {
                    W[i][k][j] = _random(-10, 10);
                }
            }
        }
    }

    test_2d_classes(&X);  
    save_2d_data("test1.csv", X);  
    save_u_matrix("w11.csv", W);   
    kohonen_som(X, &W, 1e-4);      
    save_u_matrix("w12.csv", W);   
}

/**
 * @brief 生成 4 个空间三维聚类的随机特征数据
 */
void test_3d_classes1(std::vector<std::valarray<double>> *data) {
    const size_t N = data->size();
    const double R = 0.3;  
    const int num_classes = 4;
    const std::array<std::array<double, 3>, num_classes> centres = {
        std::array<double, 3>({.5, .5, .5}),    
        std::array<double, 3>({.5, -.5, -.5}),  
        std::array<double, 3>({-.5, .5, .5}),   
        std::array<double, 3>({-.5, -.5 - .5})  
    };

    for (size_t i = 0; i < N; i++) {
        int cls = std::rand() % num_classes;
        (*data)[i][0] = _random(centres[cls][0] - R, centres[cls][0] + R);
        (*data)[i][1] = _random(centres[cls][1] - R, centres[cls][1] + R);
        (*data)[i][2] = _random(centres[cls][2] - R, centres[cls][2] + R);
    }
}

/**
 * @brief 测试 2：3维空间（4个聚类）的 Kohonen SOM
 */
void test2() {
    int N = 300;
    int features = 3;
    int num_out = 30;
    std::vector<std::valarray<double>> X(N);
    std::vector<std::vector<std::valarray<double>>> W(num_out);
    for (int i = 0; i < std::max(num_out, N); i++) {
        if (i < N) {  
            X[i] = std::valarray<double>(features);
        }
        if (i < num_out) {  
            W[i] = std::vector<std::valarray<double>>(num_out);
            for (int k = 0; k < num_out; k++) {
                W[i][k] = std::valarray<double>(features);
                for (int j = 0; j < features; j++) {
                    W[i][k][j] = _random(-10, 10);
                }
            }
        }
    }

    test_3d_classes1(&X);  
    save_2d_data("test2.csv", X);  
    save_u_matrix("w21.csv", W);   
    kohonen_som(X, &W, 1e-4);      
    save_u_matrix("w22.csv", W);   
}

/**
 * @brief 生成 8 个空间三维聚类的随机特征数据
 */
void test_3d_classes2(std::vector<std::valarray<double>> *data) {
    const size_t N = data->size();
    const double R = 0.2;  
    const int num_classes = 8;
    const std::array<std::array<double, 3>, num_classes> centres = {
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
 * @brief 测试 3：3维空间（8个聚类）的 Kohonen SOM
 */
void test3() {
    int N = 500;
    int features = 3;
    int num_out = 30;
    std::vector<std::valarray<double>> X(N);
    std::vector<std::vector<std::valarray<double>>> W(num_out);
    for (int i = 0; i < std::max(num_out, N); i++) {
        if (i < N) {  
            X[i] = std::valarray<double>(features);
        }
        if (i < num_out) {  
            W[i] = std::vector<std::valarray<double>>(num_out);
            for (int k = 0; k < num_out; k++) {
                W[i][k] = std::valarray<double>(features);
                for (int j = 0; j < features; j++) {
                    W[i][k][j] = _random(-10, 10);
                }
            }
        }
    }

    test_3d_classes2(&X);  
    save_2d_data("test3.csv", X);  
    save_u_matrix("w31.csv", W);   
    kohonen_som(X, &W, 1e-4);      
    save_u_matrix("w32.csv", W);   
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
