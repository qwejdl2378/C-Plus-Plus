/**
 * @file
 * @brief Implementation of the [A1Z26 Cipher](https://www.dcode.fr/letter-number-cipher) (A1Z26 字母数字转换密码实现)
 *
 * @details
 * A1Z26 密码是一种简单的单表代换密码，它通过将英文字母替换为其在字母表中的序号来实现加密。
 * 例如：A 对应 1，B 对应 2，...，Z 对应 26。在加密输出中，字母之间通常用连字符 `-` 连接，单词之间用空格隔开。
 *
 * 时间复杂度: 加密 $O(N)$，解密 $O(N)$
 * 空间复杂度: $O(N)$
 *
 * @note
 * 【空输入下溢崩溃与非法字符隐质污染 Bug 审计与修复】：
 * 1. **空字符串 pop_back 崩溃 Bug**：在 `encrypt` 和 `decrypt` 函数尾部，
 *    代码直接无条件执行了 `result.pop_back();`。
 *    若输入字符串为空（`""`），对空字符串调用 `pop_back()` 是 C++ 中的**未定义行为（Undefined Behavior）**，会在许多编译器上导致程序段错误直接崩溃。
 *    **修复**：在调用 `pop_back()` 前增加安全判断：`if (!result.empty()) result.pop_back();`。
 * 2. **未知字符隐式修改 Map Bug**：在加密非字母字符（如标点符号）时，
 *    直接使用 `a1z26_encrypt_map[letter]` 会隐式在 `std::map` 中插入该未知字符键，键值默认为 0。这会造成垃圾数据污染哈希表。
 *    **修复**：使用 `find()` 方法检查字符是否在映射表中，对于空格和非字母字符进行保留或安全处理，从而彻底杜绝内存污染。
 *
 * @author [Focusucof](https://github.com/Focusucof)
 */

#include <algorithm>  /// 用于 std::transform 和 std::replace
#include <cassert>    /// 用于 assert 断言
#include <cstdint>    /// 用于 uint8_t
#include <iostream>   /// 用于标准输出
#include <map>        /// 用于 std::map
#include <sstream>    /// 用于 std::stringstream
#include <string>     /// 用于 std::string
#include <vector>     /// 用于 std::vector

namespace ciphers {
namespace a1z26 {

// 解密字母映射表
const std::map<uint8_t, char> a1z26_decrypt_map = {
    {1, 'a'},  {2, 'b'},  {3, 'c'},  {4, 'd'},  {5, 'e'},  {6, 'f'},  {7, 'g'},
    {8, 'h'},  {9, 'i'},  {10, 'j'}, {11, 'k'}, {12, 'l'}, {13, 'm'}, {14, 'n'},
    {15, 'o'}, {16, 'p'}, {17, 'q'}, {18, 'r'}, {19, 's'}, {20, 't'}, {21, 'u'},
    {22, 'v'}, {23, 'w'}, {24, 'x'}, {25, 'y'}, {26, 'z'},
};

// 加密数字映射表
const std::map<char, uint8_t> a1z26_encrypt_map = {
    {'a', 1},  {'b', 2},  {'c', 3},  {'d', 4},  {'e', 5},  {'f', 6},  {'g', 7},
    {'h', 8},  {'i', 9},  {'j', 10}, {'k', 11}, {'l', 12}, {'m', 13}, {'n', 14},
    {'o', 15}, {'p', 16}, {'q', 17}, {'r', 18}, {'s', 19}, {'t', 20}, {'u', 21},
    {'v', 22}, {'w', 23}, {'x', 24}, {'y', 25}, {'z', 26}
};

/**
 * @brief A1Z26 加密函数
 * @param text 待加密的明文字符串
 * @returns 密文字符串（各字母间以 '-' 隔开，单词间以空格隔开）
 */
std::string encrypt(std::string text) {
    if (text.empty()) {
        return "";
    }

    std::string result;
    // 转换为小写字母
    std::transform(text.begin(), text.end(), text.begin(), ::tolower);
    // 将冒号替换为空格以规避冲突
    std::replace(text.begin(), text.end(), ':', ' ');

    for (char letter : text) {
        if (letter != ' ') {
            // 核心修复：先进行 find() 检查，防范非法字符直接用 [] 导致向 map 中隐式写入脏数据
            auto it = a1z26_encrypt_map.find(letter);
            if (it != a1z26_encrypt_map.end()) {
                result += std::to_string(it->second);
                result += "-";
            }
        } else {
            // 如果遇到空格，退格掉前一个字母后的 '-'，然后附加空格
            if (!result.empty() && result.back() == '-') {
                result.pop_back();
            }
            result += ' ';
        }
    }

    // 核心修复：添加安全判断，避免空字符下溢崩溃
    if (!result.empty() && result.back() == '-') {
        result.pop_back(); 
    }
    return result;
}

/**
 * @brief A1Z26 解密函数
 * @param text 待解密的密文字符串
 * @param bReturnUppercase 解密结果是否返回大写
 * @return 明文字符串
 */
std::string decrypt(const std::string& text, bool bReturnUppercase = false) {
    if (text.empty()) {
        return "";
    }

    std::string result;
    std::vector<std::string> word_array;
    std::stringstream sstream(text);
    std::string word;

    // 按空格分词（即分割每个单词）
    while (sstream >> word) {
        word_array.push_back(word);
    }

    for (auto& i : word_array) {
        // 将数字间的连字符 '-' 替换为空格，便于再次分割
        std::replace(i.begin(), i.end(), '-', ' ');
        std::vector<std::string> text_array;

        std::stringstream ss(i);
        std::string res_text;
        while (ss >> res_text) {
            text_array.push_back(res_text);
        }

        // 还原出每个字符
        for (auto& num_str : text_array) {
            try {
                int code = std::stoi(num_str);
                auto it = a1z26_decrypt_map.find(static_cast<uint8_t>(code));
                if (it != a1z26_decrypt_map.end()) {
                    result += it->second;
                }
            } catch (const std::exception&) {
                // 忽略非数字和解析异常
            }
        }
        result += ' '; // 单词间补回空格
    }

    // 核心修复：安全清除尾部多余的空格，杜绝空字符下溢段错误
    if (!result.empty()) {
        result.pop_back();
    }

    if (bReturnUppercase) {
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    }
    return result;
}

}  // namespace a1z26
}  // namespace ciphers

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1
    std::string input = "Hello World";
    std::string expected = "8-5-12-12-15 23-15-18-12-4";
    std::string output = ciphers::a1z26::encrypt(input);

    std::cout << "Input: " << input << "\nExpected: " << expected << "\nOutput: " << output << std::endl;
    assert(output == expected);
    std::cout << "TEST 1 PASSED\n";

    // 测试 2
    input = "12-15-23-5-18-3-1-19-5";
    expected = "lowercase";
    output = ciphers::a1z26::decrypt(input);

    std::cout << "Input: " << input << "\nExpected: " << expected << "\nOutput: " << output << std::endl;
    assert(output == expected);
    std::cout << "TEST 2 PASSED\n";

    // 测试 3
    input = "21-16-16-5-18-3-1-19-5";
    expected = "UPPERCASE";
    output = ciphers::a1z26::decrypt(input, true);

    std::cout << "Input: " << input << "\nExpected: " << expected << "\nOutput: " << output << std::endl;
    assert(output == expected);
    std::cout << "TEST 3 PASSED\n";

    // 测试 4：空输入保护
    assert(ciphers::a1z26::encrypt("") == "");
    assert(ciphers::a1z26::decrypt("") == "");
    std::cout << "TEST 4 (Empty Safety) PASSED\n";
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试
    return 0;
}
