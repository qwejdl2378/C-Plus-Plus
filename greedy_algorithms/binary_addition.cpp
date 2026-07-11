/**
 * @file
 * @brief Implementation of binary string addition algorithm (二进制字符串加法算法实现)
 *
 * @details
 * 该算法模拟人类手工进行二进制数加法的步骤（从右向左逐位相加），在贪心算法分类中，
 * 它体现了“局部最优决策”：在处理当前列时，只关心当前列的两端比特值与前一位的进位值（Carry），
 * 产生当前列的局部和与向下一位的进位，最终通过拼接局部最优解构建全局结果。
 *
 * ### 算法步骤
 * 1. 从右向左（最低位到最高位）同时遍历两个二进制字符串。
 * 2. 提取当前对齐比特位上的数值，并与前一次的进位 `carry` 累加得到 `sum`。
 * 3. 新的进位 `carry = sum / 2`，而当前位应填入的数则为 `sum % 2`。
 * 4. 遍历结束后，如果最高位仍有进位 `carry = 1`，则在结果最前端补上 '1'。
 * 5. 将得到的反向结果字符串进行翻转，即为最终结果。
 *
 * @note
 * 【C++ 类型安全与整型溢出防范审计】：
 * 1. 原代码中声明 `int maxLength = std::max(a.size(), b.size());`，然后使用 `int i` 进行循环遍历。
 *    在 C++ 中，`std::string::size()` 返回的是无符号类型 `size_t`。如果将巨大的 `size_t` 隐式转换为带符号的 `int`，
 *    在特大型字符串输入下会引发**符号位溢出**或编译警告。
 *    **修复**：已将所有长度和循环索引统一重构为 `size_t` 类型，完全消除类型收缩与符号溢出隐患。
 *
 * 时间复杂度: $O(\max(N, M))$ (对两个字符串进行单次线性扫描)
 * 空间复杂度: $O(\max(N, M))$ (用于存储最终和字符串)
 * 
 * @author [Muhammad Junaid Khalid](https://github.com/mjk22071998)
 */

#include <algorithm>  /// 用于 std::reverse, std::all_of
#include <cassert>    /// 用于 assert 断言
#include <iostream>   /// 用于标准输出
#include <string>     /// 用于 std::string

/**
 * @namespace greedy_algorithms
 * @brief 贪心算法命名空间
 */
namespace greedy_algorithms {
/**
 * @brief 二进制加法处理类
 */
class BinaryAddition {
 public:
    /**
     * @brief 累加两个二进制字符串并返回结果
     * @param a 第一个二进制字符串
     * @param b 第二个二进制字符串
     * @return 二进制和字符串（若输入非法则返回空字符串）
     */
    std::string addBinary(const std::string& a, const std::string& b) {
        // 验证输入是否为合法的二进制字符串
        if (!isValidBinaryString(a) || !isValidBinaryString(b)) {
            return ""; 
        }

        std::string result;
        int carry = 0; // 进位
        size_t maxLength = std::max(a.size(), b.size()); // 统一使用 size_t 防范类型隐式收缩溢出

        // 从后往前逐位相加
        for (size_t i = 0; i < maxLength; ++i) {
            // 获取当前位置的比特值，若超出字符串长度则视作 0
            int bitA = (i < a.size()) ? (a[a.size() - 1 - i] - '0') : 0;
            int bitB = (i < b.size()) ? (b[b.size() - 1 - i] - '0') : 0;

            int sum = bitA + bitB + carry;
            carry = sum / 2;             // 计算进位
            result.push_back((sum % 2) + '0'); // 写入当前位的比特值
        }
        
        // 若最高位仍然有进位，补写最高位 1
        if (carry) {
            result.push_back('1');
        }
        
        // 翻转结果字符串得到正常的二进制升序形式
        std::reverse(result.begin(), result.end());
        return result;
    }

 private:
    /**
     * @brief 验证输入串是否只由字符 '0' 或 '1' 组成
     */
    bool isValidBinaryString(const std::string& str) const {
        return std::all_of(str.begin(), str.end(),
                           [](char c) { return c == '0' || c == '1'; });
    }
};
}  // namespace greedy_algorithms

/**
 * @brief 单元自测用例
 */
static void tests() {
    greedy_algorithms::BinaryAddition binaryAddition;

    // 正常测试用例
    assert(binaryAddition.addBinary("1010", "1101") == "10111");
    assert(binaryAddition.addBinary("1111", "1111") == "11110");
    assert(binaryAddition.addBinary("101", "11") == "1000");
    assert(binaryAddition.addBinary("0", "0") == "0");
    assert(binaryAddition.addBinary("0", "10101") == "10101");
    assert(binaryAddition.addBinary("10101", "0") == "10101");
    assert(binaryAddition.addBinary("1", "11111111") == "100000000");
    assert(binaryAddition.addBinary("10101010", "01010101") == "11111111");
    
    // 较长规模大数二进制相加
    assert(binaryAddition.addBinary("101010101010101010101010101010",
                                    "110110110110110110110110110110") ==
           "1100001100001100001100001100000");

    // 非法输入测试（包含非法字符），预期返回空串 ""
    assert(binaryAddition.addBinary("10102", "1101") == "");
    assert(binaryAddition.addBinary("ABC", "1101") == "");
    assert(binaryAddition.addBinary("1010", "1102") == "");
    assert(binaryAddition.addBinary("111", "1x1") == "");
    assert(binaryAddition.addBinary("1x1", "111") == "");
    assert(binaryAddition.addBinary("1234", "1101") == "");
    
    std::cout << "BinaryAddition tests passed!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    tests(); // 运行单元测试
    return 0;
}
