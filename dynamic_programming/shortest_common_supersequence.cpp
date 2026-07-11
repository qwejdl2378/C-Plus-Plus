/**
 * @file
 * @brief Implementation of the [Shortest Common Supersequence](https://en.wikipedia.org/wiki/Shortest_common_supersequence_problem) algorithm (最短公共超序列算法实现)
 *
 * @details
 * 最短公共超序列（SCS, Shortest Common Supersequence）：
 * 给定两个字符串 X 和 Y，求一个长度最短的字符串 Z，使得 X 和 Y 都是 Z 的子序列（即保持原有字符顺序，但不要求连续）。
 *
 * ### 核心解题思路
 * 1. SCS 问题与最长公共子序列（LCS）问题密切相关：
 *    `length(SCS(X, Y)) = length(X) + length(Y) - length(LCS(X, Y))`
 * 2. 本算法首先利用动态规划构建 LCS 的 `lookup` 查找表。
 * 3. 接着利用该表从末尾开始反向回溯构建超序列：
 *    - 若 `X[i-1] == Y[j-1]`，当前字符只需在超序列中出现一次，加入超序列，且 `i--, j--`；
 *    - 否则，取 LCS 表中较大的前驱方向：
 *      - 若 `lookup[i-1][j] > lookup[i][j-1]`，加入 `X[i-1]` 且 `i--`；
 *      - 否则，加入 `Y[j-1]` 且 `j--`。
 * 4. 将回溯过程中未处理完毕的任一字符串剩余部分拷贝进超序列。
 * 5. 由于是反向构造，返回前需要对字符串执行反转操作。
 *
 * 时间复杂度: O(|str1| * |str2|)
 * 空间复杂度: O(|str1| * |str2|)
 *
 * @author [Ridhish Jain](https://github.com/ridhishjain)
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>

/**
 * @namespace dynamic_programming
 * @brief 动态规划算法命名空间
 */
namespace dynamic_programming {
/**
 * @namespace shortest_common_supersequence
 * @brief 最短公共超序列相关算法命名空间
 */
namespace shortest_common_supersequence {
/**
 * @brief 计算两个字符串的最短公共超序列（SCS）
 * @param str1 字符串 X
 * @param str2 字符串 Y
 * @returns 对应 shortest supersequence 字符串 Z
 */
std::string scs(const std::string &str1, const std::string &str2) {
    // 边界情况处理
    if(str1.empty() && str2.empty()) {
        return "";
    }
    else if(str1.empty()) {
        return str2;
    }
    else if(str2.empty()) {
        return str1;
    }

    // 建立 LCS 状态矩阵
    std::vector <std::vector <int>> lookup(str1.length() + 1, std::vector <int> (str2.length() + 1, 0));

    for(int i=1; i <= str1.length(); i++) {
        for(int j=1; j <= str2.length(); j++) {
            if(str1[i-1] == str2[j-1]) {
                lookup[i][j] = lookup[i-1][j-1] + 1;
            }
            else {
                lookup[i][j] = std::max(lookup[i-1][j], lookup[i][j-1]);
            }
        }
    }

    // 反向回溯构造超序列
    int i = str1.length();
    int j = str2.length();
    std::string s;

    while(i > 0 && j > 0) {
        // 如果当前两字符相同，只需添加一次
        if(str1[i-1] == str2[j-1]) {
            s.push_back(str1[i-1]);
            i--;
            j--;
        }
        // 字符不同，根据 LCS 的路径信息进行转移，同时保留原字符
        else {
            if(lookup[i-1][j] > lookup[i][j-1]) {
                s.push_back(str1[i-1]);
                i--;
            }
            else {
                s.push_back(str2[j-1]);
                j--;
            }
        }
    }

    // 拷贝剩下的字符
    while(i > 0) {
        s.push_back(str1[i-1]);
        i--;
    }
    while(j > 0) {
        s.push_back(str2[j-1]);
        j--;
    }

    // 回溯是从后往前，因此反转以得到正确的前向序列
    reverse(s.begin(), s.end());
    return s;
}
} // namespace shortest_common_supersequence
} // namespace dynamic_programming

/**
 * @brief 单元自测用例
 */
static void test() {
    std::vector <std::vector <std::string>> scsStrings {
        {"ABCXYZ", "ABZ"},
        {"ABZ", "ABCXYZ"},
        {"AGGTAB", "GXTXAYB"},
        {"X", "Y"},
    };

    std::vector <std::string> calculatedOutput(4, "");
    int i=0;
    for(auto & scsString : scsStrings) {
        calculatedOutput[i] = dynamic_programming::shortest_common_supersequence::scs(
            scsString[0], scsString[1]
        );
        i++;
    }

    std::vector <std::string> expectedOutput {
        "ABCXYZ",
        "ABCXYZ",
        "AGGXTXAYB",
        "XY"
    };

    for(int i=0; i < scsStrings.size(); i++) {
        assert(expectedOutput[i] == calculatedOutput[i]);
    }

    std::cout << "All tests passed successfully!\n";
    return;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行自测

    std::string s1, s2;
    std::cin >> s1;
    std::cin >> s2;

    std::string ans = dynamic_programming::shortest_common_supersequence::scs(s1, s2);
    std::cout << ans;
    return 0;
}
