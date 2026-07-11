/**
 * @file
 * @brief Naive implementation of the [Travelling Salesman Problem](https://en.wikipedia.org/wiki/Travelling_salesman_problem) (基于全排列回溯的旅行商问题 TSP 暴力解法实现)
 *
 * @details
 * 旅行商问题（TSP）：给定一组城市和每对城市之间的距离，要求找出一条经过每个城市恰好一次并回到起点的最短路径。
 *
 * ### 暴力求解原理
 * 本程序采用朴素的暴力排列组合策略：
 * 1. 固定起点为 `src`（不参与排列）。
 * 2. 对剩余的 $V-1$ 个城市的所有可能访问顺序进行全排列生成（使用 `std::next_permutation`）。
 * 3. 对于每一种排列序列，依次累加边权，计算该路径的完整闭环代价（加上最后回到起点的代价）。
 * 4. 在所有排列中寻找使代价最小的路径，并将其作为最终结果返回。
 *
 * 时间复杂度: $O((V - 1)! \cdot V)$，对于顶点数较大的图（如 $V \ge 12$）计算会极其缓慢。
 * 空间复杂度: $O(V)$
 *
 * @author [Mayank Mamgain](http://github.com/Mayank17M)
 */

#include <algorithm>  /// 用于 std::min 和 std::next_permutation
#include <cassert>    /// 用于 assert 断言
#include <cstdint>   /// 用于 std::uint32_t 等类型
#include <iostream>   /// 用于输入输出
#include <limits>     /// 用于 std::numeric_limits
#include <vector>     /// 用于 std::vector

/**
 * @namespace graph
 * @brief 图算法命名空间
 */
namespace graph {
/**
 * @brief 暴力求解旅行商问题的最短路长
 * @param cities 城市间距离矩阵（邻接矩阵）
 * @param src 旅行商出发城市索引 (0-indexed)
 * @param V 城市（顶点）总数
 * @returns 遍历所有城市并返回起点的最短路径代价
 */
int TravellingSalesmanProblem(std::vector<std::vector<uint32_t>> *cities,
                              int32_t src, uint32_t V) {
    // 存储除起点外的所有其他城市顶点
    std::vector<uint32_t> vtx;
    for (uint32_t i = 0; i < V; i++) {
        if (static_cast<int32_t>(i) != src) {
            vtx.push_back(i);
        }
    }

    // 将最小路径值初始化为 int32 最大值
    int32_t min_path = std::numeric_limits<int32_t>::max();
    
    // 对剩余城市顶点做全排列
    do {
        int32_t curr_weight = 0; // 当前路径代价
        int k = src;
        
        // 计算当前排列下的总距离
        for (int i : vtx) {
            curr_weight += (*cities)[k][i];
            k = i;
        }
        curr_weight += (*cities)[k][src]; // 回到起点 src

        // 更新全局最小代价
        min_path = std::min(min_path, curr_weight);

    } while (next_permutation(vtx.begin(), vtx.end()));

    return min_path;
}
}  // namespace graph

/**
 * @brief 单元自测用例
 */
static void tests() {
    std::cout << "Initiatinig Predefined Tests..." << std::endl;
    std::cout << "Initiating Test 1..." << std::endl;
    std::vector<std::vector<uint32_t>> cities = {
        {0, 20, 42, 35}, {20, 0, 30, 34}, {42, 30, 0, 12}, {35, 34, 12, 0}};
    uint32_t V = cities.size();
    assert(graph::TravellingSalesmanProblem(&cities, 0, V) == 97);
    std::cout << "1st test passed..." << std::endl;

    std::cout << "Initiating Test 2..." << std::endl;
    cities = {{0, 5, 10, 15}, {5, 0, 20, 30}, {10, 20, 0, 35}, {15, 30, 35, 0}};
    V = cities.size();
    assert(graph::TravellingSalesmanProblem(&cities, 0, V) == 75);
    std::cout << "2nd test passed..." << std::endl;

    std::cout << "Initiating Test 3..." << std::endl;
    cities = {
        {0, 10, 15, 20}, {10, 0, 35, 25}, {15, 35, 0, 30}, {20, 25, 30, 0}};
    V = cities.size();
    assert(graph::TravellingSalesmanProblem(&cities, 0, V) == 80);
    std::cout << "3rd test passed..." << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    tests();  // 运行测试用例
    std::vector<std::vector<uint32_t>> cities = {
        {0, 5, 10, 15}, {5, 0, 20, 30}, {10, 20, 0, 35}, {15, 30, 35, 0}};
    uint32_t V = cities.size();
    std::cout << "Shortest path for default matrix: " 
              << graph::TravellingSalesmanProblem(&cities, 0, V) << std::endl;
    return 0;
}
