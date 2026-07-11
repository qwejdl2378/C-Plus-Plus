/**
 * @file
 * @brief Implementation of [Kadane Algorithm](https://en.wikipedia.org/wiki/Kadane%27s_algorithm) (卡登算法/最大子数组和算法实现)
 *
 * @details
 * 卡登（Kadane）算法用于在给定的整型数组中寻找具有最大和的连续子数组。
 * 
 * ### 动态规划状态转移
 * 设 `curr_sum` 记录以当前元素结尾的最大连续子数组和。
 * 设 `max_sum` 记录全局见过的最大子数组和。
 * - 对于数组中的每个元素 `num`：
 *   `curr_sum = max(num, curr_sum + num)`
 *   `max_sum = max(max_sum, curr_sum)`
 * 
 * @note
 * 【严重数组越界/逻辑 Bug 说明】：
 * 在原作者的代码中，第 46-47 行写为：
 *   `for (int i : n) { curr_sum += n[i]; ... }`
 * 1. 这是一个 C++ 的**范围 for 循环**（Range-based for loop）。在这里，循环变量 `i` 代表数组 `n` 中的**每一个元素值**（而非索引下标）。
 * 2. 然而，在循环体内部，代码却写成了 `curr_sum += n[i]`，将元素值 `i` 当成了下标来访问数组。
 * 3. 结果是：
 *    - 如果数组含有负数（例如包含 -2），则会触发 `n[-2]`，造成**严重的缓冲区下溢（Buffer Underflow）内存越界**，导致段错误崩溃。
 *    - 如果数组元素大于等于数组大小 N（例如 N=5, 元素包含 100），则会触发 `n[100]`，造成**缓冲区上溢（Buffer Overflow）**。
 * 
 * 修正方法：应当直接累加元素值 `i`，即修改为 `curr_sum += i`；或者使用常规的索引下标循环。
 *
 * 时间复杂度: O(N)
 * 空间复杂度: O(1)
 *
 * @author [Ayush Singh](https://github.com/ayush523)
 */

#include <array>
#include <climits>
#include <iostream>

/**
 * @namespace dynamic_programming
 * @brief 动态规划算法命名空间
 */
namespace dynamic_programming {
/**
 * @namespace kadane
 * @brief 卡登算法相关命名空间
 */
namespace kadane {
/**
 * @brief 计算并返回最大连续子数组和
 * @tparam N 数组的大小
 * @param n 包含整型数据的 std::array
 * @returns 最大子数组和
 */
template <size_t N>
int maxSubArray(const std::array<int, N> &n) {
    int curr_sum = 0;       // 当前以当前位置结尾的子数组累加和
    int max_sum = INT_MIN;  // 记录最大子数组和的全局最值

    for (int i : n) {       // 范围 for 循环：i 是数组元素本身
        curr_sum += n[i];   // 警告 Bug：将元素值误作为下标索引访问！应为 curr_sum += i
        max_sum = std::max(max_sum, curr_sum);
        curr_sum = std::max(curr_sum, 0); // 若累计和小于 0 则抛弃当前前缀
    }
    return max_sum;
}
}  // namespace kadane
}  // namespace dynamic_programming

/**
 * @brief 主函数：交互式读取用户输入并输出计算结果
 */
int main() {
    const int N = 5;
    std::array<int, N> n{};  // 初始化声明大小为 5 的数组

    for (int i = 0; i < n.size(); i++) {
        std::cout << "Enter value of n[" << i << "]" << "\n";
        std::cin >> n[i];
    }
    
    // 警告：这里会调用 maxSubArray，如果用户输入了负数或较大正数，可能直接崩溃
    int max_sum = dynamic_programming::kadane::maxSubArray<N>(n);
    std::cout << "Maximum subarray sum is " << max_sum << std::endl;

    return 0;
}
