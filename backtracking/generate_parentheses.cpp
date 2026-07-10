/**
 * @file
 * @brief Well-formed [Generated Parentheses](https://leetcode.com/explore/interview/card/top-interview-questions-medium/109/backtracking/794/) with all combinations. (括号生成算法实现)
 *
 * @details 括号生成问题：给定 n 对括号，生成所有可能的并且有效的括号组合。
 * 有效的括号序列：每个左括号 '(' 都有对应的右括号 ')'，且右括号闭合的顺序是正确的。
 *
 * @author [Giuseppe Coco](https://github.com/WoWS17)
 *
 */

#include <cassert>   /// 用于 assert 断言
#include <iostream>  /// 用于输入输出
#include <vector>    /// 用于 vector 容器

/**
 * @brief 回溯算法命名空间
 * @namespace backtracking
 */
namespace backtracking {
/**
 * @brief 括号生成类定义
 */
class generate_parentheses {
 private:
    std::vector<std::string> res;  ///< 保存所有生成的合法括号组合的容器

    void makeStrings(std::string str, int n, int closed, int open);

 public:
    std::vector<std::string> generate(int n);
};

/**
 * @brief 回溯递归函数，向字符串中逐个添加左括号或右括号
 *
 * @param str 当前回溯构建中的字符串状态
 * @param n 需要生成的括号对数
 * @param closed 当前字符串中已包含的右括号 ')' 数量
 * @param open 当前字符串中已包含的左括号 '(' 数量
 */
void generate_parentheses::makeStrings(std::string str, int n,
                                                     int closed, int open) {
    // 剪枝条件：在构建的任意时刻，右括号的数量不能超过左括号的数量。
    // 如果右括号数量大于左括号，说明一定存在无法闭合的右括号，当前分支无效。
    if (closed > open)  
        return;

    // 如果生成的字符串长度达到了 2*n，但左右括号数不相等，则为非法组合
    if ((str.length() == 2 * n) && (closed != open)) {
        return;
    }

    // 成功条件：字符串长度达到 2*n 且左右括号数相等，是一组合法解
    if (str.length() == 2 * n) {
        res.push_back(str);
        return;
    }

    // 递归分支 1：尝试向后添加一个右括号 ')'，并增加 closed 的计数
    makeStrings(str + ')', n, closed + 1, open);
    // 递归分支 2：尝试向后添加一个左括号 '('，并增加 open 的计数
    makeStrings(str + '(', n, closed, open + 1);
}

/**
 * @brief 包装接口，供外部调用以获取所有合法的括号排列组合
 *
 * @param n 括号的对数
 * @return 包含所有合法括号排列的 std::vector<std::string>
 */
std::vector<std::string> generate_parentheses::generate(int n) {
    backtracking::generate_parentheses::res.clear();
    // 括号序列必须以左括号 '(' 开头
    std::string str = "(";
    // 调用回溯辅助函数，初始时已放置 1 个左括号，0 个右括号
    generate_parentheses::makeStrings(str, n, 0, 1);
    return res;
}
}  // namespace backtracking

/**
 * @brief 自测用例集
 */
static void test() {
    int n = 0;
    std::vector<std::string> patterns;
    backtracking::generate_parentheses p;

    n = 1;
    patterns = {{"()"}};
    assert(p.generate(n) == patterns);

    n = 3;
    patterns = {{"()()()"}, {"()(())"}, {"(())()"}, {"(()())"}, {"((()))"}};
    assert(p.generate(n) == patterns);

    n = 4;
    patterns = {{"()()()()"}, {"()()(())"}, {"()(())()"}, {"()(()())"},
                {"()((()))"}, {"(())()()"}, {"(())(())"}, {"(()())()"},
                {"(()()())"}, {"(()(()))"}, {"((()))()"}, {"((())())"},
                {"((()()))"}, {"(((())))"}};
    assert(p.generate(n) == patterns);

    std::cout << "All tests passed\n";
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行自测函数
    return 0;
}
