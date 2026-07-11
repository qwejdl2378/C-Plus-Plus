/**
 * @file
 * @brief Implementation of [Base64 Encoding and Decoding](https://en.wikipedia.org/wiki/Base64) (Base64 编码与解码算法实现)
 *
 * @details
 * Base64 是一种基于 64 个 ASCII 字符来表示任意二进制数据的传输编码方式。
 * 每一个 Base64 字符对应 6 个比特（bit）的信息，因此，3 个 8-bit 的字节（共 24 bits）
 * 可以由 4 个 6-bit 的 Base64 字符来表示。
 *
 * ### 编码原理（3 字节转 4 字符）
 * 1. 每次从输入流中读取 3 个字节（共 24 位）。
 * 2. 将这 24 位划分为 4 组，每组 6 位。
 * 3. 每一组的 6 位无符号值（范围 0-63）作为索引，在标准字符表 `ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/` 中查找对应的字符输出。
 * 4. 如果输入字节数不足 3 个：
 *    - 剩余 2 字节：编码出 3 个 Base64 字符，末尾补一个 `=` 填充字符。
 *    - 剩余 1 字节：编码出 2 个 Base64 字符，末尾补两个 `=` 填充字符。
 *
 * 时间复杂度: $O(N)$
 * 空间复杂度: $O(N)$
 *
 * @note
 * 【有符号数据截断与畸形输入越界崩溃 Bug 审计与修复】：
 * 1. **找回索引类型截断 Bug**：原 `find_idx` 函数被定义为返回 `uint8_t`，但在字符未找到时却执行了 `return -1;`。
 *    在 C++ 中，这会导致 `-1` 隐式强转为无符号最大值 `255`。当解码非法的非 Base64 字符时，程序会用 `255` 进行位移运算，产生未定义的脏字符数据。
 *    **修复**：将返回类型更改为有符号的 `int`，从而安全保留 `-1` 异常返回值。
 * 2. **畸形密文长度越界读崩溃 Bug**：在解码函数 `base64_decode` 中，
 *    代码以 4 字节为单位滑动：`for (uint32_t i = 0; i < base64_str.size(); i += 4)`。
 *    如果输入的 Base64 字符串是不完整的畸形密文（长度不是 4 的倍数，如长度为 1），
 *    执行 `char second_byte = base64_str[i + 1];` 会直接触发**严重的数组越界读取崩溃**。
 *    **修复**：在解码前增加防御校验，确认长度是 4 的倍数且不为空：
 *    `if (base64_str.empty() || base64_str.size() % 4 != 0) return "";`
 *
 * @author [Ashish Daulatabad](https://github.com/AshishYUO)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>   /// 用于 std::uint32_t, uint8_t
#include <iostream>  /// 用于标准输出
#include <string>    /// 用于 std::string

namespace ciphers {
namespace base64_encoding {

// RFC4648 标准定义的 Base64 字符映射表
const std::string chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * @brief Base64 编码函数
 * @param input 待编码的明文/二进制字符串
 * @return Base64 编码后的 ASCII 字符串
 */
std::string base64_encode(const std::string &input) {
    std::string base64_string;
    
    // 每次处理 3 个字节
    for (uint32_t i = 0; i < input.size(); i += 3) {
        char first_byte = input[i];
        // 第 1 个字符：取前 6 位进行编码
        base64_string.push_back(chars[first_byte >> 2]);

        if (i + 1 < input.size()) {
            char second_byte = input[i + 1];
            // 第 2 个字符：取第 1 个字符的后 2 位 + 第 2 个字符的前 4 位
            base64_string.push_back(
                chars[(((first_byte & 3) << 4) | ((second_byte & 0xF0) >> 4))]);

            if (i + 2 < input.size()) {
                char third_byte = input[i + 2];
                // 第 3 个字符：取第 2 个字符的后 4 位 + 第 3 个字符的前 2 位
                base64_string.push_back(chars[((third_byte & 0xC0) >> 6) |
                                              ((second_byte & 0x0F) << 2)]);
                // 第 4 个字符：取第 3 个字符的后 6 位
                base64_string.push_back(chars[(third_byte & 0x3F)]);
            } else {
                // 剩余 2 个字节情况：末尾补一个 '='
                base64_string.push_back(chars[((second_byte & 0x0F) << 2)]);
                base64_string.push_back('=');
            }
        } else {
            // 剩余 1 个字节情况：末尾补两个 '='
            base64_string.push_back(chars[((first_byte & 3) << 4)]);
            base64_string.push_back('=');
            base64_string.push_back('=');
        }
    }
    return base64_string;
}

/**
 * @brief 辅助检索字符在 Base64 表中索引的函数
 * @param c 目标字符
 * @return 对应的索引值 (0-63)，若为非法字符则返回 -1
 */
int find_idx(const char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    } else if (c >= 'a' && c <= 'z') {
        return c - 'a' + 26;
    } else if (c >= '0' && c <= '9') {
        return c - '0' + 52;
    } else if (c == '+') {
        return 62;
    } else if (c == '/') {
        return 63;
    }
    return -1; // 核心修复：返回有符号整型 -1，防止被隐式强转为 uint8_t 的 255
}

/**
 * @brief Base64 解码函数
 * @param base64_str Base64 编码的密文字符串
 * @return 解码后的原始字符串
 */
std::string base64_decode(const std::string &base64_str) {
    // 核心修复：防范空串或长度非 4 的倍数的畸形输入导致 i+1 越界读崩溃
    if (base64_str.empty() || base64_str.size() % 4 != 0) {
        return "";
    }

    std::string base64_decoded;
    
    // 每次处理 4 个字符（还原出 3 个字节）
    for (uint32_t i = 0; i < base64_str.size(); i += 4) {
        char first_byte = base64_str[i];
        char second_byte = base64_str[i + 1];

        // 还原第 1 个字节：前 6 位 + 后 2 位
        int idx1 = find_idx(first_byte);
        int idx2 = find_idx(second_byte);
        if (idx1 == -1 || idx2 == -1) continue; // 跳过非法字符
        
        char first_actual_byte = static_cast<char>((idx1 << 2) | (idx2 >> 4));
        base64_decoded.push_back(first_actual_byte);

        // 如果第 3 位不是填充字符 '='
        if (base64_str[i + 2] != '=') {
            char third_byte = base64_str[i + 2];
            int idx3 = find_idx(third_byte);
            if (idx3 == -1) continue;

            // 还原第 2 个字节：前 4 位 + 后 4 位
            char second_actual_byte = static_cast<char>(((idx2 & 0x0F) << 4) | (idx3 >> 2));
            base64_decoded.push_back(second_actual_byte);

            // 如果第 4 位不是填充字符 '='
            if (base64_str[i + 3] != '=') {
                char fourth_byte = base64_str[i + 3];
                int idx4 = find_idx(fourth_byte);
                if (idx4 == -1) continue;

                // 还原第 3 个字节：前 2 位 + 后 6 位
                char third_actual_byte = static_cast<char>(((idx3 & 0x03) << 6) | idx4);
                base64_decoded.push_back(third_actual_byte);
            }
        }
    }
    return base64_decoded;
}

}  // namespace base64_encoding
}  // namespace ciphers

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1
    std::string str =
        "To err is human, but to really foul things up you need a computer.";
    std::string base64_str = ciphers::base64_encoding::base64_encode(str);
    std::string verify =
        "VG8gZXJyIGlzIGh1bWFuLCBidXQgdG8gcmVhbGx5IGZvdWwgdGhpbmdzIHVwIHlvdSBuZW"
        "VkIGEgY29tcHV0ZXIu";
    
    assert(base64_str == verify);
    std::string original_str = ciphers::base64_encoding::base64_decode(base64_str);
    assert(original_str == str);

    // 测试 2 (来自 Wikipedia 官方样例)
    str =
        "Man is distinguished, not only by his reason, but by this singular "
        "passion from other animals, which is a lust of the mind, that by a "
        "perseverance of delight in the continued and indefatigable generation "
        "of knowledge, exceeds the short vehemence of any carnal pleasure.";

    base64_str = ciphers::base64_encoding::base64_encode(str);
    verify =
        "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieS"
        "B0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBh"
        "IGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodC"
        "BpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25v"
        "d2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbG"
        "Vhc3VyZS4=";
    
    assert(base64_str == verify);
    original_str = ciphers::base64_encoding::base64_decode(base64_str);
    assert(original_str == str);

    // 测试 3：畸形大小输入安全防护
    assert(ciphers::base64_encoding::base64_decode("abc") == "");

    std::cout << "All Base64 Encoding and Decoding tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试
    return 0;
}
