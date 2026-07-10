/*
 * @brief [Magic sequence](https://www.csplib.org/Problems/prob019/) implementation (幻数序列回溯算法实现)
 *
 * @details 使用回溯法求解幻数序列问题。
 * "长度为 n 的幻数序列是一个由整数 x_0 ... x_{n-1} 组成的序列，其中每个元素均在 0 到 n-1 之间，
 * 满足：对于所有的 i (在 0 到 n-1 之间)，数字 i 在该序列中恰好出现 x_i 次。
 * 例如：6, 2, 1, 0, 0, 0, 1, 0, 0, 0 是长度为 10 的幻数序列，因为其中 0 出现了 6 次，1 出现了 2 次，
 * 2 出现了 1 次，6 出现了 1 次，其余数字均出现 0 次。"
 *
 * @author [Jxtopher](https://github.com/Jxtopher)
 */

#include <algorithm>  /// 用于 std::count
#include <cassert>    /// 用于 assert
#include <iostream>   /// 用于输入输出
#include <list>       /// 用于 std::list
#include <numeric>    /// 用于 std::accumulate
#include <vector>     /// 用于 std::vector

/**
 * @brief 回溯算法命名空间
 * @namespace backtracking
 */
namespace backtracking {
/**
 * @brief 幻数序列算法命名空间
 * @namespace magic_sequence
 */
namespace magic_sequence {
using sequence_t =
    std::vector<unsigned int>;  ///< 序列类型定义

/**
 * @brief 打印幻数序列的辅助函数
 * @param s 待打印的序列
 */
void print(const sequence_t& s) {
    for (const auto& item : s) std::cout << item << " ";
    std::cout << std::endl;
}

/**
 * @brief 检查当前序列是否满足幻数序列定义
 * @param s 当前序列
 * @returns `true` 如果是合法的幻数序列；`false` 如果不合法
 */
bool is_magic(const sequence_t& s) {
    for (unsigned int i = 0; i < s.size(); i++) {
        // 统计数字 i 在整个序列中出现的次数，检查其是否等于 s[i]
        if (std::count(s.cbegin(), s.cend(), i) != s[i]) {
            // 如果出现次数与 s[i] 记录的值不相符，说明不合法
            return false;
        }
    }
    return true;
}

/**
 * @brief 子解剪枝过滤器 (Pruning Filter)
 * @details
 * 幻数序列有一个数学特性：所有元素之和必须等于序列长度 N，
 * 并且每个元素都必须是正整数。
 * 因此，当前已经填入的前 depth 个元素之和不能超过 N。
 *
 * @param s 序列当前状态
 * @param depth 当前已经填入的元素长度 (决策树的当前深度)
 * @returns `true` 如果前缀和合法，没有超过长度 N；`false` 超过 N 应当被剪枝
 */
bool filtering(const sequence_t& s, unsigned int depth) {
    return std::accumulate(s.cbegin(), s.cbegin() + depth,
                           static_cast<unsigned int>(0)) <= s.size();
}

/**
 * @brief 幻数序列问题的回溯求解核心函数
 * @param s 存储当前序列状态的指针
 * @param ret 保存所有找到的合法幻数序列的链表指针
 * @param depth 当前决策树递归深度（正在尝试填入第 depth 个位置的数值）
 */
void solve(sequence_t* s, std::list<sequence_t>* ret, unsigned int depth = 0) {
    // 递归出口：当所有位置都尝试填入数值后，验证整个序列是否是合法的幻数序列
    if (depth == s->size()) {
        if (is_magic(*s)) {
            ret->push_back(*s); // 发现合法解，保存到结果链表中
        }
    } else {
        // 尝试在该位置填入 0 到 n-1 之间的任意数字
        for (unsigned int i = 0; i < s->size(); i++) {
            (*s)[depth] = i; // 做出选择：当前位置填入 i
            
            // 剪枝过滤：检测当前的部分序列前缀和是否仍然合法
            if (filtering(*s, depth + 1)) {
                solve(s, ret, depth + 1); // 递归进入下一深度
            }
        }
    }
}

}  // namespace magic_sequence
}  // namespace backtracking

/**
 * @brief 单元测试集
 */
static void test() {
    // 测试一个已知的合法幻数序列 (N = 10)
    backtracking::magic_sequence::sequence_t s_magic = {6, 2, 1, 0, 0,
                                                        0, 1, 0, 0, 0};
    assert(backtracking::magic_sequence::is_magic(s_magic));

    // 测试一个非法序列
    backtracking::magic_sequence::sequence_t s_not_magic = {5, 2, 1, 0, 0,
                                                            0, 1, 0, 0, 0};
    assert(!backtracking::magic_sequence::is_magic(s_not_magic));
}

/**
 * @brief 主函数，演示求解尺寸为 2 到 11 间所有幻数序列解
 * @returns 0
 */
int main() {
    test();  // 运行单元测试

    // 分别求解规模从 2 到 11 的幻数序列问题并打印结果
    for (unsigned int i = 2; i < 12; i++) {
        std::cout << "Solution for n = " << i << std::endl;
        
        // 保存所有有效解的链表
        std::list<backtracking::magic_sequence::sequence_t> list_of_solutions;
        
        // 初始填充序列为长度 i，每个元素初始设为 i
        backtracking::magic_sequence::sequence_t s1(i, i);
        
        // 启动回溯求解
        backtracking::magic_sequence::solve(&s1, &list_of_solutions);
        
        // 打印所有的求解方案
        for (const auto& item : list_of_solutions) {
            backtracking::magic_sequence::print(item);
        }
    }
    return 0;
}
