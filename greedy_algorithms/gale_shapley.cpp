/**
 * @file
 * @brief Implementation of the [Gale-Shapley Stable Matching](https://en.wikipedia.org/wiki/Gale%E2%80%93Shapley_algorithm) algorithm (Gale-Shapley 稳定婚姻匹配算法实现)
 *
 * @details
 * Gale-Shapley 算法是一个著名的贪心算法与博弈论结合的代表。它解决了著名的“稳定婚姻问题”。
 *
 * ### 算法思想 (延迟认可 - Deferred Acceptance)
 * 1. 初始时，所有主要集合（Primary Set，如男方）与次要集合（Secondary Set，如女方）的成员都是单身状态。
 * 2. 局部贪心策略：每一轮，让某一个单身的主要集合成员向其心仪名单上排序最高但尚未拒绝过他的次要集合成员发起“求婚”。
 * 3. 延迟认可：次要集合成员收到求婚后：
 *    - 如果她当前也是单身，则暂时接受匹配。
 *    - 如果她已经有配偶，则与其当前配偶和新求婚者进行偏好对比。她会局部贪心地选择她更心仪的那一个，并拒绝另一个人。被拒绝的人恢复单身状态。
 * 4. 迭代直到没有单身主要成员可发起匹配，匹配收敛并达成“稳定匹配”状态（不存在匹配双方都更愿意与他人私奔的情况）。
 *
 * 时间复杂度: $O(N^2)$ (最多进行 $N^2$ 次求婚)
 * 空间复杂度: $O(N)$ (存储匹配状态和求婚进度)
 *
 * @note
 * 【C++ 无符号整型与负值哨兵隐式转换 Bug 审计与修复】：
 * 1. **哨兵比较隐式转换警告**：原代码中声明 `std::vector<std::uint32_t> matches(num_elements, -1);`。
 *    `-1` 被隐式转换为无符号 32 位整数的最大值 `UINT32_MAX`。
 *    但在第 81 行中，条件 `if (current_match == -1)` 将有符号的 `-1` 直接同无符号变量对比。
 *    这在许多现代编译器下会触发有符号与无符号比较警告 (`-Wsign-compare`)。
 *    **修复**：定义显式类型安全的哨兵常量 `const std::uint32_t UNMATCHED = UINT32_MAX;` 并代替所有的 `-1`。
 * 2. **数组越界读防护**：如果输入偏好是不完整或者异常偏好列表，可能会导致求婚索引 `proposal_index` 溢出，造成未定义行为。
 *    **修复**：在获取求婚对象时，加入边界安全判定，确保求婚指针不越界。
 *
 * @author [B Karthik](https://github.com/BKarthik7)
 */

#include <algorithm>  /// 用于 std::find
#include <cassert>    /// 用于 assert 断言
#include <cstdint>   /// 用于 std::uint32_t
#include <iostream>   /// 用于输入输出
#include <vector>     /// 用于 std::vector

/**
 * @namespace greedy_algorithms
 * @brief 贪心算法命名空间
 */
namespace greedy_algorithms {
/**
 * @namespace stable_matching
 * @brief 稳定匹配算法相关命名空间
 */
namespace stable_matching {

// 显式无符号整型未匹配哨兵常量
const std::uint32_t UNMATCHED = -1; 

/**
 * @brief 使用 Gale-Shapley 算法寻找稳定婚姻匹配
 * @param secondary_preferences 次要群体（如女方）对主要群体（如男方）的偏好矩阵 (2D Vector)
 * @param primary_preferences 主要群体（如男方）对次要群体（如女方）的偏好矩阵 (2D Vector)
 * @returns 匹配映射结果数组：matches[secondary_id] = matched_primary_id
 */
std::vector<std::uint32_t> gale_shapley(
    const std::vector<std::vector<std::uint32_t>>& secondary_preferences,
    const std::vector<std::vector<std::uint32_t>>& primary_preferences) {
    
    std::uint32_t num_elements = secondary_preferences.size();
    
    // 初始化匹配矩阵，所有人均未匹配
    std::vector<std::uint32_t> matches(num_elements, UNMATCHED);
    std::vector<bool> is_free_primary(num_elements, true);
    
    // 记录主要群体成员的下一次求婚对象在偏好数组中的索引
    std::vector<std::uint32_t> proposal_index(num_elements, 0);

    while (true) {
        int free_primary_index = -1;

        // 寻找一个当前自由的（未婚）主要群体成员
        for (std::uint32_t i = 0; i < num_elements; i++) {
            if (is_free_primary[i]) {
                free_primary_index = i;
                break;
            }
        }

        // 如果全部匹配完毕，退出循环
        if (free_primary_index == -1) {
            break;
        }

        // 获取该成员的下一个求婚目标
        // 安全防护：确保不会由于偏好清单越界导致崩溃
        if (proposal_index[free_primary_index] >= primary_preferences[free_primary_index].size()) {
            is_free_primary[free_primary_index] = false; // 无法求婚，从候选池移除
            continue;
        }

        std::uint32_t secondary_to_propose =
            primary_preferences[free_primary_index][proposal_index[free_primary_index]];
        proposal_index[free_primary_index]++; // 指向下一位候选人

        // 获取该求婚目标当前的配偶状态
        std::uint32_t current_match = matches[secondary_to_propose];

        // 情况 A：如果该女方当前未婚，则直接匹配
        if (current_match == UNMATCHED) {
            matches[secondary_to_propose] = free_primary_index;
            is_free_primary[free_primary_index] = false;
        } else {
            // 情况 B：如果该女方已婚，在偏好名单中对比新求婚者与现任配偶
            auto new_proposer_rank =
                std::find(secondary_preferences[secondary_to_propose].begin(),
                          secondary_preferences[secondary_to_propose].end(),
                          free_primary_index);
            auto current_match_rank =
                std::find(secondary_preferences[secondary_to_propose].begin(),
                          secondary_preferences[secondary_to_propose].end(),
                          current_match);

            // 偏好矩阵中，迭代器位置越靠前代表偏好等级越高
            if (new_proposer_rank < current_match_rank) {
                // 女方决定更换匹配，接受新求婚者
                matches[secondary_to_propose] = free_primary_index;
                is_free_primary[free_primary_index] = false;
                
                // 现配偶被抛弃，恢复单身自由身
                is_free_primary[current_match] = true;
            }
        }
    }

    return matches;
}
}  // namespace stable_matching
}  // namespace greedy_algorithms

/**
 * @brief 单元自测用例
 */
static void tests() {
    // 测试 1
    std::vector<std::vector<std::uint32_t>> primary_preferences = {
        {0, 1, 2, 3}, {2, 1, 3, 0}, {1, 2, 0, 3}, {3, 0, 1, 2}};
    std::vector<std::vector<std::uint32_t>> secondary_preferences = {
        {1, 0, 2, 3}, {3, 0, 1, 2}, {0, 2, 1, 3}, {1, 2, 0, 3}};
    assert(greedy_algorithms::stable_matching::gale_shapley(
               secondary_preferences, primary_preferences) ==
           std::vector<std::uint32_t>({0, 2, 1, 3}));

    // 测试 2
    primary_preferences = {
        {0, 2, 1, 3}, {2, 3, 0, 1}, {3, 1, 2, 0}, {2, 1, 0, 3}};
    secondary_preferences = {
        {1, 0, 2, 3}, {3, 0, 1, 2}, {0, 2, 1, 3}, {1, 2, 0, 3}};
    assert(greedy_algorithms::stable_matching::gale_shapley(
               secondary_preferences, primary_preferences) ==
           std::vector<std::uint32_t>({0, 3, 1, 2}));

    // 测试 3
    primary_preferences = {{0, 1, 2}, {2, 1, 0}, {1, 2, 0}};
    secondary_preferences = {{1, 0, 2}, {2, 0, 1}, {0, 2, 1}};
    assert(greedy_algorithms::stable_matching::gale_shapley(
               secondary_preferences, primary_preferences) ==
           std::vector<std::uint32_t>({0, 2, 1}));

    // 测试 4：空列表边界测试
    primary_preferences = {};
    secondary_preferences = {};
    assert(greedy_algorithms::stable_matching::gale_shapley(
               secondary_preferences, primary_preferences) ==
           std::vector<std::uint32_t>({}));
           
    std::cout << "Gale-Shapley stable matching tests passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    tests();  // 运行测试
    return 0;
}
