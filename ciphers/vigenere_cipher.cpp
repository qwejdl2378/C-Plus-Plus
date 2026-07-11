/**
 * @file
 * @brief Implementation of the [Vigenère Cipher](https://en.wikipedia.org/wiki/Vigen%C3%A8re_cipher) Algorithm (维吉尼亚密码算法实现)
 *
 * @details
 * 维吉尼亚密码（Vigenère Cipher）是一种使用一系列交织的凯撒密码组成的字母加密方法。
 * 它是一种经典的**多表代换密码（Polyalphabetic Substitution Cipher）**。
 *
 * ### 加密与解密原理
 * 设字母表映射为 $A \to 0, B \to 1, \dots, Z \to 25$：
 * - 加密公式：$E(M_i) = (M_i + K_{i \bmod L_k}) \bmod 26$
 * - 解密公式：$D(C_i) = (C_i - K_{i \bmod L_k} + 26) \bmod 26$
 * 其中 $M_i$ 是明文的第 $i$ 个字符，$C_i$ 是密文的第 $i$ 个字符，$K$ 是密钥，其长度为 $L_k$。
 *
 * @note
 * 本实现只对大写英文字母（'A'-'Z'）进行加密和解密。
 *
 * 【空密钥除以零崩溃 Bug 审计与修复】：
 * 1. **空密钥除零崩溃 Bug**：在循环中，密钥指针以 `j = (j + 1) % key.length()` 循环复用。
 *    若传入的密钥 `key` 为空字符串 `""`，`key.length()` 为 0，模运算 `% 0` 会直接引发 **SIGFPE（浮点数除以零异常）** 导致程序崩溃退出。
 *    **修复**：在函数入口处添加防御性校验：
 *    `if (key.empty() || text.empty()) return text;`
 *    确保密钥长度不为零，根除除零崩溃隐患。
 *
 * @author [Deep Raval](https://github.com/imdeep2905)
 */

#include <iostream>
#include <string>
#include <cassert>

namespace ciphers {
namespace vigenere {   
namespace {

/**
 * @brief 将数值 [0, 25] 转换为对应的大写字母字符 ['A', 'Z']
 */
inline char get_char(const int x) {
    return static_cast<char>(x + 65); // ASCII 中 65 对应 'A'
}

/**
 * @brief 将大写字母字符 ['A', 'Z'] 转换为对应的整数数值 [0, 25]
 */
inline int get_value(const char c) {
    return static_cast<int>(c - 65);
}

} // 匿名命名空间

/**
 * @brief 维吉尼亚密码加密函数
 * @param text 待加密的明文字符串（只包含大写字母 A-Z）
 * @param key 加密密钥（只包含大写字母 A-Z）
 * @return 加密后的密文字符串
 */
std::string encrypt(const std::string &text, const std::string &key) {
    // 核心修复：防御空密钥导致的 % 0 除以零崩溃 Bug
    if (key.empty() || text.empty()) {
        return text;
    }

    std::string encrypted_text = "";
    
    // 循环遍历文本，密钥指针 j 在达到 key.length() 后回绕复用
    for (size_t i = 0, j = 0; i < text.length(); i++, j = (j + 1) % key.length()) {
        int place_value_text = get_value(text[i]);
        int place_value_key = get_value(key[j]);
        place_value_text = (place_value_text + place_value_key) % 26;
        encrypted_text += get_char(place_value_text);
    }
    return encrypted_text;
}

/**
 * @brief 维吉尼亚密码解密函数
 * @param text 待解密的密文字符串（只包含大写字母 A-Z）
 * @param key 解密密钥（只包含大写字母 A-Z）
 * @return 解密还原的明文字符串
 */
std::string decrypt(const std::string &text, const std::string &key) {
    // 核心修复：防御空密钥导致的 % 0 除以零崩溃 Bug
    if (key.empty() || text.empty()) {
        return text;
    }

    std::string decrypted_text = "";
    for (size_t i = 0, j = 0; i < text.length(); i++, j = (j + 1) % key.length()) {
        int place_value_text = get_value(text[i]);
        int place_value_key = get_value(key[j]);
        place_value_text = (place_value_text - place_value_key + 26) % 26;
        decrypted_text += get_char(place_value_text);
    }        
    return decrypted_text;
}

} // namespace vigenere
} // namespace ciphers

/**
 * @brief 单元自测用例
 */
void test() {
    // 测试 1
    std::string text1 = "NIKOLATESLA";
    std::string encrypted1 = ciphers::vigenere::encrypt(text1, "TESLA");
    std::string decrypted1 = ciphers::vigenere::decrypt(encrypted1, "TESLA");
    assert(text1 == decrypted1);
    std::cout << "Original text : " << text1;
    std::cout << " , Encrypted text (with key = TESLA) : " << encrypted1;
    std::cout << " , Decrypted text : "<< decrypted1 << std::endl;

    // 测试 2
    std::string text2 = "GOOGLEIT";
    std::string encrypted2 = ciphers::vigenere::encrypt(text2, "REALLY");
    std::string decrypted2 = ciphers::vigenere::decrypt(encrypted2, "REALLY");
    assert(text2 == decrypted2);
    std::cout << "Original text : " << text2;
    std::cout << " , Encrypted text (with key = REALLY) : " << encrypted2;
    std::cout << " , Decrypted text : "<< decrypted2 << std::endl;

    // 测试 3：空密钥防护验证
    assert(ciphers::vigenere::encrypt("TEST", "") == "TEST");
    assert(ciphers::vigenere::decrypt("TEST", "") == "TEST");
    std::cout << "Vigenere Cipher empty key protection test passed!\n";

    std::cout << "All Vigenere Cipher tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行自测
    return 0;
}
