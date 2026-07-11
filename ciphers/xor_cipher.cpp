/**
 * @file
 * @brief Implementation of the [XOR Cipher](https://en.wikipedia.org/wiki/XOR_cipher) Algorithm (XOR 异或加解密算法实现)
 *
 * @details
 * XOR 密码是一种简单的加法密码（或加性密码），它基于二进制按位异或（Exclusive OR）操作的数学性质：
 * - $A \oplus 0 = A$ （任何数与 0 异或保持不变）
 * - $A \oplus A = 0$ （任何数与自身异或归零）
 * - $(A \oplus B) \oplus B = A \oplus 0 = A$ （对数据连续异或两次相同的 Key 即可还原为原数据）
 *
 * 由于这一对称特征，XOR 密码的**加密和解密使用的是完全相同的函数逻辑**。
 *
 * 时间复杂度: $O(N)$
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【高位整型截断与隐式转换审计与修复】：
 * 1. **大整数 Key 位宽截断隐患**：原代码传入的 `key` 类型为 `int`（通常为 32 位），
 *    而在对字符异或后：`char(c ^ key)`，由于 `char` 仅占 8 位，高 24 位会被直接截断。
 *    若传入的 `key` 大于 255，会产生隐式截断警告，且实际运算仅相当于 `key & 0xFF`。
 *    **修复**：在函数入口处对 `key` 进行 8 位掩码提取，并显式声明为 `uint8_t`，避免高位截断和潜在的符号位扩展未定义行为。
 *
 * @author [Deep Raval](https://github.com/imdeep2905)
 */

#include <iostream>
#include <string>
#include <cassert>
#include <cstdint>

namespace ciphers {
namespace XOR {   

/**
 * @brief 使用 XOR 密码进行加密
 * @param text 待加密的明文字符串
 * @param key 异或密钥
 * @return 密文字符串
 */
std::string encrypt(const std::string &text, const int &key) {
    // 核心修复：提取低 8 位密钥，防止高位整型截断产生编译器警告
    uint8_t key_byte = static_cast<uint8_t>(key & 0xFF);
    
    std::string encrypted_text = "";
    for (char c : text) {
        // 进行按位异或运算并强转回 char
        char encrypted_char = static_cast<char>(static_cast<uint8_t>(c) ^ key_byte);
        encrypted_text += encrypted_char;
    }
    return encrypted_text;
}

/**
 * @brief 使用 XOR 密码进行解密（与加密完全对称）
 * @param text 待解密的密文字符串
 * @param key 异或密钥
 * @return 解密还原的明文字符串
 */
std::string decrypt(const std::string &text, const int &key) {
    // 异或运算具有自反性，解密和加密逻辑完全相同
    return encrypt(text, key);
}

} // namespace XOR
} // namespace ciphers

/**
 * @brief 单元自测用例
 */
void test() {
    // 测试 1：包含特殊符号与空格的文本
    std::string text1 = "Whiplash! : Do watch this movie...";
    std::string encrypted1 = ciphers::XOR::encrypt(text1, 17);
    std::string decrypted1 = ciphers::XOR::decrypt(encrypted1, 17);
    assert(text1 == decrypted1);
    std::cout << "Original text : " << text1 << std::endl;
    std::cout << "Encrypted text : " << encrypted1 << std::endl;
    std::cout << "Decrypted text : " << decrypted1 << std::endl;

    // 测试 2
    std::string text2 = "->Valar M0rghulis<-";
    std::string encrypted2 = ciphers::XOR::encrypt(text2, 29);
    std::string decrypted2 = ciphers::XOR::decrypt(encrypted2, 29);
    assert(text2 == decrypted2);
    std::cout << "Original text : " << text2 << std::endl;
    std::cout << "Encrypted text : " << encrypted2 << std::endl;
    std::cout << "Decrypted text : " << decrypted2 << std::endl;

    // 测试 3：超大密钥截断校验
    std::string text3 = "SafeCheck";
    std::string encrypted3 = ciphers::XOR::encrypt(text3, 1029); // 1029 % 256 = 5
    std::string decrypted3 = ciphers::XOR::decrypt(encrypted3, 5);
    assert(text3 == decrypted3);

    std::cout << "All XOR Cipher tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行自测
    return 0;
}
