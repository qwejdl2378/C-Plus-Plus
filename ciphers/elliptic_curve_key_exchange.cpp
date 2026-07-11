/**
 * @file
 * @brief Implementation of [Elliptic Curve Diffie-Hellman (ECDH) Key Exchange](https://cryptobook.nakov.com/asymmetric-key-ciphers/ecdh-key-exchange) (椭圆曲线迪菲-赫尔曼密钥交换算法实现)
 *
 * @details
 * ECDH 是一种匿名的密钥协商协议，它允许两方（Alice 和 Bob）在不安全的信道上建立共享密钥。
 * 它基于椭圆曲线密码学（ECC）的离散对数难题。
 *
 * ### 核心数学原理
 * 设椭圆曲线基点为 $G$，Alice 的私钥为 $a$，其公钥为 $A = a \cdot G$；
 * Bob 的私钥为 $b$，其公钥为 $B = b \cdot G$。
 * - Alice 计算：$SharedKey = a \cdot B = a \cdot (b \cdot G)$
 * - Bob 计算：$SharedKey = b \cdot A = b \cdot (a \cdot G)$
 * 由于标点乘法的结合律：$a \cdot (b \cdot G) = b \cdot (a \cdot G)$，双方能计算出完全相同的共享秘钥。
 *
 * 时间复杂度: $O(\log(\text{privKey}))$ 标点点乘
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【椭圆曲线无穷远点加法退化与越界计算 Bug 审计与修复】：
 * 1. **无穷远点相加溢出 Bug**：在点加法函数 `addition` 中，
 *    代码使用 `inf = ~uint256_t(0)`（即最大 256 位无符号数）表示无穷远点。
 *    但原代码没有在 `addition` 开始处判断任何一个输入点是否已经是无穷远点。
 *    如果将 `{inf, inf}` 传入点加，会执行 `den = b.x - a.x`，导致大数溢出计算出非法的坐标值，从而无法正确返回相加后的点。
 *    **修复**：在 `addition` 函数最开头增加标准的无穷远点单位元判别：
 *    `if (a.x == inf && a.y == inf) return b;`
 *    `if (b.x == inf && b.y == inf) return a;`
 *    这保证了 $P + O = P$ 的椭圆曲线群基本加法性质。
 *
 * @author [Ashish Daulatabad](https://github.com/AshishYUO)
 */

#include <cassert>   /// 用于 assert 断言
#include <iostream>  /// 用于标准输出
#include "uint256_t.hpp"  /// 引入 256 位无符号整型支持

namespace ciphers {
namespace elliptic_curve_key_exchange {

/**
 * @brief 椭圆曲线上的二维坐标点结构体
 */
typedef struct Point {
    uint256_t x, y;  ///< 横纵坐标

    /**
     * @brief 判等运算符重载
     */
    inline bool operator==(const Point &p) { return x == p.x && y == p.y; }

    /**
     * @brief 打印点坐标的输出流运算符重载
     */
    friend std::ostream &operator<<(std::ostream &op, const Point &p) {
        op << p.x << " " << p.y;
        return op;
    }
} Point;

/**
 * @brief 高精度快速幂取模运算（Modular Exponentiation）
 * @param number 底数
 * @param power 指数
 * @param mod 模数
 * @return (number ^ power) % mod
 */
uint256_t exp(uint256_t number, uint256_t power, const uint256_t &mod) {
    if (!power) {
        return uint256_t(1);
    }
    uint256_t ans(1);
    number = number % mod;
    while (power) {
        if ((power & 1)) {
            ans = (ans * number) % mod;
        }
        power >>= 1;
        if (power) {
            number = (number * number) % mod;
        }
    }
    return ans;
}

/**
 * @brief 椭圆曲线点加法（Point Addition）
 * @param a 加数点 A
 * @param b 加数点 B
 * @param curve_a_coeff 椭圆曲线方程中的系数 a (y^2 = x^3 + ax + b)
 * @param mod 有限域模数 p
 * @return 相加后的新点坐标
 */
Point addition(Point a, Point b, const uint256_t &curve_a_coeff,
               uint256_t mod) {
    uint256_t zero(0);
    uint256_t inf = ~zero; // 定义无穷远点坐标标识值

    // 核心修复：处理无穷远点（单位元）加法性质：P + O = O + P = P
    if (a.x == inf && a.y == inf) {
        return b;
    }
    if (b.x == inf && b.y == inf) {
        return a;
    }

    uint256_t lambda(0);  /// 斜率

    // 如果两个点互不相同
    if (a.x != b.x || a.y != b.y) {
        // 若横坐标相同而纵坐标不同，表示两点关于 X 轴对称，其和为无穷远点
        if (b.x == a.x) {
            return {inf, inf};
        }
        // 计算两点连线斜率：lambda = (y2 - y1) / (x2 - x1) % mod
        uint256_t num = (b.y - a.y + mod), den = (b.x - a.x + mod);
        // 使用费马小定理求 den 的逆元进行除法取模
        lambda = (num * (exp(den, mod - 2, mod))) % mod;
    } else {
        // 如果两个点相同（点翻倍 / Point Doubling）
        // y 坐标为 0 时切线垂直，切点和为无穷远点
        if (!a.y) {
            return {inf, inf};
        }
        // 计算切线斜率：lambda = (3 * x1^2 + a_coeff) / (2 * y1) % mod
        uint256_t axsq = ((a.x * a.x)) % mod;
        axsq += (axsq << 1); // 乘 3 优化 (axsq * 3)
        axsq %= mod;
        uint256_t a_2 = (a.y << 1); // 乘 2 优化 (2 * y1)
        lambda = (((axsq + curve_a_coeff) % mod) * exp(a_2, mod - 2, mod)) % mod;
    }
    
    Point c;
    // 计算相加后的新坐标：
    // x3 = lambda^2 - x1 - x2
    // y3 = lambda * (x1 - x3) - y1
    c.x = ((lambda * lambda) % mod + (mod << 1) - a.x - b.x) % mod;
    c.y = (((lambda * (a.x + mod - c.x)) % mod) + mod - a.y) % mod;
    return c;
}

/**
 * @brief 椭圆曲线标量乘法（Point Multiplication - 双倍与相加法）
 * @param a 基点 G
 * @param curve_a_coeff 椭圆曲线方程中的系数 a
 * @param p 标量因子（私钥）
 * @param mod 有限域模数 p
 * @return 乘积点坐标
 */
Point multiply(const Point &a, const uint256_t &curve_a_coeff, uint256_t p,
               const uint256_t &mod) {
    Point N = a;
    N.x %= mod;
    N.y %= mod;
    uint256_t inf = ~uint256_t(0);
    Point Q = {inf, inf}; // 初始化 Q 为无穷远点

    while (p) {
        if ((p & 1)) {
            if (Q.x == inf && Q.y == inf) {
                Q.x = N.x;
                Q.y = N.y;
            } else {
                Q = addition(Q, N, curve_a_coeff, mod);
            }
        }
        p >>= 1;
        if (p) {
            N = addition(N, N, curve_a_coeff, mod);
        }
    }
    return Q;
}
}  // namespace elliptic_curve_key_exchange
}  // namespace ciphers

/**
 * @brief 单元测试：验证 uint128_t 基础大数运算
 */
static void uint128_t_tests() {
    uint128_t a("122"), b("2312");
    assert(a + b == 2434);
    assert(b - a == 2190);
    assert(a * b == 282064);
    assert(b / a == 18);
    assert(b % a == 116);
    assert((a & b) == 8);
    assert((a | b) == 2426);
    assert((a ^ b) == 2418);
    assert((a << 64) == uint128_t("2250502776992565297152"));
    assert((b >> 7) == 18);

    a = uint128_t("12321421424232142122");
    b = uint128_t("23123212");
    assert(a + b == uint128_t("12321421424255265334"));
    assert(a - b == uint128_t("12321421424209018910"));
    assert(a * b == uint128_t("284910839733861759501135864"));
    assert(a / b == 532859423865LL);
    assert(a % b == 3887742);
    assert((a & b) == 18912520);
    assert((a | b) == uint128_t("12321421424236352814"));
    assert((a ^ b) == uint128_t("12321421424217440294"));
    assert((a << 64) == uint128_t("227290107637132170748078080907806769152"));
}

/**
 * @brief 单元测试：验证 uint256_t 基础大数运算
 */
static void uint256_t_tests() {
    uint256_t a("122"), b("2312");
    assert(a + b == 2434);
    assert(b - a == 2190);
    assert(a * b == 282064);
    assert(b / a == 18);
    assert(b % a == 116);
    assert((a & b) == 8);
    assert((a | b) == 2426);
    assert((a ^ b) == 2418);
    assert((a << 64) == uint256_t("2250502776992565297152"));
    assert((b >> 7) == 18);

    a = uint256_t("12321423124513251424232142122");
    b = uint256_t("23124312431243243215354315132413213212");
    assert(a + b == uint256_t("23124312443564666339867566556645355334"));
    assert(a - b == uint256_t("115792089237316195423570985008687907853246860353"
                              "221642219366742944204948568846"));
    assert(a * b == uint256_t("284924437928789743312147393953938013677909398222"
                              "169728183872115864"));
    assert(b / a == uint256_t("1876756621"));
    assert(b % a == uint256_t("2170491202688962563936723450"));
    assert((a & b) == uint256_t("3553901085693256462344"));
    assert((a | b) == uint256_t("23124312443564662785966480863388892990"));
    assert((a ^ b) == uint256_t("23124312443564659232065395170132430646"));
    assert((a << 128) == uint256_t("4192763024643754272961909047609369343091683"
                                   "376561852756163540549632"));
}

/**
 * @brief ECDH 密钥交换测试
 */
static void test() {
    // 使用 secp112r1 曲线进行密钥交换测试
    // y^2 = (x^3 + ax + b) % mod
    uint256_t a("4451685225093714772084598273548424"),
        b("2061118396808653202902996166388514"),
        mod("4451685225093714772084598273548427");

    // 曲线的基点 G (Generator Point)
    ciphers::elliptic_curve_key_exchange::Point ptr = {
        uint256_t("188281465057972534892223778713752"),
        uint256_t("3419875491033170827167861896082688")};

    // 1. Alice 生成密钥对
    std::cout << "For Alice:\n";
    uint256_t alice_private_key("164330438812053169644452143505618");
    ciphers::elliptic_curve_key_exchange::Point alice_public_key =
        multiply(ptr, a, alice_private_key, mod);
    std::cout << "\tPrivate key: " << alice_private_key << "\n";
    std::cout << "\tPublic Key: " << alice_public_key << "\n";

    // 2. Bob 生成密钥对
    std::cout << "For Bob:\n";
    uint256_t bob_private_key("1959473333748537081510525763478373");
    ciphers::elliptic_curve_key_exchange::Point bob_public_key =
        multiply(ptr, a, bob_private_key, mod);
    std::cout << "\tPrivate key: " << bob_private_key << "\n";
    std::cout << "\tPublic Key: " << bob_public_key << "\n";

    // 3. 交换公钥后各自计算共享密钥
    ciphers::elliptic_curve_key_exchange::Point alice_shared_key = multiply(
                                                    bob_public_key, a,
                                                    alice_private_key, mod);
    ciphers::elliptic_curve_key_exchange::Point bob_shared_key = multiply(
                                                    alice_public_key, a,
                                                    bob_private_key, mod);

    std::cout << "Shared keys:\n";
    std::cout << "\tAlice's derived shared key: " << alice_shared_key << "\n";
    std::cout << "\tBob's derived shared key:   " << bob_shared_key << "\n";

    // 4. 验证共享密钥是否一致
    assert(alice_shared_key == bob_shared_key);
    std::cout << "ECDH Key Exchange test passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    uint128_t_tests();  // 运行 128 位大数运算自测
    uint256_t_tests();  // 运行 256 位大数运算自测
    test();             // 运行 ECDH 密钥交换测试
    return 0;
}
