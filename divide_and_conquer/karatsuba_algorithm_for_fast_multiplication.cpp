/**
 * @file
 * @brief Implementation of the [Karatsuba algorithm for fast multiplication](https://en.wikipedia.org/wiki/Karatsuba_algorithm) (快速乘法卡拉楚巴算法实现)
 * @details
 * 给定两个用二进制字符串表示的数，计算它们的乘积并返回 64 位整数值。
 * 普通的长乘法逐位相乘需要 O(n^2) 的时间复杂度。
 * 卡拉楚巴（Karatsuba）算法采用分治策略，将乘法次数从 4 次减少到 3 次，从而将时间复杂度降低到 O(n^(log2(3))) ≈ O(n^1.585)。
 *
 * ### 算法原理（以二进制为例）
 * 设大数 X, Y 分别分割为高位（左半部分）与低位（右半部分）：
 *   X = Xl * 2^sh + Xr
 *   Y = Yl * 2^sh + Yr
 * 传统的乘法 X * Y = Xl*Yl * 2^(2*sh) + (Xl*Yr + Xr*Yl) * 2^sh + Xr*Yr。这包含 4 次子乘积。
 * 卡拉楚巴算法观察到：
 *   Xl*Yr + Xr*Yl = (Xl + Xr) * (Yl + Yr) - Xl*Yl - Xr*Yr
 * 于是我们只需计算三个子乘积：
 *   P1 = Xl * Yl
 *   P2 = Xr * Yr
 *   P3 = (Xl + Xr) * (Yl + Yr)
 * 最终乘积为：P1 * 2^(2*sh) + (P3 - P1 - P2) * 2^sh + P2。
 * 
 * @author [Swastika Gupta](https://github.com/Swastyy)
 * @author [Ameer Carlo Lubang](https://github.com/poypoyan)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstring>   /// 用于 std::string 字符串操作
#include <iostream>  /// 用于输入输出

/**
 * @namespace divide_and_conquer
 * @brief 分治算法命名空间
 */
namespace divide_and_conquer {
/**
 * @namespace karatsuba_algorithm
 * @brief 卡拉楚巴快速乘法相关命名空间
 */
namespace karatsuba_algorithm {
/**
 * @brief 两个二进制字符串相加
 * @param first 输入二进制串一
 * @param second 输入二进制串二
 * @returns 累加得到的二进制字符串
 */
std::string add_strings(std::string first, std::string second) {
    std::string result;

    // 前导零对齐，使两字符串长度相同
    int64_t len1 = first.size();
    int64_t len2 = second.size();
    std::string zero = "0";
    if (len1 < len2) {
        for (int64_t i = 0; i < len2 - len1; i++) {
            zero += first;
            first = zero;
            zero = "0";
        }
    } else if (len1 > len2) {
        for (int64_t i = 0; i < len1 - len2; i++) {
            zero += second;
            second = zero;
            zero = "0";
        }
    }

    int64_t length = std::max(len1, len2);
    int64_t carry = 0; // 进位标志
    for (int64_t i = length - 1; i >= 0; i--) {
        int64_t firstBit = first.at(i) - '0';
        int64_t secondBit = second.at(i) - '0';

        // 异或计算当前位的值
        int64_t sum = (char(firstBit ^ secondBit ^ carry)) + '0';
        result.insert(result.begin(), sum);

        // 计算新进位
        carry = char((firstBit & secondBit) | (secondBit & carry) |
                (firstBit & carry));
    }

    // 处理最后的溢出进位
    if (carry) {
        result.insert(result.begin(), '1');
    }
    return result;
}

/**
 * @brief 安全截取子串的包装函数，处理隐式的前导零
 * @param str 二进制输入串
 * @param x1 截取起始索引
 * @param x2 截取子串长度
 * @param n 全局最长补齐长度
 * @returns 对应子区间的二进制字符串，不足处视作 '0'
 */
std::string safe_substr(const std::string &str, int64_t x1, int64_t x2, int64_t n) {
    int64_t len = str.size();

    if (len >= n) {
        return str.substr(x1, x2);
    }

    int64_t y1 = x1 - (n - len);  // 对应在实际 str 中的左起始映射索引
    int64_t y2 = (x1 + x2 - 1) - (n - len);  // 对应在实际 str 中的右终止映射索引

    if (y2 < 0) {
        return "0"; // 落在前导零区域
    } else if (y1 < 0) {
        return str.substr(0, y2 + 1); // 部分落在前导零区域，截取 str 头部
    } else {
        return str.substr(y1, x2); // 完全在 str 实部区域
    }
}

/**
 * @brief 分治递归求解 Karatsuba 乘法
 * @param str1 二进制大数一
 * @param str2 二进制大数二
 * @returns 乘积的 64 位整数值
 */
int64_t karatsuba_algorithm(std::string str1, std::string str2) {
    int64_t len1 = str1.size();
    int64_t len2 = str2.size();
    int64_t n = std::max(len1, len2);

    if (n == 0) {
        return 0;
    }
    if (n == 1) {
        return (str1[0] - '0') * (str2[0] - '0'); // 单比特直接乘
    }

    int64_t fh = n / 2;   // 高位长度
    int64_t sh = n - fh;  // 低位长度

    // 将大数划分为高位 (l) 和低位 (r) 两个部分
    std::string Xl = divide_and_conquer::karatsuba_algorithm::safe_substr(str1, 0, fh, n);
    std::string Xr = divide_and_conquer::karatsuba_algorithm::safe_substr(str1, fh, sh, n);

    std::string Yl = divide_and_conquer::karatsuba_algorithm::safe_substr(str2, 0, fh, n);
    std::string Yr = divide_and_conquer::karatsuba_algorithm::safe_substr(str2, fh, sh, n);

    // 递归计算 3 个核心子乘积
    int64_t product1 = karatsuba_algorithm(Xl, Yl); // P1 = Xl * Yl
    int64_t product2 = karatsuba_algorithm(Xr, Yr); // P2 = Xr * Yr
    int64_t product3 = karatsuba_algorithm(
        divide_and_conquer::karatsuba_algorithm::add_strings(Xl, Xr),
        divide_and_conquer::karatsuba_algorithm::add_strings(Yl, Yr)); // P3 = (Xl + Xr) * (Yl + Yr)

    // 合并子乘积：product1 * 2^(2*sh) + (product3 - product1 - product2) * 2^sh + product2
    return product1 * (1 << (2 * sh)) +
           (product3 - product1 - product2) * (1 << sh) +
           product2;
}
}  // namespace karatsuba_algorithm
}  // namespace divide_and_conquer

/**
 * @brief 单元自测用例
 */
static void test() {
    // 1st test: 1 * 10 = 10
    std::string s11 = "1";
    std::string s12 = "1010";
    std::cout << "1st test... ";
    assert(divide_and_conquer::karatsuba_algorithm::karatsuba_algorithm(s11, s12) == 10);
    std::cout << "passed" << std::endl;

    // 2nd test: 3 * 10 = 30
    std::string s21 = "11";
    std::string s22 = "1010";
    std::cout << "2nd test... ";
    assert(divide_and_conquer::karatsuba_algorithm::karatsuba_algorithm(s21, s22) == 30);
    std::cout << "passed" << std::endl;

    // 3rd test: 6 * 10 = 60
    std::string s31 = "110";
    std::string s32 = "1010";
    std::cout << "3rd test... ";
    assert(divide_and_conquer::karatsuba_algorithm::karatsuba_algorithm(s31, s32) == 60);
    std::cout << "passed" << std::endl;
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行自测
    return 0;
}
