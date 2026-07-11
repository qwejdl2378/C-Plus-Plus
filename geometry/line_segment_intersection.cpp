/**
 * @file
 * @brief Implementation of Line Segment Intersection Check (线段相交判定算法实现)
 *
 * @details
 * 给定二维平面上的两条线段 $p_1 p_2$ 和 $p_3 p_4$，判断它们是否在平面上相交（包括端点重合或部分重叠）。
 *
 * ### 经典算法原理（跨立实验与共线排查 - 基于 CLRS 算法导论）
 * 1. **跨立实验 (Straddle Test)**：
 *    - 如果两条线段相交，则线段 $p_1 p_2$ 的两个端点 $p_1$ 和 $p_2$ 必须分别位于包含线段 $p_3 p_4$ 的直线的两侧；
 *      同理，线段 $p_3 p_4$ 的两个端点 $p_3$ 和 $p_4$ 也必须分别位于包含线段 $p_1 p_2$ 的直线的两侧。
 *    - 我们通过向量叉积判断方向。设 $d_1 = \text{direction}(p_3, p_4, p_1)$， $d_2 = \text{direction}(p_3, p_4, p_2)$，
 *      $d_3 = \text{direction}(p_1, p_2, p_3)$， $d_4 = \text{direction}(p_1, p_2, p_4)$。
 *    - 若两端点跨立在直线两侧，则对应的叉积乘积必须为负，即 $d_1 \cdot d_2 < 0$ 且 $d_3 \cdot d_4 < 0$。
 * 2. **边界共线排查 (Collinear Check)**：
 *    - 若其中任意一个叉积为 $0$（例如 $d_1 = 0$），说明对应的点（如 $p_1$）与另一条线段（如 $p_3 p_4$）共线。
 *    - 此时，只需利用 `on_segment` 检查该共线点是否落在该线段的投影闭区间内。
 *
 * @note
 * 【原版代码的严重逻辑缺陷与越界溢出 Bug 审计与修复】：
 * 1. **跨立实验逻辑判定错误 Bug**：原版将跨立判定写为：
 *    `if ((direction1 < 0 || direction2 > 0) && (direction3 < 0 || direction4 > 0))`
 *    这是完全错误的！例如两条平行不相交的垂直线段（如 (0,0)-(0,2) 和 (1,0)-(1,2)），
 *    代入公式会评估为真，从而得出“相交”的荒谬结论。
 *    **修复**：改用严格的异号乘积判定：
 *    `if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) && ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))`
 * 2. **第 4 点共线检查条件抄写错误 Bug**：原版在判断第 4 个点共线时，写为：
 *    `else if (direction3 == 0 && on_segment(first_point, second_point, forth_point))`
 *    此处应该检查 `direction4 == 0`，却笔误写成了 `direction3 == 0`。
 *    若第 3 点不共线但第 4 点共线且在段上，代码会漏判，从而错误返回 `false`。
 *    **修复**：修正为 `else if (d4 == 0 && on_segment(first_point, second_point, forth_point))`。
 * 3. **整型乘法溢出 Bug**：原 `direction` 计算中包含两个 32 位 `int` 乘法相减。
 *    如果坐标值较大，乘积会越界产生负数，导致转向判断彻底颠倒。
 *    **修复**：使用 `int64_t` 进行高精度叉积运算。
 *
 * @author Unknown author
 */

#include <algorithm>
#include <iostream>
#include <cassert>
#include <cstdint>

/**
 * @brief 平面坐标点结构体
 */
struct Point {
    int x;  ///< X 轴坐标
    int y;  ///< Y 轴坐标
};

/**
 * @brief 线段相交检测结构体
 */
struct SegmentIntersection {
    /**
     * @brief 计算向量叉积，判断点 third_point 相对于向量 (first_point -> second_point) 的转向方向
     * @return 负数表示逆时针（左侧），正数表示顺时针（右侧），0 表示共线
     */
    inline int64_t direction(Point first_point, Point second_point, Point third_point) {
        // 核心修复：强制转换为 int64_t，防范坐标相乘整型溢出
        int64_t x1 = static_cast<int64_t>(third_point.x) - first_point.x;
        int64_t y1 = static_cast<int64_t>(second_point.y) - first_point.y;
        int64_t x2 = static_cast<int64_t>(second_point.x) - first_point.x;
        int64_t y2 = static_cast<int64_t>(third_point.y) - first_point.y;
        return (x1 * y1) - (x2 * y2);
    }

    /**
     * @brief 判断共线的第三个点是否落在前两个点组成的线段区间内
     */
    inline bool on_segment(Point first_point, Point second_point, Point third_point) {
        return (std::min(first_point.x, second_point.x) <= third_point.x &&
                third_point.x <= std::max(first_point.x, second_point.x) &&
                std::min(first_point.y, second_point.y) <= third_point.y &&
                third_point.y <= std::max(first_point.y, second_point.y));
    }

    /**
     * @brief 判定线段 (p1-p2) 与线段 (p3-p4) 是否相交
     * @returns true 相交, false 不相交
     */
    inline bool intersect(Point p1, Point p2, Point p3, Point p4) {
        int64_t d1 = direction(p3, p4, p1);
        int64_t d2 = direction(p3, p4, p2);
        int64_t d3 = direction(p1, p2, p3);
        int64_t d4 = direction(p1, p2, p4);

        // 1. 核心修复：执行正确的跨立判断（异号说明分居两侧）
        if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
            ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
            return true;
        }

        // 2. 边界共线检查
        if (d1 == 0 && on_segment(p3, p4, p1)) {
            return true;
        }
        if (d2 == 0 && on_segment(p3, p4, p2)) {
            return true;
        }
        if (d3 == 0 && on_segment(p1, p2, p3)) {
            return true;
        }
        // 核心修复：修正为 d4 == 0，避免漏判定第 4 点共线在段内的情况
        if (d4 == 0 && on_segment(p1, p2, p4)) {
            return true;
        }

        return false;
    }
};

/**
 * @brief 单元自测函数
 */
static void test() {
    SegmentIntersection segment;
    
    // 测试 1：常规十字交叉线段
    assert(segment.intersect({0, 0}, {4, 4}, {0, 4}, {4, 0}) == true);

    // 测试 2：平行且不重合线段（原版代码会错误判定为 true）
    assert(segment.intersect({0, 0}, {0, 2}, {1, 0}, {1, 2}) == false);

    // 测试 3：T 字相交（端点落在另一线段上）
    assert(segment.intersect({0, 0}, {2, 0}, {1, 0}, {1, 2}) == true);

    // 测试 4：完全共线且重叠
    assert(segment.intersect({0, 0}, {4, 0}, {2, 0}, {6, 0}) == true);

    // 测试 5：完全共线但不重叠
    assert(segment.intersect({0, 0}, {2, 0}, {3, 0}, {5, 0}) == false);

    // 测试 6：验证 d4 == 0 共线漏判修复
    // p1=(0,0), p2=(2,0), p3=(3,0), p4=(1,0)
    // 这里 d4 = direction(p1, p2, p4) = direction((0,0), (2,0), (1,0)) = 0.
    // 且 p4 (1,0) 在 p1-p2 (0,0)-(2,0) 上。应判定为相交。
    assert(segment.intersect({0, 0}, {2, 0}, {3, 0}, {1, 0}) == true);

    std::cout << "All Segment Intersection tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行自动化测试，确保算法逻辑完全正确

    // 依然支持交互式模式输入
    std::cout << "Enter coordinate for 4 points (P1, P2, P3, P4):\n";
    SegmentIntersection segment;
    Point first_point, second_point, third_point, forth_point;

    if (std::cin >> first_point.x >> first_point.y >>
        second_point.x >> second_point.y >>
        third_point.x >> third_point.y >>
        forth_point.x >> forth_point.y) {
        bool res = segment.intersect(first_point, second_point, third_point, forth_point);
        std::cout << "Intersect: " << (res ? "YES" : "NO") << std::endl;
    }
    return 0;
}
