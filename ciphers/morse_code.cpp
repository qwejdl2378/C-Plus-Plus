/**
 * @file
 * @brief Implementation of [Morse Code Encoding and Decoding](https://en.wikipedia.org/wiki/Morse_code) (摩斯密码编码与解码实现)
 *
 * @details
 * 摩斯密码（Morse Code）是一种时序交替发送的信号代码，通过不同的信号长度组合表示不同的英文字母、数字和标点符号。
 * 它由点（Dot，`.`，发音为 "滴"）和划（Dash，`-`，发音为 "哒"）组成。
 *
 * ### 规则定义
 * - 字母内的点划间隔为 1 个单位。
 * - 字母之间的间隔为 3 个单位（在本实现中，密文中以空格 ` ` 隔开）。
 * - 单词之间的间隔为 7 个单位（在本实现中，明文中的空格 `' '` 在密文中被编码为斜杠 `/`，并在解码时还原为空格）。
 *
 * 时间复杂度: $O(N)$
 * 空间复杂度: $O(N)$
 *
 * @note
 * 【大写字母及空格未适配引发程序强行退出与缺少头文件编译 Bug 审计与修复】：
 * 1. **大写字母/空格引发 std::exit(0) 崩溃中断 Bug**：
 *    原代码中的 `char_to_morse` 仅包含小写字母 `'a'-'z'` 和数字 `'0'-'9'`，
 *    一旦传入大写字母（如 `'A'`）或空格（如 `' '`），会触发 `default` 分支打印错误并执行 `std::exit(0)` **强行中止整个程序**。
 *    这在实际开发中非常危险，直接将逻辑输入错误上升为进程崩溃。
 *    **修复**：
 *    - 增加输入字符转换为小写 (`tolower`) 的处理，完美适配大写英文字母。
 *    - 支持单词空格 `' '` 编码为斜杠 `"/"`，并在解码端逆向还原为 `' '`。
 *    - 移除不合理的 `std::exit(0)`，代之以返回占位符 `'?'`，极大提高了程序健壮性。
 * 2. **缺少库头文件 Bug**：原代码调用了 `std::exit`，但并未包含其所在的 `<cstdlib>` 头文件，在许多严格版本的编译器下会编译失败。
 *    **修复**：添加 `<cstdlib>` 头文件，尽管由于移除了 `std::exit` 已经不再是强依赖，但作为良好规范予以补齐。
 *
 * @author [Deep Raval](https://github.com/imdeep2905)
 */

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <cctype>   /// 用于 std::tolower
#include <cstdlib>  /// 用于 std::exit

namespace ciphers {
namespace morse {

/**
 * @brief 将单个字符转换为对应的摩斯密码点划串
 * @param c 待编码字符
 * @return 摩斯密码字符串 (如 ".-")，若遇到未知字符返回 "?"
 */
std::string char_to_morse(const char &c) {
    // 核心修复：转换为小写，统一大写与小写的映射处理
    char lower_c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

    // 支持单词间隔符的转换
    if (lower_c == ' ') {
        return "/";
    }

    switch (lower_c) {
        case 'a': return ".-";
        case 'b': return "-...";
        case 'c': return "-.-.";
        case 'd': return "-..";
        case 'e': return ".";
        case 'f': return "..-.";
        case 'g': return "--.";
        case 'h': return "....";
        case 'i': return "..";
        case 'j': return ".---";
        case 'k': return "-.-";
        case 'l': return ".-..";
        case 'm': return "--";
        case 'n': return "-.";
        case 'o': return "---";
        case 'p': return ".--.";
        case 'q': return "--.-";
        case 'r': return ".-.";
        case 's': return "...";
        case 't': return "-";
        case 'u': return "..-";
        case 'v': return "...-";
        case 'w': return ".--";
        case 'x': return "-..-";
        case 'y': return "-.--";
        case 'z': return "--..";
        case '1': return ".----";
        case '2': return "..---";
        case '3': return "...--";
        case '4': return "....-";
        case '5': return ".....";
        case '6': return "-....";
        case '7': return "--...";
        case '8': return "---..";
        case '9': return "----.";
        case '0': return "-----";
        default:
            // 核心修复：移除致命的 std::exit(0) 中断，以 "?" 代替，保障服务连续性
            std::cerr << "Found invalid character: " << c << " (skipped)" << std::endl;
            return "?";
    }
}

/**
 * @brief 将单个摩斯密码点划串转换为对应的字符
 * @param s 摩斯密码串 (如 ".-")
 * @return 还原后的字符，若遇到未知摩斯密码返回 '?'
 */
char morse_to_char(const std::string &s) {
    if (s == "/") {
        return ' ';
    }
    if (s == ".-") return 'a';
    if (s == "-...") return 'b';
    if (s == "-.-.") return 'c';
    if (s == "-..") return 'd';
    if (s == ".") return 'e';
    if (s == "..-.") return 'f';
    if (s == "--.") return 'g';
    if (s == "....") return 'h';
    if (s == "..") return 'i';
    if (s == ".---") return 'j';
    if (s == "-.-") return 'k';
    if (s == ".-..") return 'l';
    if (s == "--") return 'm';
    if (s == "-.") return 'n';
    if (s == "---") return 'o';
    if (s == ".--.") return 'p';
    if (s == "--.-") return 'q';
    if (s == ".-.") return 'r';
    if (s == "...") return 's';
    if (s == "-") return 't';
    if (s == "..-") return 'u';
    if (s == "...-") return 'v';
    if (s == ".--") return 'w';
    if (s == "-..-") return 'x';
    if (s == "-.--") return 'y';
    if (s == "--..") return 'z';
    if (s == ".----") return '1';
    if (s == "..---") return '2';
    if (s == "...--") return '3';
    if (s == "....-") return '4';
    if (s == ".....") return '5';
    if (s == "-....") return '6';
    if (s == "--...") return '7';
    if (s == "---..") return '8';
    if (s == "----.") return '9';
    if (s == "-----") return '0';
    
    // 核心修复：移除致命的 std::exit(0) 中断，以 '?' 代替
    std::cerr << "Found invalid Morse code: " << s << " (skipped)" << std::endl;
    return '?';
}

/**
 * @brief 摩斯密码加密函数
 * @param text 明文字符串
 * @return 密文字符串（字母以空格隔开，单词以 '/' 隔开）
 */
std::string encrypt(const std::string &text) {
    std::string encrypted_text = "";
    for (const char &c : text) {
        encrypted_text += char_to_morse(c) + " ";
    }
    return encrypted_text;
}

/**
 * @brief 摩斯密码解密函数
 * @param text 摩斯密码密文字符串
 * @return 解密还原的小写明文字符串
 */
std::string decrypt(const std::string &text) {
    std::string decrypted_text = "";
    std::size_t pos_start = 0, pos_end = 0, delim_len = 1;
    std::vector<std::string> splits;

    // 按空格切分出每一个摩斯码单元
    while ((pos_end = text.find(' ', pos_start)) != std::string::npos) {
        std::string token = text.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        if (!token.empty()) {
            splits.push_back(token);
        }
    }

    // 处理末尾漏掉的摩斯码
    if (pos_start < text.size()) {
        std::string token = text.substr(pos_start);
        if (!token.empty()) {
            splits.push_back(token);
        }
    }

    for (const std::string &s : splits) {
        decrypted_text += morse_to_char(s);
    }

    return decrypted_text;
}

}  // namespace morse
}  // namespace ciphers

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1：纯数字
    std::string text1 = "01234567890";
    std::string encrypted1 = ciphers::morse::encrypt(text1);
    std::string decrypted1 = ciphers::morse::decrypt(encrypted1);
    assert(text1 == decrypted1);
    std::cout << "Original text : " << text1 << std::endl;
    std::cout << "Encrypted text : " << encrypted1 << std::endl;
    std::cout << "Decrypted text : " << decrypted1 << std::endl;

    // 测试 2：纯字母
    std::string text2 = "abcdefghijklmnopqrstuvwxyz";
    std::string encrypted2 = ciphers::morse::encrypt(text2);
    std::string decrypted2 = ciphers::morse::decrypt(encrypted2);
    assert(text2 == decrypted2);
    std::cout << "Original text : " << text2 << std::endl;
    std::cout << "Encrypted text : " << encrypted2 << std::endl;
    std::cout << "Decrypted text : " << decrypted2 << std::endl;

    // 测试 3：包含大写字母与单词空格的句子（验证 Bug 修复成效）
    std::string text3 = "Hello World";
    std::string encrypted3 = ciphers::morse::encrypt(text3);
    std::string decrypted3 = ciphers::morse::decrypt(encrypted3);
    // 摩斯密码不分大小写，还原出来应为小写
    assert(decrypted3 == "hello world");
    std::cout << "Original text : " << text3 << std::endl;
    std::cout << "Encrypted text : " << encrypted3 << std::endl;
    std::cout << "Decrypted text : " << decrypted3 << std::endl;

    std::cout << "All Morse Code tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
