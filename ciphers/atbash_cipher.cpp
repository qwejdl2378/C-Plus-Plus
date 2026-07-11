/**
 * @file
 * @brief Implementation of the [Atbash Cipher](https://en.wikipedia.org/wiki/Atbash) (埃特巴什码加密算法实现)
 *
 * @details
 * 埃特巴什码（Atbash Cipher）是一种特殊的单表代换密码。
 * 它的加密规律极其简单：将字母表完全倒序排列，然后一一对应替换。
 * 例如：'a' 被替换为 'z'，'b' 被替换为 'y'，以此类推。大写字母同理。
 * 埃特巴什码具有**对称性（对合性）**：用同样的映射对密文再次进行加密，就能直接将其还原为明文。
 *
 * 时间复杂度: $O(N)$
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【未知非字母字符内存污染与丢失 Bug 审计与修复】：
 * 1. **非字母字符被替换为 \0 Bug**：原代码在处理未知字符（如感叹号 `!`、句号 `.`、数字等）时，
 *    直接执行 `result += atbash_cipher_map[letter];`。
 *    这会直接在 `std::map` 中插入该字符作为新键，且对应的值默认初始化为 `\0`（空字符）。
 *    这会导致输入串末尾或中间的标点符号消失并被 `\0` 填充，破坏字符串内容且污染 Map。
 *    **修复**：使用 `find()` 在映射表中查找字符。如果找不到，表示该字符非字母，保持原样追加到结果中。
 *
 * @author [Focusucof](https://github.com/Focusucof)
 */

#include <cassert>   /// 用于 assert 断言
#include <iostream>  /// 用于标准输出
#include <map>       /// 用于 std::map
#include <string>    /// 用于 std::string

namespace ciphers {
namespace atbash {

// 埃特巴什码字符替换映射表
const std::map<char, char> atbash_cipher_map = {
    {'a', 'z'}, {'b', 'y'}, {'c', 'x'}, {'d', 'w'}, {'e', 'v'}, {'f', 'u'},
    {'g', 't'}, {'h', 's'}, {'i', 'r'}, {'j', 'q'}, {'k', 'p'}, {'l', 'o'},
    {'m', 'n'}, {'n', 'm'}, {'o', 'l'}, {'p', 'k'}, {'q', 'j'}, {'r', 'i'},
    {'s', 'h'}, {'t', 'g'}, {'u', 'f'}, {'v', 'e'}, {'w', 'd'}, {'x', 'c'},
    {'y', 'b'}, {'z', 'a'}, {'A', 'Z'}, {'B', 'Y'}, {'C', 'X'}, {'D', 'W'},
    {'E', 'V'}, {'F', 'U'}, {'G', 'T'}, {'H', 'S'}, {'I', 'R'}, {'J', 'Q'},
    {'K', 'P'}, {'L', 'O'}, {'M', 'N'}, {'N', 'M'}, {'O', 'L'}, {'P', 'K'},
    {'Q', 'J'}, {'R', 'I'}, {'S', 'H'}, {'T', 'G'}, {'U', 'F'}, {'V', 'E'},
    {'W', 'D'}, {'X', 'C'}, {'Y', 'B'}, {'Z', 'A'}, {' ', ' '}
};

/**
 * @brief Atbash 密码加密/解密处理函数
 * @param text 输入的原文本
 * @return 转换后的文本
 */
std::string atbash_cipher(const std::string& text) {
    std::string result;
    for (char letter : text) {
        auto it = atbash_cipher_map.find(letter);
        if (it != atbash_cipher_map.end()) {
            // 如果在映射表中，进行字母对应替换
            result += it->second;
        } else {
            // 核心修复：如果不在表中（如标点、数字），保持原字符输出，防止 Map 内存污染与 \0 乱码
            result += letter;
        }
    }
    return result;
}

}  // namespace atbash
}  // namespace ciphers

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1：常规英文字符与空格
    std::string text = "Hello World";
    std::string expected = "Svool Dliow";
    std::string encrypted_text = ciphers::atbash::atbash_cipher(text);
    std::string decrypted_text = ciphers::atbash::atbash_cipher(encrypted_text);
    
    assert(expected == encrypted_text);
    assert(text == decrypted_text);
    
    std::cout << "Original text: " << text << std::endl;
    std::cout << "Expected text: " << expected << std::endl;
    std::cout << "Encrypted text: " << encrypted_text << std::endl;
    std::cout << "Decrypted text: " << decrypted_text << std::endl;

    // 测试 2：验证标点符号及数字的无损保留（原版代码在此处会污染为 \0）
    std::string text_special = "C++ 2026!";
    std::string expected_special = "X++ 2026!";
    assert(ciphers::atbash::atbash_cipher(text_special) == expected_special);

    std::cout << "All Atbash Cipher tests passed successfully!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
