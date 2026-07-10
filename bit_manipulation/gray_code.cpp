/**
 * @file
 * @brief Program to generate n-bit [Gray code](https://en.wikipedia.org/wiki/Gray_code) (生成 n 位格雷码)
 * 
 * @details
 * 格雷码（Gray Code）是一种循环二进制码（无权码）。
 * 它的核心特征是：任意两个相邻的数值，其二进制表示中仅有一个比特位发生改变。
 * 在通信、传感器数据读取和错误校正等领域有着广泛应用。
 *
 * ### 格雷码公式
 * 对于任意非负整数 i，其对应的格雷码值为：`i ^ (i >> 1)`。
 */

#include <bitset>  /// 用于格雷码二进制位图表示
#include <cassert>  /// 用于 assert 断言
#include <iostream>  /// 用于输入输出
#include <vector>  /// 用于 std::vector 容器

/**
 * @namespace bit_manipulation
 * @brief 位运算算法命名空间
 */
namespace bit_manipulation {
/**
 * @namespace gray_code
 * @brief 格雷码生成相关命名空间
 */
namespace gray_code {
/**
 * @brief 生成含有 2^n 个格雷码数字的数组序列
 * @param n 格雷码的二进制位数
 * @return 包含 std::bitset<32> 表示的格雷码序列的 vector
 */
std::vector<std::bitset<32>> gray_code_generation(int n) {
    std::vector<std::bitset<32>> gray_code = {};

    // 如果 n <= 0，没有格雷码，返回空数组
    if (n <= 0) {
        return gray_code;
    }
    
    int total_codes = 1 << n;  // 总共含有 2^n 个格雷码编码

    for (int i = 0; i < total_codes; i++) {
        int gray_num = i ^ (i >> 1);  // 核心格雷码映射公式：i 异或 i 右移 1 位
        gray_code.push_back(std::bitset<32>(gray_num));  // 保存结果为 32 位位图格式
    }

    return gray_code;
} 
}  // namespace gray_code
}  // namespace bit_manipulation

/**
 * @brief 单元自测用例
 */
static void test() { 
    std::vector<std::bitset<32>> gray_code_negative_1 = {};
    std::vector<std::bitset<32>> gray_code_0 = {};

    std::vector<std::bitset<32>> gray_code_1 = {
        std::bitset<32>(0), std::bitset<32>(1)
    };

    std::vector<std::bitset<32>> gray_code_2 = {
        std::bitset<32>(0), std::bitset<32>(1), std::bitset<32>(3), std::bitset<32>(2)
    };

    std::vector<std::bitset<32>> gray_code_3 = {
        std::bitset<32>(0), std::bitset<32>(1), std::bitset<32>(3), std::bitset<32>(2),
        std::bitset<32>(6), std::bitset<32>(7), std::bitset<32>(5), std::bitset<32>(4)
    };

    std::vector<std::bitset<32>> gray_code_4 = {
        std::bitset<32>(0), std::bitset<32>(1), std::bitset<32>(3), std::bitset<32>(2),
        std::bitset<32>(6), std::bitset<32>(7), std::bitset<32>(5), std::bitset<32>(4),
        std::bitset<32>(12), std::bitset<32>(13), std::bitset<32>(15), std::bitset<32>(14),
        std::bitset<32>(10), std::bitset<32>(11), std::bitset<32>(9), std::bitset<32>(8)
    };

    std::vector<std::bitset<32>> gray_code_5 = {
        std::bitset<32>(0), std::bitset<32>(1), std::bitset<32>(3), std::bitset<32>(2),
        std::bitset<32>(6), std::bitset<32>(7), std::bitset<32>(5), std::bitset<32>(4),
        std::bitset<32>(12), std::bitset<32>(13), std::bitset<32>(15), std::bitset<32>(14),
        std::bitset<32>(10), std::bitset<32>(11), std::bitset<32>(9), std::bitset<32>(8),
        std::bitset<32>(24), std::bitset<32>(25), std::bitset<32>(27), std::bitset<32>(26),
        std::bitset<32>(30), std::bitset<32>(31), std::bitset<32>(29), std::bitset<32>(28),
        std::bitset<32>(20), std::bitset<32>(21), std::bitset<32>(23), std::bitset<32>(22),
        std::bitset<32>(18), std::bitset<32>(19), std::bitset<32>(17), std::bitset<32>(16)
    };

    // 验证非法边界 n <= 0
    assert(bit_manipulation::gray_code::gray_code_generation(-1) == gray_code_negative_1);
    assert(bit_manipulation::gray_code::gray_code_generation(0) == gray_code_0);

    // 验证正常值 n
    assert(bit_manipulation::gray_code::gray_code_generation(1) == gray_code_1);
    assert(bit_manipulation::gray_code::gray_code_generation(2) == gray_code_2);
    assert(bit_manipulation::gray_code::gray_code_generation(3) == gray_code_3);
    assert(bit_manipulation::gray_code::gray_code_generation(4) == gray_code_4);
    assert(bit_manipulation::gray_code::gray_code_generation(5) == gray_code_5);
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行自测
    return 0;
}
