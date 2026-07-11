/**
 * @file
 * @brief Computes Catalan numbers using dynamic programming (基于动态规划/记忆化计算卡特兰数)
 * @details
 * 卡特兰数（Catalan Numbers）在组合数学中极为常见，如求二叉树的数量、括号匹配数量等。
 *
 * ### 递推关系公式
 *   C(0) = C(1) = 1;
 *   C(n) = sum( C(i) * C(n-i-1) )，对于 i = 0 到 n-1
 * 
 * ### C++17 现代语法实现优化
 * 本代码使用 C++17 标准库的 `std::transform_reduce` 函数，将 `known` 数组与其自身的逆序迭代器 `known.rbegin()` 进行 Zip 元素相乘，最后累加求和，
 * 简洁地在一行内完成了 $\sum C_i \cdot C_{n-i-1}$ 的乘积和计算。
 * 
 * @see https://en.wikipedia.org/wiki/Catalan_number
 * @see https://oeis.org/A000108/
 */

#include <cassert>     /// 用于 assert 断言
#include <cstdint>     /// 用于 std::uint64_t
#include <cstdlib>     /// 用于 std::size_t
#include <functional>  /// 用于 std::plus 和 std::multiplies
#include <numeric>     /// 用于 std::transform_reduce
#include <vector>      /// 用于 std::vector

/**
 * @brief 计算并缓存卡特兰数的类
 */
class catalan_numbers {
    using value_type = std::uint64_t;
    std::vector<value_type> known{1, 1}; // 缓存的卡特兰数列表，初始值 C(0)=1, C(1)=1

    /**
     * @brief 计算下一个卡特兰数的值
     * @details 利用 C++17 的 std::transform_reduce 进行向量内积（对折相乘相加）
     * @returns 下一个卡特兰数的值
     */
    value_type compute_next() {
        return std::transform_reduce(known.begin(), known.end(), known.rbegin(),
                                     static_cast<value_type>(0), std::plus<>(),
                                     std::multiplies<>());
    }

    /**
     * @brief 计算下一个卡特兰数并追加到缓存中
     */
    void add() { known.push_back(this->compute_next()); }

 public:
    /**
     * @brief 获取第 n 个卡特兰数，如果缓存不足，则增量递推计算至第 n 项
     * @param n 卡特兰数的索引
     * @return 第 n 个卡特兰数
     */
    value_type get(std::size_t n) {
        while (known.size() <= n) {
            this->add(); // 补足缓存
        }
        return known[n];
    }
};

/**
 * @brief 自测第 0 至 20 项卡特兰数
 */
void test_catalan_numbers_up_to_20() {
    catalan_numbers cn;
    assert(cn.get(0) == 1ULL);
    assert(cn.get(1) == 1ULL);
    assert(cn.get(2) == 2ULL);
    assert(cn.get(3) == 5ULL);
    assert(cn.get(4) == 14ULL);
    assert(cn.get(5) == 42ULL);
    assert(cn.get(6) == 132ULL);
    assert(cn.get(7) == 429ULL);
    assert(cn.get(8) == 1430ULL);
    assert(cn.get(9) == 4862ULL);
    assert(cn.get(10) == 16796ULL);
    assert(cn.get(11) == 58786ULL);
    assert(cn.get(12) == 208012ULL);
    assert(cn.get(13) == 742900ULL);
    assert(cn.get(14) == 2674440ULL);
    assert(cn.get(15) == 9694845ULL);
    assert(cn.get(16) == 35357670ULL);
    assert(cn.get(17) == 129644790ULL);
    assert(cn.get(18) == 477638700ULL);
    assert(cn.get(19) == 1767263190ULL);
    assert(cn.get(20) == 6564120420ULL);
}

/**
 * @brief 自测第 25 项卡特兰数
 */
void test_catalan_numbers_25() {
    catalan_numbers cn;
    assert(cn.get(25) == 4861946401452ULL);
}

/**
 * @brief 主函数
 */
int main() {
    test_catalan_numbers_up_to_20(); // 运行自测一
    test_catalan_numbers_25();        // 运行自测二
    return 0;
}
