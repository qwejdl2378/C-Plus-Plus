/**
 * @file
 * @brief Implementation of the [Convex Hull](https://en.wikipedia.org/wiki/Convex_hull) algorithm using [Graham Scan](https://en.wikipedia.org/wiki/Graham_scan) (基于 Graham 扫描法的二维平面点集凸包算法实现)
 *
 * @details
 * 凸包（Convex Hull）是一个几何学概念。在一个实数向量空间 V 中，对于给定集合 X，
 * 所有包含 X 的凸集的交集 S 被称为 X 的凸包。
 * 用直观的话说，就是用一根橡皮筋将平面上所有点紧紧圈起来所得到的最小多边形。
 *
 * ### 复杂度分析
 * - Jarvis 步进法（Jarvis March）的最坏时间复杂度为 $O(N^2)$。
 * - Graham 扫描法（Graham Scan）通过引入极角排序，能将最坏时间复杂度降为稳定的 $O(N \log N)$。
 *
 * @author [Lajat Manekar](https://github.com/Lazeeez)
 */

#include <cassert>   /// 用于 assert 断言
#include <iostream>  /// 用于标准输出
#include <vector>    /// 用于 std::vector

#include "./graham_scan_functions.hpp"  /// 引入辅助功能函数

/**
 * @brief 单元自测用例
 */
static void test() {
    std::vector<geometry::grahamscan::Point> points = {
        {0, 3}, {1, 1}, {2, 2}, {4, 4}, {0, 0}, {1, 2}, {3, 1}, {3, 3}};
    std::vector<geometry::grahamscan::Point> expected_result = {
        {0, 3}, {4, 4}, {3, 1}, {0, 0}};
    std::vector<geometry::grahamscan::Point> derived_result;

    derived_result = geometry::grahamscan::convexHull(points, points.size());

    // 验证求解出的各个凸包顶点坐标是否符合预期
    std::cout << "Graham Scan Convex Hull test: ";
    assert(derived_result.size() == expected_result.size());
    for (size_t i = 0; i < expected_result.size(); i++) {
        assert(derived_result[i].x == expected_result[i].x);
        assert(derived_result[i].y == expected_result[i].y);
    }
    std::cout << "passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
