/**
 * @file
 * @brief Simple C++ implementation of the [SHA-256 Hashing Algorithm](https://en.wikipedia.org/wiki/SHA-2) (SHA-256 哈希算法实现)
 *
 * @details
 * SHA-256 是 SHA-2（Secure Hash Algorithm 2）密码散列函数家族的一员，由美国国家安全局（NSA）设计，并于 2001 年发布。
 * 该算法接收任意长度的消息并生成一个 256 位（32 字节）的定长散列值，广泛应用于数据完整性验证、数字签名及区块链技术（如比特币）。
 *
 * ### 算法步骤
 * 1. **计算填充尺寸**：填充消息，使得其字节长度模 64 后等于 56（即模 512 位后余 448）。
 * 2. **消息字节填充（get_char）**：消息首位填充 `0x80`，后续填充 `0x00`，最后的 8 字节填入大端序的 64 位原始消息比特长度。
 * 3. **计算消息日程数组**：将 512 位的消息块扩展为 64 个 32 位的字。
 * 4. **主循环与压缩变换**：利用 64 个常数（前 64 个素数的立方根小数部分前 32 位）和逻辑函数更新哈希状态寄存器。
 *
 * 时间复杂度: $O(N)$，其中 $N$ 是消息字节数。
 * 空间复杂度: $O(1)$，内存完全静态分配于栈。
 *
 * @note
 * 【32位/64位平台移植性兼容 Bug 审计与修复】：
 * 1. **`size_t` 引发 32 位平台越界抛出异常 Bug**：原 `get_char` 代码在提取原始比特大小的对应字节时，使用了：
 *    `extract_byte<size_t>(input_size * 8, padded_input_size - pos - 1)`
 *    `size_t` 的宽度与硬件平台相关（64 位系统下为 8 字节，32 位系统下为 4 字节）。
 *    如果程序在 32 位架构平台上编译，`sizeof(size_t)` 为 4。当 `pos` 开始扫描高 4 位字节（对应的 `byte_num >= 4`）时，
 *    `extract_byte` 内部的 `sizeof(in_value) <= byte_num` 安全断言会触发并强制抛出 `std::out_of_range` 越界异常，导致程序崩溃。
 *    **修复**：无论在何种目标平台下，哈希大小描述均强制采用固定的 `uint64_t`（8 字节宽度）进行处理，确保了代码在 32 位/64 位硬件平台下的 100% 移植兼容性：
 *    `extract_byte<uint64_t>(static_cast<uint64_t>(input_size) * 8, padded_input_size - pos - 1)`
 *
 * @author [Md. Anisul Haque](https://github.com/mdanisulh)
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <array>     
#include <cassert>   
#include <cstdint>   
#include <iomanip>   
#include <iostream>  
#include <sstream>   
#include <utility>   
#include <vector>    

namespace hashing {
namespace sha256 {

/**
 * @class Hash
 * @brief 保存哈希状态值并提供核心压缩和转十六进制字符串功能
 */
class Hash {
    // 初始化前 8 个素数（2..19）平方根小数部分前 32 位的哈希初始常数
    std::array<uint32_t, 8> hash = {0x6A09E667, 0xBB67AE85, 0x3C6EF372,
                                    0xA54FF53A, 0x510E527F, 0x9B05688C,
                                    0x1F83D9AB, 0x5BE0CD19};

 public:
    void update(const std::array<uint32_t, 64> &blocks);
    std::string to_string() const;
};

/**
 * @brief 将 32 位无符号整数循环右移
 * @param n 待循环右移的整数
 * @param rotate 移动的位数
 * @return 循环右移后的结果
 */
uint32_t right_rotate(uint32_t n, size_t rotate) {
    return (n >> rotate) | (n << (32 - rotate));
}

/**
 * @brief 更新哈希工作寄存器（核心压缩变换）
 * @param blocks 扩展后的消息日程数组
 */
void Hash::update(const std::array<uint32_t, 64> &blocks) {
    // 前 64 个素数（2..311）立方根小数部分前 32 位的哈希常量
    const std::array<uint32_t, 64> round_constants = {
        0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1,
        0x923F82A4, 0xAB1C5ED5, 0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
        0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174, 0xE49B69C1, 0xEFBE4786,
        0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
        0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147,
        0x06CA6351, 0x14292967, 0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
        0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85, 0xA2BFE8A1, 0xA81A664B,
        0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
        0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A,
        0x5B9CCA4F, 0x682E6FF3, 0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
        0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2};

    // 初始化 8 个工作变量
    auto a = hash[0];
    auto b = hash[1];
    auto c = hash[2];
    auto d = hash[3];
    auto e = hash[4];
    auto f = hash[5];
    auto g = hash[6];
    auto h = hash[7];

    // 64 步迭代混淆运算
    for (size_t block_num = 0; block_num < 64; ++block_num) {
        const auto s1 =
            right_rotate(e, 6) ^ right_rotate(e, 11) ^ right_rotate(e, 25);
        const auto ch = (e & f) ^ (~e & g);
        const auto temp1 =
            h + s1 + ch + round_constants[block_num] + blocks[block_num];
        const auto s0 =
            right_rotate(a, 2) ^ right_rotate(a, 13) ^ right_rotate(a, 22);
        const auto maj = (a & b) ^ (a & c) ^ (b & c);
        const auto temp2 = s0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    // 将哈希值加到原哈希寄存器中
    hash[0] += a;
    hash[1] += b;
    hash[2] += c;
    hash[3] += d;
    hash[4] += e;
    hash[5] += f;
    hash[6] += g;
    hash[7] += h;
}

/**
 * @brief 将最终的哈希状态数组格式化转换为 64 位十六进制散列字符串
 */
std::string Hash::to_string() const {
    std::stringstream ss;
    for (size_t i = 0; i < 8; ++i) {
        ss << std::hex << std::setfill('0') << std::setw(8) << hash[i];
    }
    return ss.str();
}

/**
 * @brief 计算填充后所需的总大小（必须是 64 字节的整数倍）
 */
std::size_t compute_padded_size(const std::size_t input_size) {
    if (input_size % 64 < 56) {
        return input_size + 64 - (input_size % 64);
    }
    return input_size + 128 - (input_size % 64);
}

/**
 * @brief 提取数值特定偏移位置的单字节
 * @param in_value 待提取数据
 * @param byte_num 字节偏移位置（0 表示最低有效字节 LSB）
 * @return 对应字节值
 */
template <typename T>
uint8_t extract_byte(const T in_value, const std::size_t byte_num) {
    if (sizeof(in_value) <= byte_num) {
        throw std::out_of_range("Byte at index byte_num does not exist");
    }
    return (in_value >> (byte_num * 8)) & 0xFF;
}

/**
 * @brief 获取填充后虚拟扩展字符串中指定索引位置的单个字符
 * @param input 原始输入字符串
 * @param pos 期望查询的绝对位置索引
 */
char get_char(const std::string &input, std::size_t pos) {
    const auto input_size = input.length();
    if (pos < input_size) {
        return input[pos];
    }
    if (pos == input_size) {
        return '\x80'; // 填充起始标记位 10000000
    }
    const auto padded_input_size = compute_padded_size(input_size);
    if (pos < padded_input_size - 8) {
        return '\x00'; // 中间部分全部填充 0
    }
    if (padded_input_size <= pos) {
        throw std::out_of_range("pos is out of range");
    }
    // 核心修复：将 extract_byte 类型指定为固定的 uint64_t 以适配 32 位机器，消除溢出崩溃隐患
    return static_cast<char>(
        extract_byte<uint64_t>(static_cast<uint64_t>(input_size) * 8, padded_input_size - pos - 1));
}

/**
 * @brief 根据 512 位分块生成扩展至 64 个词的消息日程表数组 (Message Schedule Array)
 */
std::array<uint32_t, 64> create_message_schedule_array(const std::string &input,
                                                       const size_t byte_num) {
    std::array<uint32_t, 64> blocks{};

    // 将 64 字节分块依次组合为 16 个 32 位的子块
    for (size_t block_num = 0; block_num < 16; ++block_num) {
        blocks[block_num] =
            (static_cast<uint8_t>(get_char(input, byte_num + block_num * 4))
             << 24) |
            (static_cast<uint8_t>(get_char(input, byte_num + block_num * 4 + 1))
             << 16) |
            (static_cast<uint8_t>(get_char(input, byte_num + block_num * 4 + 2))
             << 8) |
            static_cast<uint8_t>(get_char(input, byte_num + block_num * 4 + 3));
    }

    // 将 16 个 32 位子块扩展到 64 个子块
    for (size_t block_num = 16; block_num < 64; ++block_num) {
        const auto s0 = right_rotate(blocks[block_num - 15], 7) ^
                        right_rotate(blocks[block_num - 15], 18) ^
                        (blocks[block_num - 15] >> 3);
        const auto s1 = right_rotate(blocks[block_num - 2], 17) ^
                        right_rotate(blocks[block_num - 2], 19) ^
                        (blocks[block_num - 2] >> 10);
        blocks[block_num] =
            blocks[block_num - 16] + s0 + blocks[block_num - 7] + s1;
    }

    return blocks;
}

/**
 * @brief SHA-256 算法统一入口函数
 * @param input 原始消息字符串
 * @return 64 位散列十六进制字符串
 */
std::string sha256(const std::string &input) {
    Hash h;
    for (size_t byte_num = 0; byte_num < compute_padded_size(input.length());
         byte_num += 64) {
        h.update(create_message_schedule_array(input, byte_num));
    }
    return h.to_string();
}
}  // namespace sha256
}  // namespace hashing

/**
 * @brief 单元自测 - 填充尺寸校验
 */
static void test_compute_padded_size() {
    assert(hashing::sha256::compute_padded_size(55) == 64);
    assert(hashing::sha256::compute_padded_size(56) == 128);
    assert(hashing::sha256::compute_padded_size(130) == 192);
}

/**
 * @brief 单元自测 - 字节数据提取校验
 */
static void test_extract_byte() {
    assert(hashing::sha256::extract_byte<uint32_t>(512, 0) == 0);
    assert(hashing::sha256::extract_byte<uint32_t>(512, 1) == 2);
    bool exception = false;
    try {
        hashing::sha256::extract_byte<uint32_t>(512, 5);
    } catch (const std::out_of_range &) {
        exception = true;
    }
    assert(exception);
}

/**
 * @brief 单元自测 - 消息扩展虚拟边界字符读取校验
 */
static void test_get_char() {
    assert(hashing::sha256::get_char("test", 3) == 't');
    assert(hashing::sha256::get_char("test", 4) == '\x80');
    assert(hashing::sha256::get_char("test", 5) == '\x00');
    assert(hashing::sha256::get_char("test", 63) == 32);
    bool exception = false;
    try {
        hashing::sha256::get_char("test", 64);
    } catch (const std::out_of_range &) {
        exception = true;
    }
    assert(exception);
}

/**
 * @brief 单元自测 - 循环右移功能校验
 */
static void test_right_rotate() {
    assert(hashing::sha256::right_rotate(128, 3) == 16);
    assert(hashing::sha256::right_rotate(1, 30) == 4);
    assert(hashing::sha256::right_rotate(6, 30) == 24);
}

/**
 * @brief 单元自测 - 标准哈希测试向量核对
 */
static void test_sha256() {
    struct TestCase {
        const std::string input;
        const std::string expected_hash;
        TestCase(std::string input, std::string expected_hash)
            : input(std::move(input)),
              expected_hash(std::move(expected_hash)) {}
    };
    const std::vector<TestCase> test_cases{
        TestCase(
            "",
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"),
        TestCase(
            "test",
            "9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08"),
        TestCase(
            "Hello World",
            "a591a6d40bf420404a011733cfb7b190d62c65bf0bcda32b57b277d9ad9f146e"),
        TestCase("Hello World!",
                 "7f83b1657ff1fc53b92dc18148a1d65dfc2d4b1fa3d677284addd200126d9"
                 "069")};
    for (const auto &tc : test_cases) {
        assert(hashing::sha256::sha256(tc.input) == tc.expected_hash);
    }
}

/**
 * @brief 单元测试入口
 */
static void test() {
    test_compute_padded_size();
    test_extract_byte();
    test_get_char();
    test_right_rotate();
    test_sha256();

    std::cout << "All SHA-256 tests have successfully passed!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试用例确认正确性
    return 0;
}
