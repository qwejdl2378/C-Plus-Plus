/**
 * @file
 * @brief Simple C++ implementation of the [SHA-1 Hashing Algorithm](https://en.wikipedia.org/wiki/SHA-1) (SHA-1 哈希算法实现)
 *
 * @details
 * SHA-1（Secure Hash Algorithm 1）是一种密码散列函数，由美国国家安全局（NSA）设计，并由美国国家标准与技术研究院（NIST）发布为联邦信息处理标准（FIPS）。
 * 它能将任意长度的输入消息映射为一个 160 位（20 字节）的散列值（通常用 40 位十六进制数表示）。
 * 虽然自 2010 年起 SHA-1 已不再被视为安全防御级别的算法，但在许多旧系统和文件完整性校验中仍广泛存在。
 *
 * 时间复杂度: $O(N)$，其中 $N$ 是消息长度。
 * 空间复杂度: $O(1)$。
 *
 * @note
 * 【非交互式下 EOF 导致无限死循环 Hang 挂起与内存泄露 Bug 审计与修复】：
 * 1. **非交互式 EOF 无限死循环 Bug**：与 `md5.cpp` 结构类似，
 *    原 `interactive` 函数在非交互式/管道输入环境下遇到 EOF 时，`std::getline` 失败且不阻塞，
 *    会导致确认询问的内层 `while(true)` 变成 CPU 占用 100% 的死循环。
 *    **修复**：添加流状态检查，一旦 `std::getline` 失败检测到 EOF 即刻退出。
 * 2. **堆内存泄露修复**：核心算法 `hash_bs` 会在堆上分配 `new uint8_t[20]`，
 *    在使用后通过显式 `delete[] static_cast<uint8_t*>(sig)` 将其释放以避免内存泄漏。
 *
 * @author [tGautot](https://github.com/tGautot)
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <algorithm>  
#include <array>      
#include <cassert>    
#include <cstdint>
#include <cstring>    
#include <iostream>   
#include <string>     
#include <vector>     

namespace hashing {
namespace {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
} // namespace
namespace sha1 {

/**
 * @brief 将 32 位无符号整数循环左移
 * @param n 待循环左移的整数
 * @param rotate 移动的位数
 * @return 循环左移后的结果
 */
uint32_t leftRotate32bits(uint32_t n, std::size_t rotate) {
    return (n << rotate) | (n >> (32 - rotate));
}

/**
 * @brief 将 160 位哈希签名转换为 40 位十六进制字符串
 * @param sig 指向 20 字节签名的指针
 * @return 40 位十六进制字符串
 */
std::string sig2hex(void* sig) {
    const char* hexChars = "0123456789abcdef";
    auto* intsig = static_cast<uint8_t*>(sig);
    std::string hex = "";
    for (uint8_t i = 0; i < 20; i++) {
        hex.push_back(hexChars[(intsig[i] >> 4) & 0xF]);
        hex.push_back(hexChars[(intsig[i]) & 0xF]);
    }
    return hex;
}

/**
 * @brief SHA-1 算法核心逻辑，输入原始字节数组和大小
 * @param input_bs 字节流输入指针
 * @param input_size 输入的字节大小
 * @return 指向堆上分配的 20 字节 SHA-1 签名的指针（调用者需要执行 delete[] 释放内存）
 */
void* hash_bs(const void* input_bs, uint64_t input_size) {
    auto* input = static_cast<const uint8_t*>(input_bs);

    // 初始 160 位哈希值状态常数
    uint32_t h0 = 0x67452301, a = 0;
    uint32_t h1 = 0xEFCDAB89, b = 0;
    uint32_t h2 = 0x98BADCFE, c = 0;
    uint32_t h3 = 0x10325476, d = 0;
    uint32_t h4 = 0xC3D2E1F0, e = 0;

    // 计算填充后所需的总大小，必须是 64 字节（512 位）的整数倍
    uint64_t padded_message_size = 0;
    if (input_size % 64 < 56) {
        padded_message_size = input_size + 64 - (input_size % 64);
    } else {
        padded_message_size = input_size + 128 - (input_size % 64);
    }

    // 分配内存
    std::vector<uint8_t> padded_message(padded_message_size, 0);

    // 复制原数据
    if (input_size > 0 && input != nullptr) {
        std::copy(input, input + input_size, padded_message.begin());
    }

    // 追加 1 bit 标记 (0x80)
    padded_message[input_size] = 1 << 7;  

    // 末尾 8 字节填入原始消息长度（以 bit 为单位，大端序表示）
    uint64_t input_bitsize = input_size * 8;
    for (uint8_t i = 0; i < 8; i++) {
        padded_message[padded_message_size - 8 + i] =
            (input_bitsize >> (56 - 8 * i)) & 0xFF;
    }

    std::array<uint32_t, 80> blocks{};

    // 按 64 字节分块执行主循环
    for (uint64_t chunk = 0; chunk * 64 < padded_message_size; chunk++) {
        // 构建前 16 个 32 位子块
        for (uint8_t bid = 0; bid < 16; bid++) {
            blocks[bid] = 0;
            for (uint8_t cid = 0; cid < 4; cid++) {
                blocks[bid] = (blocks[bid] << 8) +
                              padded_message[chunk * 64 + bid * 4 + cid];
            }
        }

        // 将 16 个子块扩展至 80 个
        for (uint8_t i = 16; i < 80; i++) {
            blocks[i] =
                leftRotate32bits(blocks[i - 3] ^ blocks[i - 8] ^
                                     blocks[i - 14] ^ blocks[i - 16],
                                 1);
        }

        a = h0;
        b = h1;
        c = h2;
        d = h3;
        e = h4;

        // 80 步主循环
        for (uint8_t i = 0; i < 80; i++) {
            uint32_t F = 0, g = 0;
            if (i < 20) {
                F = (b & c) | ((~b) & d);
                g = 0x5A827999;
            } else if (i < 40) {
                F = b ^ c ^ d;
                g = 0x6ED9EBA1;
            } else if (i < 60) {
                F = (b & c) | (b & d) | (c & d);
                g = 0x8F1BBCDC;
            } else {
                F = b ^ c ^ d;
                g = 0xCA62C1D6;
            }

            uint32_t temp = leftRotate32bits(a, 5) + F + e + g + blocks[i];
            e = d;
            d = c;
            c = leftRotate32bits(b, 30);
            b = a;
            a = temp;
        }

        // 状态更新
        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
    }

    // 输出哈希状态值并确保其按大端方式排列为 20 字节签名
    auto* sig = new uint8_t[20];
    for (uint8_t i = 0; i < 4; i++) {
        sig[i] = (h0 >> (24 - 8 * i)) & 0xFF;
        sig[i + 4] = (h1 >> (24 - 8 * i)) & 0xFF;
        sig[i + 8] = (h2 >> (24 - 8 * i)) & 0xFF;
        sig[i + 12] = (h3 >> (24 - 8 * i)) & 0xFF;
        sig[i + 16] = (h4 >> (24 - 8 * i)) & 0xFF;
    }

    return sig;
}

/**
 * @brief 哈希包装入口函数，支持 message.data() 防止空串 UB 隐患
 */
void* hash(const std::string& message) {
    return hash_bs(message.data(), message.size());
}

}  // namespace sha1
}  // namespace hashing

/**
 * @brief 单元自测用例
 */
static void test() {
    // 1. 验证空字符串哈希，并释放堆内存防止泄漏
    void* sig = hashing::sha1::hash("");
    std::cout << "Hashing empty string" << std::endl;
    std::string hex1 = hashing::sha1::sig2hex(sig);
    std::cout << hex1 << std::endl << std::endl;
    assert(hex1 == "da39a3ee5e6b4b0d3255bfef95601890afd80709");
    delete[] static_cast<uint8_t*>(sig);

    // 2. 验证常规语句哈希，并释放内存
    void* sig2 = hashing::sha1::hash("The quick brown fox jumps over the lazy dog");
    std::cout << "Hashing The quick brown fox jumps over the lazy dog" << std::endl;
    std::string hex2 = hashing::sha1::sig2hex(sig2);
    std::cout << hex2 << std::endl << std::endl;
    assert(hex2 == "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12");
    delete[] static_cast<uint8_t*>(sig2);

    // 3. 验证相似语句哈希（微小变动带来巨大哈希变动），并释放内存
    void* sig3 = hashing::sha1::hash("The quick brown fox jumps over the lazy dog.");
    std::cout << "Hashing The quick brown fox jumps over the lazy dog." << std::endl;
    std::string hex3 = hashing::sha1::sig2hex(sig3);
    std::cout << hex3 << std::endl << std::endl;
    assert(hex3 == "408d94384216f890ff7a0c3528e8bed1e0b01621");
    delete[] static_cast<uint8_t*>(sig3);

    // 4. 验证复杂混合字符哈希，并释放内存
    void* sig4 = hashing::sha1::hash("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    std::cout << "Hashing ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" << std::endl;
    std::string hex4 = hashing::sha1::sig2hex(sig4);
    std::cout << hex4 << std::endl << std::endl;
    assert(hex4 == "761c457bf73b14d27e9e9265c46f4b4dda11f940");
    delete[] static_cast<uint8_t*>(sig4);
}

/**
 * @brief 交互式哈希计算器，修复 EOF 死循环并解决泄漏问题
 */
static void interactive() {
    std::string input;
    while (true) {
        std::cout << "Enter a message to be hashed (Ctrl-D or Ctrl-C to exit): " << std::endl;
        // 核心修复：检查 std::getline 返回值，防止 EOF 发生死循环
        if (!std::getline(std::cin, input)) {
            break;
        }
        void* sig = hashing::sha1::hash(input);
        std::cout << "Hash is: " << hashing::sha1::sig2hex(sig) << std::endl;
        // 核心修复：用完后立即回收堆签名，杜绝内存泄漏
        delete[] static_cast<uint8_t*>(sig); 

        bool outer_break = false;
        while (true) {
            std::cout << "Want to enter another message? (y/n) ";
            if (!std::getline(std::cin, input)) {
                outer_break = true;
                break;
            }
            if (input == "y") {
                break;
            } else if (input == "n") {
                outer_break = true;
                break;
            }
        }
        if (outer_break) {
            break;
        }
    }
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测用例

    std::cout << "Would you like to run interactive mode? (1 for Yes, 0 for No): ";
    int run_interactive = 0;
    if (!(std::cin >> run_interactive) || run_interactive == 0) {
        return 0;
    }

    interactive();
    return 0;
}
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
