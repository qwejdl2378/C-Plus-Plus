/**
 * @file
 * @brief Bottom-up Implementation of the [Rod Cutting Problem](https://en.wikipedia.org/wiki/Dynamic_programming#Rod_cutting) (钢条切割问题算法实现)
 *
 * @details
 * 给定一根长度为 n 的钢条和一个价格表，价格表包含了所有长度小于等于 n 的钢条的价格。
 * 求切割并销售该钢条所能获得的最大利润。
 *
 * ### 动态规划递推原理（自底向上）
 * 设 `profit[i]` 表示长度为 `i` 的钢条所能获得的最大收益：
 * - 基础状态：`profit[0] = 0`（长度为 0 时收益为 0）
 * - 递推关系：`profit[i] = max(price[j - 1] + profit[i - j])`，其中 1 <= j <= i
 *   这表示第一刀切在长度 j 处（获得收益 `price[j-1]`），剩下 `i-j` 长度的最优收益为 `profit[i-j]`。
 * 
 * @note
 * 【类型截断风险提示】：
 * 在第 61 行中，函数虽然声明返回 `int`，但是内部将结果赋值给了 `const int16_t ans = profit[n];`。
 * `int16_t` 的取值范围仅为 -32768 到 32767。如果钢条长度较大或售价高，最大利润超过 32767 时，
 * 会发生数值截断溢出错误。在工业级开发中应统一使用 `int` 或 `int32_t` / `int64_t`。
 *
 * 时间复杂度: O(N^2)
 * 空间复杂度: O(N)
 *
 * @author [Anmol](https://github.com/Anmol3299)
 * @author [Pardeep](https://github.com/Pardeep009)
 */

#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <iostream>

/**
 * @namespace dynamic_programming
 * @brief 动态规划算法命名空间
 */
namespace dynamic_programming {
/**
 * @namespace cut_rod
 * @brief 钢条切割问题相关命名空间
 */
namespace cut_rod {
/**
 * @brief 计算钢条切割的最大收益
 * @tparam T 价格表数组的大小
 * @param price 价格表数组价格（1-indexed）
 * @param n 钢条总长度
 * @returns 最大收益数值
 */
template <size_t T>
int maxProfitByCuttingRod(const std::array<int, T> &price, const uint64_t &n) {
    int *profit = new int[n + 1];  // profit[i] 保存长度为 i 时的最大利润

    profit[0] = 0;  // 长度为 0 的钢条收益为 0

    // 外层循环：逐步计算长度从 1 到 n 的子问题的最大利润
    // 内层循环：枚举第一刀切下的长度 j，利用子问题 profit[i-j] 递推当前最大值
    for (size_t i = 1; i <= n; i++) {
        int q = INT_MIN;
        for (size_t j = 1; j <= i; j++) {
            q = std::max(q, price[j - 1] + profit[i - j]);
        }
        profit[i] = q;
    }
    const int16_t ans = profit[n]; // 警告：这里使用 int16_t 容易导致溢出
    delete[] profit;
    return ans;
}
}  // namespace cut_rod
}  // namespace dynamic_programming

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1：长度为 8，预期最大利润 47
    const int16_t n1 = 8;
    std::array<int32_t, n1> price1 = {1, 2, 4, 6, 8, 45, 21, 9};
    const int64_t max_profit1 =
        dynamic_programming::cut_rod::maxProfitByCuttingRod(price1, n1);
    const int64_t expected_max_profit1 = 47;
    assert(max_profit1 == expected_max_profit1);
    std::cout << "Maximum profit with " << n1 << " inch road is " << max_profit1
              << std::endl;

    // 测试 2：长度为 30，预期最大利润 90
    const int16_t n2 = 30;
    std::array<int32_t, n2> price2 = {
        1,  5,  8,  9,  10, 17, 17, 20, 24, 30,
        31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50};

    const int64_t max_profit2 =
        dynamic_programming::cut_rod::maxProfitByCuttingRod(price2, n2);
    const int32_t expected_max_profit2 = 90;
    assert(max_profit2 == expected_max_profit2);
    std::cout << "Maximum profit with " << n2 << " inch road is " << max_profit2
              << std::endl;

    // 测试 3：长度为 5，预期最大利润 45
    const int16_t n3 = 5;
    std::array<int32_t, n3> price3 = {2, 9, 17, 23, 45};
    const int64_t max_profit3 =
        dynamic_programming::cut_rod::maxProfitByCuttingRod(price3, n3);
    const int64_t expected_max_profit3 = 45;
    assert(max_profit3 == expected_max_profit3);
    std::cout << "Maximum profit with " << n3 << " inch road is " << max_profit3
              << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行自测
    return 0;
}
