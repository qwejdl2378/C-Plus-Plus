/**
 * @file
 * @brief Simple C++ implementation of the [MD5 Hashing Algorithm](https://en.wikipedia.org/wiki/MD5) (MD5 哈希算法实现)
 *
 * @details
 * MD5（Message-Digest Algorithm 5）是由 Ron Rivest 于 1991 年设计的一种广泛使用的密码散列函数。
 * 它可以产生一个 128 位（16 字节）的散列值（Hash Value），用于确保信息传输完整一致。
 *
 * ### 算法步骤
 * 1. **填充消息（Padding）**：首先在消息后追加一个 1 bit (0x80) 和若干个 0，使得消息长度在模 512 位后余 448。
 *    最后在剩余的 64 位（8 字节）中填入原始消息的长度（以 bit 为单位），全部采用**小端字节序（Little Endian）**表示。
 * 2. **分块处理（Processing in 512-bit Blocks）**：将填充后的消息分成 512 位（64 字节）的数据块。
 *    每个块又细分为 16 个 32 位的子块。
 * 3. **主循环与四轮非线性变换**：对于每个 512 位的数据块，用四个辅助函数 $F, G, H, I$ 进行 64 步迭代，
 *    不断更新寄存器 A、B、C、D 的状态。
 *
 * 时间复杂度: $O(N)$，其中 $N$ 是消息长度。
 * 空间复杂度: $O(1)$ 或与消息长度成正比（用于缓存填充后的数据块）。
 *
 * @note
 * 【非交互式下 EOF 导致无限死循环 Hang 挂起与内存泄漏 Bug 审计与修复】：
 * 1. **非交互式 EOF 无限死循环 Bug**：原 `interactive` 函数中，
 *    使用 `std::getline(std::cin, input);` 读取用户输入。如果程序运行在自动化测试或非交互式管道环境中，
 *    `cin` 会立即触发 EOF。`std::getline` 失败且不阻塞，导致内部的 `while (true)` 确认询问循环变成 **100% CPU 占用死循环**。
 *    **修复**：严格检查 `std::getline` 的返回值，一旦检测到 EOF 或流错误立即优雅退出。
 * 2. **堆内存泄漏修复**：核心算法 `hash_bs` 会在堆上分配 `new uint8_t[16]` 作为生成的 128 位签名指针返回给调用者。
 *    原有的 `test` 与 `interactive` 中使用后均未释放，产生显式内存泄漏。
 *    **修复**：在获得 hex 字符串表示后，立即执行 `delete[] static_cast<uint8_t*>(sig)` 进行内存释放。
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
namespace md5 {

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
 * @brief 检测当前运行环境是是大端序还是小端序
 * @return true 表示大端序, false 表示小端序
 */
bool isBigEndian() {
    union {
        uint32_t i;
        std::array<char, 4> c;
    } bint = {0x01020304};

    return bint.c[0] == 1;
}

/**
 * @brief 在必要时将 32 位整数转换为小端字节序
 */
uint32_t toLittleEndian32(uint32_t n) {
    if (!isBigEndian()) {
        return ((n << 24) & 0xFF000000) | ((n << 8) & 0x00FF0000) |
               ((n >> 8) & 0x0000FF00) | ((n >> 24) & 0x000000FF);
    }
    return n;
}

/**
 * @brief 在必要时将 64 位整数转换为小端字节序
 */
uint64_t toLittleEndian64(uint64_t n) {
    if (!isBigEndian()) {
        return ((n << 56) & 0xFF00000000000000) |
               ((n << 40) & 0x00FF000000000000) |
               ((n << 24) & 0x0000FF0000000000) |
               ((n << 8) & 0x000000FF00000000) |
               ((n >> 8) & 0x00000000FF000000) |
               ((n >> 24) & 0x0000000000FF0000) |
               ((n >> 40) & 0x000000000000FF00) |
               ((n >> 56) & 0x00000000000000FF);
    }
    return n;
}

/**
 * @brief 将 128 位哈希签名转换为 32 位十六进制字符串
 * @param sig 指向 16 字节签名的指针
 * @return 32 位十六进制字符串
 */
std::string sig2hex(void* sig) {
    const char* hexChars = "0123456789abcdef";
    auto* intsig = static_cast<uint8_t*>(sig);
    std::string hex = "";
    for (uint8_t i = 0; i < 16; i++) {
        hex.push_back(hexChars[(intsig[i] >> 4) & 0xF]);
        hex.push_back(hexChars[(intsig[i]) & 0xF]);
    }
    return hex;
}

/**
 * @brief MD5 算法核心逻辑，输入原始字节数组和大小
 * @param input_bs 字节流输入指针
 * @param input_size 输入的字节大小
 * @return 指向堆上分配的 16 字节 MD5 签名的指针（调用者需要执行 delete[] 释放内存）
 */
void* hash_bs(const void* input_bs, uint64_t input_size) {
    auto* input = static_cast<const uint8_t*>(input_bs);

    // 每一步中循环左移的位数常量表
    std::array<uint32_t, 64> s = {
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
        5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,
        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

    // 常数表 K，代表 2^32 * abs(sin(i)) 的整数部分
    std::array<uint32_t, 64> K = {
        3614090360, 3905402710, 606105819,  3250441966, 4118548399, 1200080426,
        2821735955, 4249261313, 1770035416, 2336552879, 4294925233, 2304563134,
        1804603682, 4254626195, 2792965006, 1236535329, 4129170786, 3225465664,
        643717713,  3921069994, 3593408605, 38016083,   3634488961, 3889429448,
        568446438,  3275163606, 4107603335, 1163531501, 2850285829, 4243563512,
        1735328473, 2368359562, 4294588738, 2272392833, 1839030562, 4259657740,
        2763975236, 1272893353, 4139469664, 3200236656, 681279174,  3936430074,
        3572445317, 76029189,   3654602809, 3873151461, 530742520,  3299628645,
        4096336452, 1126891415, 2878612391, 4237533241, 1700485571, 2399980690,
        4293915773, 2240044497, 1873313359, 4264355552, 2734768916, 1309151649,
        4149444226, 3174756917, 718787259,  3951481745};

    // 初始寄存器状态（常数）
    uint32_t a0 = 0x67452301, A = 0;
    uint32_t b0 = 0xefcdab89, B = 0;
    uint32_t c0 = 0x98badcfe, C = 0;
    uint32_t d0 = 0x10325476, D = 0;

    // 计算填充后所需的总大小，必须是 64 字节（512 位）的整数倍
    uint64_t padded_message_size = 0;
    if (input_size % 64 < 56) {
        padded_message_size = input_size + 64 - (input_size % 64);
    } else {
        padded_message_size = input_size + 128 - (input_size % 64);
    }

    std::vector<uint8_t> padded_message(padded_message_size, 0);

    // 复制原消息内容到填充缓冲区
    if (input_size > 0 && input != nullptr) {
        std::copy(input, input + input_size, padded_message.begin());
    }

    // 在消息后追加一位 1，即 0x80
    padded_message[input_size] = 1 << 7;  

    // 在末尾第 8 字节处写入原始消息的比特大小（小端序表示）
    uint64_t input_bitsize_le = toLittleEndian64(input_size * 8);
    for (uint8_t i = 0; i < 8; i++) {
        padded_message[padded_message_size - 8 + i] =
            (input_bitsize_le >> (56 - 8 * i)) & 0xFF;
    }

    std::array<uint32_t, 16> blocks{};

    // 按 64 字节分块执行主循环
    for (uint64_t chunk = 0; chunk * 64 < padded_message_size; chunk++) {
        // 构建 16 个 32 位子块
        for (uint8_t bid = 0; bid < 16; bid++) {
            blocks[bid] = 0;
            for (uint8_t cid = 0; cid < 4; cid++) {
                blocks[bid] = (blocks[bid] << 8) +
                              padded_message[chunk * 64 + bid * 4 + cid];
            }
        }

        A = a0;
        B = b0;
        C = c0;
        D = d0;

        // 64 步迭代混淆运算
        for (uint8_t i = 0; i < 64; i++) {
            uint32_t F = 0, g = 0;
            if (i < 16) {
                F = (B & C) | ((~B) & D);
                g = i;
            } else if (i < 32) {
                F = (D & B) | ((~D) & C);
                g = (5 * i + 1) % 16;
            } else if (i < 48) {
                F = B ^ C ^ D;
                g = (3 * i + 5) % 16;
            } else {
                F = C ^ (B | (~D));
                g = (7 * i) % 16;
            }

            F += A + K[i] + toLittleEndian32(blocks[g]);

            A = D;
            D = C;
            C = B;
            B += leftRotate32bits(F, s[i]);
        }
        // 将结果累加到寄存器状态
        a0 += A;
        b0 += B;
        c0 += C;
        d0 += D;
    }

    // 导出最终的 128 位散列签名（分配小端字节序的 16 字节）
    auto* sig = new uint8_t[16];
    for (uint8_t i = 0; i < 4; i++) {
        sig[i] = (a0 >> (8 * i)) & 0xFF;
        sig[i + 4] = (b0 >> (8 * i)) & 0xFF;
        sig[i + 8] = (c0 >> (8 * i)) & 0xFF;
        sig[i + 12] = (d0 >> (8 * i)) & 0xFF;
    }

    return sig;
}

/**
 * @brief 哈希入口包装函数，自动调用 hash_bs
 */
void* hash(const std::string& message) {
    return hash_bs(message.data(), message.size());
}

}  // namespace md5
}  // namespace hashing

/**
 * @brief 单元自测用例
 */
static void test() {
    // 1. 验证空字符串哈希，并释放堆内存防止泄漏
    void* sig = hashing::md5::hash("");
    std::cout << "Hashing empty string" << std::endl;
    std::string hex1 = hashing::md5::sig2hex(sig);
    std::cout << hex1 << std::endl << std::endl;
    assert(hex1 == "d41d8cd98f00b204e9800998ecf8427e");
    delete[] static_cast<uint8_t*>(sig);

    // 2. 验证常规语句哈希，并释放内存
    void* sig2 = hashing::md5::hash("The quick brown fox jumps over the lazy dog");
    std::cout << "Hashing The quick brown fox jumps over the lazy dog" << std::endl;
    std::string hex2 = hashing::md5::sig2hex(sig2);
    std::cout << hex2 << std::endl << std::endl;
    assert(hex2 == "9e107d9d372bb6826bd81d3542a419d6");
    delete[] static_cast<uint8_t*>(sig2);

    // 3. 验证相似语句哈希（微小变动带来巨大哈希变动），并释放内存
    void* sig3 = hashing::md5::hash("The quick brown fox jumps over the lazy dog.");
    std::cout << "Hashing The quick brown fox jumps over the lazy dog." << std::endl;
    std::string hex3 = hashing::md5::sig2hex(sig3);
    std::cout << hex3 << std::endl << std::endl;
    assert(hex3 == "e4d909c290d0fb1ca068ffaddf22cbd0");
    delete[] static_cast<uint8_t*>(sig3);

    // 4. 验证复杂混合字符哈希，并释放内存
    void* sig4 = hashing::md5::hash("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    std::cout << "Hashing ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" << std::endl;
    std::string hex4 = hashing::md5::sig2hex(sig4);
    std::cout << hex4 << std::endl << std::endl;
    assert(hex4 == "d174ab98d277d9f5a5611c2c9f419d9f");
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
        void* sig = hashing::md5::hash(input);
        std::cout << "Hash is: " << hashing::md5::sig2hex(sig) << std::endl;
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
