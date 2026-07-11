/**
 * @file
 * @brief Implementation of the [Jump Game](https://leetcode.com/problems/jump-game/) algorithm (跳跃游戏贪心算法实现)
 *
 * @details
 * 给定一个非负整数数组 `nums`，你最初位于数组的第一个位置。
 * 数组中的每个元素代表你在该位置可以跳跃的最大长度。判断你是否能够到达最后一个位置。
 *
 * ### 贪心算法思想
 * 1. 我们的目标是判断能否到达数组的终点。
 * 2. 我们可以从数组的**末尾**开始反向进行贪心推理：
 *    - 我们用 `lastPos` 记录能到达终点的最左端“安全位置”，初始时 `lastPos = nums.size() - 1`。
 *    - 我们从后往前遍历数组。如果当前位置 `i` 加上其能够跳跃的最大步数 `nums[i]` 大于或等于 `lastPos`：
 *      说明从位置 `i` 可以跳到当前已知的安全位置，然后再跳到终点。
 *      因此，我们贪心地将安全位置前移到当前位置：`lastPos = i`。
 * 3. 遍历结束，如果 `lastPos == 0`，说明从起点位置（索引 0）出发可以顺利跳跃至终点。
 *
 * 时间复杂度: $O(N)$ (单趟反向扫描数组)
 * 空间复杂度: $O(1)$ (仅使用常数级额外空间)
 *
 * @note
 * 【空数组输入下溢越界 Bug 审计与修复】：
 * 1. **无符号整数下溢隐患**：原代码在没有对空 vector 进行检验的情况下，
 *    直接计算 `size_t lastPos = nums.size() - 1;`。
 *    当输入数组为空（`nums.size() == 0`）时，`0 - 1` 会产生**无符号整数下溢（Unsigned Underflow）**，
 *    变为最大的无符号数（如 $2^{64}-1$）。尽管循环条件碰巧使其没有执行循环体，但这依然是极不规范的代码隐患。
 *    **修复**：在函数入口处添加防御性校验 `if (nums.empty()) return false;`。
 *
 * @author [Rakshaa Viswanathan](https://github.com/rakshaa2000)
 * @author [David Leal](https://github.com/Panquesito7)
 */

#include <cassert>   /// 用于 assert 断言
#include <iostream>  /// 用于标准输出
#include <vector>    /// 用于 std::vector

/**
 * @namespace greedy_algorithms
 * @brief 贪心算法命名空间
 */
namespace greedy_algorithms {

/**
 * @brief 判断是否能跳跃至数组的最后一个索引
 * @param nums 记录各个位置最大跳跃步数的数组
 * @return true 能到达终点
 * @return false 不能到达终点
 */
bool can_jump(const std::vector<int> &nums) {
    // 核心修复：空安全防护，防止无符号整数 0-1 下溢
    if (nums.empty()) {
        return false;
    }

    size_t lastPos = nums.size() - 1; // 记录能到达终点的最靠左的安全位置
    
    // 从后往前反向遍历数组
    // 使用递减的无符号整型循环，判断 i 减到 0 之后减 1 下溢（变为最大值）作为终结标志
    for (size_t i = lastPos; i != static_cast<size_t>(-1); i--) {
        // 如果从当前位置能够到达或越过已确定的最左安全位置 lastPos
        if (i + nums[i] >= lastPos) {
            lastPos = i; // 贪心收缩，将当前位置更新为新的安全中转站
        }
    }
    
    return lastPos == 0; // 若最左安全位置退回到了起点 0，则说明可达
}
}  // namespace greedy_algorithms

/**
 * @brief 单元自测用例
 */
static void test() {
    // 预期可达
    assert(greedy_algorithms::can_jump(std::vector<int>({4, 3, 1, 0, 5})));
    // 预期不可达（0 形成断层）
    assert(!greedy_algorithms::can_jump(std::vector<int>({3, 2, 1, 0, 4})));
    // 预期可达（大跳越过断点）
    assert(greedy_algorithms::can_jump(std::vector<int>({5, 9, 4, 7, 15, 3})));
    // 预期不可达
    assert(!greedy_algorithms::can_jump(std::vector<int>({1, 0, 5, 8, 12})));
    // 预期可达
    assert(greedy_algorithms::can_jump(std::vector<int>({2, 1, 4, 7})));
    // 边界情况：空数组
    assert(!greedy_algorithms::can_jump(std::vector<int>({})));
    // 边界情况：只有一个元素
    assert(greedy_algorithms::can_jump(std::vector<int>({0})));

    std::cout << "All Jump Game tests passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试用例
    return 0;
}
