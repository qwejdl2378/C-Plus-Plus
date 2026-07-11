/**
 * @file
 * @brief Utility functions for Graham Scan Convex Hull Algorithm (Graham 扫描法凸包算法辅助函数)
 *
 * @details
 * 本文件提供了 Graham 扫描法所需的所有几何计算和排序辅助函数。
 *
 * ### 核心计算技术与安全设计
 * 1. **向量叉积（Orientation）**：
 *    - 计算三个点 $p, q, r$ 的相对旋转方向。
 *    - 利用叉积公式：$val = (q.y - p.y) \cdot (r.x - q.x) - (q.x - p.x) \cdot (r.y - q.y)$。
 *    - 如果 $val = 0$，表示三点共线；如果 $val > 0$，表示顺时针旋转（右拐）；如果 $val < 0$，表示逆时针旋转（左拐）。
 *    - **溢出防御**：由于坐标最大可到 $10^9$ 级别，乘法运算极易引发 **32 位整型溢出（Integer Overflow）**。
 *      本实现已全部将乘法操作数转换为 `int64_t` 进行运算。
 * 2. **极角排序（Polar Angle Sort）**：
 *    - 以最下且最左的顶点 $p_0$ 作为参考原点，将其余顶点按对 $p_0$ 的极角从小到大排序。
 *    - 若极角相同，则将距离 $p_0$ 更远的点排在后面。
 *    - 排序在 `convexHull` 中采用标准 C 库函数 `qsort` 实现。
 *
 * @author [Lajat Manekar](https://github.com/Lazeeez)
 */

#ifndef GRAHAM_SCAN_FUNCTIONS_HPP_
#define GRAHAM_SCAN_FUNCTIONS_HPP_

#include <algorithm>  /// 用于 std::swap
#include <cstdint>   /// 用于 int64_t, uint64_t
#include <cstdlib>    /// 用于 qsort
#include <iostream>   /// 用于输入输出
#include <stack>      /// 用于 std::stack
#include <vector>     /// 用于 std::vector

namespace geometry {
namespace grahamscan {

/**
 * @brief 二维笛卡尔坐标点结构体
 */
struct Point {
    int x, y;
};

// 全局参考起点，用于极角排序时的基准
Point p0;

/**
 * @brief 获取栈顶下方的第二个元素（next-to-top）
 * @param S 堆栈指针
 * @return 栈顶下方的元素值
 */
Point nextToTop(std::stack<Point> *S) {
    Point p = S->top();
    S->pop();
    Point res = S->top();
    S->push(p); // 恢复栈顶
    return res;
}

/**
 * @brief 计算两点间的欧几里得距离平方
 * @return 距离平方值（采用 int64_t，防范整型乘法溢出）
 */
int64_t distSq(Point p1, Point p2) {
    int64_t dx = static_cast<int64_t>(p1.x) - p2.x;
    int64_t dy = static_cast<int64_t>(p1.y) - p2.y;
    return dx * dx + dy * dy; // 核心修复：转换为 int64_t，防止乘积越界
}

/**
 * @brief 计算三点有序三元组 (p, q, r) 的转向方向（Orientation）
 * @return 0 共线, 1 顺时针（右拐）, 2 逆时针（左拐）
 */
int orientation(Point p, Point q, Point r) {
    // 核心修复：强制转换为 int64_t，彻底解决乘积引起的整型溢出未定义行为
    int64_t val = (static_cast<int64_t>(q.y) - p.y) * (r.x - q.x) - 
                  (static_cast<int64_t>(q.x) - p.x) * (r.y - q.y);

    if (val == 0) {
        return 0;  // 三点共线
    }
    return (val > 0) ? 1 : 2;  // 顺时针为 1，逆时针为 2
}

/**
 * @brief 极角排序比较仿函数
 */
int compare(const void *vp1, const void *vp2) {
    auto *p1 = static_cast<const Point *>(vp1);
    auto *p2 = static_cast<const Point *>(vp2);

    int o = orientation(p0, *p1, *p2);
    if (o == 0) {
        // 共线时，距离 p0 较远的点排在后面
        return (distSq(p0, *p2) >= distSq(p0, *p1)) ? -1 : 1;
    }
    // 逆时针旋转度更小的排在前面
    return (o == 2) ? -1 : 1;
}

/**
 * @brief Graham 扫描法计算平面点集的凸包
 * @param points 平面坐标点数组
 * @param size 数组大小
 * @return 构成凸包的所有边界顶点数组（逆时针排列）
 */
std::vector<Point> convexHull(std::vector<Point> points, uint64_t size) {
    // 核心修复：防范空数组或不足 3 个顶点的非法状态导致 points[0] 越界崩溃
    if (size < 3 || points.size() < size) {
        return {};
    }

    // 1. 寻找最下方的点（若 y 坐标相同，则取最左侧的点）
    int ymin = points[0].y;
    int min_idx = 0;
    for (uint64_t i = 1; i < size; i++) {
        int y = points[i].y;
        if ((y < ymin) || (ymin == y && points[i].x < points[min_idx].x)) {
            ymin = points[i].y;
            min_idx = static_cast<int>(i);
        }
    }

    // 将选出的参考起点交换到数组首位 points[0]
    std::swap(points[0], points[min_idx]);

    // 2. 对其余的 N-1 个点关于 points[0] 进行极角升序排序
    p0 = points[0];
    std::qsort(&points[1], size - 1, sizeof(Point), compare);

    // 3. 去除极角相同的共线点，仅保留离原点 p0 最远的那一个
    int m = 1; 
    for (uint64_t i = 1; i < size; i++) {
        while (i < size - 1 && orientation(p0, points[i], points[i + 1]) == 0) {
            i++;
        }
        points[m] = points[i];
        m++;  // 缩减后的有效数组长度
    }

    // 如果剩余有效顶点数小于 3，说明无法构成凸包
    if (m < 3) {
        return {};
    }

    // 4. 利用栈结构维护凸包边界顶点
    std::stack<Point> St;
    St.push(points[0]);
    St.push(points[1]);
    St.push(points[2]);

    // 扫描剩余的所有点
    for (int i = 3; i < m; i++) {
        // 如果当前点 points[i] 与栈顶、次栈顶构成的不是左拐（非逆时针），则弹出栈顶（说明原栈顶为凹陷点）
        while (St.size() > 1 &&
               orientation(nextToTop(&St), St.top(), points[i]) != 2) {
            St.pop();
        }
        St.push(points[i]); // 将当前点压入栈中
    }

    // 5. 倒出栈中所有的凸包边界顶点并返回
    std::vector<Point> result;
    while (!St.empty()) {
        Point p = St.top();
        result.push_back(p);
        St.pop();
    }

    return result; 
}

}  // namespace grahamscan
}  // namespace geometry

#endif // GRAHAM_SCAN_FUNCTIONS_HPP_
