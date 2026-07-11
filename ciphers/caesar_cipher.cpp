/**
 * @file
 * @brief Implementation of the [Caesar Cipher](https://en.wikipedia.org/wiki/Caesar_cipher) Algorithm (凯撒密码算法实现)
 *
 * @details
 * 凯撒密码（Caesar Cipher）是一种极其古老和著名的替换密码。
 * 它通过将明文中的每个字母在字母表中向后（或向前）移动固定数目（`shift`）的位置来进行加密。
 * 例如，当移动位数是 3 时，'A' 被替换为 'D'，'B' 变为 'E'，依此类推。
 *
 * ### 数学模型
 * 设字母表映射为 $A \to 0, B \to 1, \dots, Z \to 25$：
 * - 加密公式：$E(x) = (x + shift) \bmod 26$
 * - 解密公式：$D(x) = (x - shift) \bmod 26$
 *
 * @note
 * 本实现默认只对大写英文字母（'A'-'Z'）进行加密和解密。
 *
 * 【负位移引起的取模截断与乱码 Bug 审计与修复】：
 * 1. **负位移未做标准化处理 Bug**：原加密函数 `encrypt` 中，
 *    直接计算 `(place_value + shift) % 26`。
 *    在 C++ 中，如果 `shift` 为负数导致被模数为负，`%` 运算会返回一个负的余数。
 *    原代码没有对 `encrypt` 中负的 `place_value` 进行加 26 的修正，这会导致加密出 ASCII 码超出 'A'-'Z' 范围的非字母乱码，解密时也无法匹配。
 *    **修复**：在函数入口处对位移值进行归一化处理：
 *    `int norm_shift = (shift % 26 + 26) % 26;`
 *    这保证了无论传入正位移、负位移还是超大位移，运算步长都安全收敛在 `[0, 25]` 内。
 *
 * @author [Deep Raval](https://github.com/imdeep2905)
 */

#include <iostream>
#include <string>
#include <cassert>

namespace ciphers {
namespace caesar {   
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
 * @brief 凯撒密码加密函数
 * @param text 待加密的明文字符串（只包含大写字母 A-Z）
 * @param shift 右移位数（可为负数或超大整数）
 * @return 加密后的密文字符串
 */
std::string encrypt(const std::string &text, const int &shift) {
    // 核心修复：对 shift 进行归一化，解决负位移取模返回负数产生乱码的 Bug
    int norm_shift = (shift % 26 + 26) % 26;
    
    std::string encrypted_text = "";
    for (char c : text) {
        int place_value = get_value(c);
        place_value = (place_value + norm_shift) % 26;
        encrypted_text += get_char(place_value);
    }
    return encrypted_text;
}

/**
 * @brief 凯撒密码解密函数
 * @param text 待解密的密文字符串（只包含大写字母 A-Z）
 * @param shift 加密时的右移位数
 * @return 解密后的明文字符串
 */
std::string decrypt(const std::string &text, const int &shift) {
    // 核心修复：对 shift 进行归一化，解决负数偏移导致的模运算问题
    int norm_shift = (shift % 26 + 26) % 26;
    
    std::string decrypted_text = "";
    for (char c : text) {
        int place_value = get_value(c);
        place_value = (place_value - norm_shift) % 26;
        if (place_value < 0) { // 解决减法取模产生负数的问题
            place_value += 26;
        }
        decrypted_text += get_char(place_value);
    }
    return decrypted_text;
}

} // namespace caesar
} // namespace ciphers

/**
 * @brief 单元自测用例
 */
void test() {
    // 测试 1：常规正位移
    std::string text1 = "ALANTURING";
    std::string encrypted1 = ciphers::caesar::encrypt(text1, 17);
    std::string decrypted1 = ciphers::caesar::decrypt(encrypted1, 17);
    assert(text1 == decrypted1);
    std::cout << "Original: " << text1 << " -> Encrypted (shift=17): " << encrypted1 << " -> Decrypted: " << decrypted1 << "\n";

    // 测试 2：超大位移
    std::string text2 = "HELLOWORLD";
    std::string encrypted2 = ciphers::caesar::encrypt(text2, 1729);
    std::string decrypted2 = ciphers::caesar::decrypt(encrypted2, 1729);
    assert(text2 == decrypted2);
    std::cout << "Original: " << text2 << " -> Encrypted (shift=1729): " << encrypted2 << " -> Decrypted: " << decrypted2 << "\n";

    // 测试 3：负数位移验证（测试上述 Bug 修复效果）
    std::string text3 = "CAESAR";
    std::string encrypted3 = ciphers::caesar::encrypt(text3, -5);
    std::string decrypted3 = ciphers::caesar::decrypt(encrypted3, -5);
    assert(text3 == decrypted3);
    std::cout << "Original: " << text3 << " -> Encrypted (shift=-5): " << encrypted3 << " -> Decrypted: " << decrypted3 << "\n";

    std::cout << "All Caesar Cipher tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行自测
    return 0;
}
