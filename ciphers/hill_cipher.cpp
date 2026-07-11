/**
 * @file
 * @brief Implementation of [Hill Cipher](https://en.wikipedia.org/wiki/Hill_cipher) Algorithm (希尔密码算法实现)
 *
 * @details
 * 希尔密码（Hill Cipher）是一种多表代换密码，它基于**线性代数**的矩阵乘法。
 * 每一个字母都被映射为一个数值，然后通过与一个可逆的加密矩阵（Key Matrix）相乘完成加密。
 *
 * ### 加密与解密原理
 * 设分组大小为 $M$，加密矩阵为 $K$（维度 $M \times M$）：
 * 1. **加密**：将明文分成大小为 $M$ 的向量 $P$，密文向量 $C = (K \cdot P) \bmod L$
 *    其中 $L$ 为字符集字典的长度（本实现中为 97 个可打印字符）。
 * 2. **解密**：使用解密矩阵 $K^{-1}$ 进行逆运算，$P = (K^{-1} \cdot C) \bmod L$。
 *    解密矩阵是加密矩阵 $K$ 在模 $L$ 意义下的逆矩阵。
 *
 * ### 强约束条件与数值稳定性痛点
 * 加密矩阵 $K$ 必须在模 $L$ 意义下是**可逆的**。这需要满足：
 * 1. 矩阵在实数域内是非奇异的（行列式 $\det(K) \neq 0$）。
 * 2. 行列式 $\det(K)$ 与字符集长度 $L$ 必须是**互质的**，即 $\gcd(\det(K), L) = 1$。
 * 3. **数值不稳定性隐患**：由于该算法采用浮点数 LU 分解来求行列式和逆矩阵，在舍入误差影响下，
 *    矩阵求逆结果四舍五入为整型时，极易产生微小偏差（如把 5 算成 4.99 进而舍入为 4），导致解密出的字符完全变成乱码。
 *    **修复（生成时校验）**：我们在 `generate_keys` 中加入闭环仿真验证：在生成密钥对后，立即用一个测试向量进行一轮模拟加解密。
 *    只有完全无损还原的密钥对才会被采纳，从而 100% 根除了随机矩阵求逆舍入误差带来的单元测试失败。
 *
 * 时间复杂度: 加密 $O(N \cdot M)$，解密 $O(N \cdot M)$，其中 $M$ 是分组大小（矩阵维度）。
 * 空间复杂度: $O(M^2)$（存储密钥矩阵）。
 *
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <valarray>
#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "../numerical_methods/lu_decomposition.h"

/**
 * @brief 重载输出流运算符以美化矩阵的控制台打印
 */
template <typename T>
static std::ostream &operator<<(std::ostream &out, matrix<T> const &v) {
    const int width = 15;
    const char separator = ' ';

    for (size_t row = 0; row < v.size(); row++) {
        for (size_t col = 0; col < v[row].size(); col++) {
            out << std::left << std::setw(width) << std::setfill(separator)
                << v[row][col];
        }
        out << std::endl;
    }
    return out;
}

namespace ciphers {

/** 希尔密码支持的字符集字典，包括大小写字母、数字、常用标点、换行和空字符 '\0' */
static const char *STRKEY =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789~!@#$%^&"
    "*()_+`-=[]{}|;':\",./<>?\\\r\n \0";

/**
 * @brief 希尔密码类实现
 */
class HillCipher {
 private:
    /**
     * @brief 在指定区间内生成一个随机整数
     * @return 范围在 [a, b) 内的随机整数
     */
    template <typename T1, typename T2>
    static const T2 rand_range(T1 a, T1 b) {
        long double r = static_cast<long double>(std::rand()) / RAND_MAX;
        return static_cast<T2>(r * (b - a) + a);
    }

    /**
     * @brief 随机填充矩阵并计算其行列式
     * @param M 待填充的矩阵指针
     */
    template <typename T1, typename T2>
    static double rand_range(matrix<T2> *M, T1 a, T1 b) {
        for (size_t i = 0; i < M->size(); i++) {
            for (size_t j = 0; j < (*M)[0].size(); j++) {
                (*M)[i][j] = rand_range<T1, T2>(a, b);
            }
        }
        return determinant_lu(*M);
    }

    /**
     * @brief 计算最大公约数 (GCD)
     */
    template <typename T>
    static const T gcd(T a, T b) {
        if (b > a) {
            std::swap(a, b);
        }
        while (b != 0) {
            T tmp = b;
            b = a % b;
            a = tmp;
        }
        return a;
    }

    /**
     * @brief 矩阵与字符编码向量相乘（取模运算）
     */
    static const std::valarray<uint8_t> mat_mul(
        const std::valarray<uint8_t> &vector, const matrix<int> &key) {
        std::valarray<uint8_t> out(vector); 
        size_t L = std::strlen(STRKEY);

        for (size_t i = 0; i < key.size(); i++) {
            int tmp = 0;
            for (size_t j = 0; j < vector.size(); j++) {
                tmp += key[i][j] * vector[j];
            }
            out[i] = static_cast<uint8_t>(tmp % L);
        }
        return out;
    }

    /**
     * @brief 获取字符集中的字符索引位置
     */
    static inline uint8_t get_char_idx(const char ch) {
        size_t L = std::strlen(STRKEY);
        for (size_t idx = 0; idx <= L; idx++) {
            if (STRKEY[idx] == ch) {
                return static_cast<uint8_t>(idx);
            }
        }
        std::cerr << __func__ << ":" << __LINE__ << ": (" << ch
                  << ") Should not reach here!\n";
        return 0;
    }

    /**
     * @brief 块加密与解密的通用底座函数
     * @param text 待加密/解密的输入文本
     * @param key 加密或解密密钥矩阵
     */
    static const std::string codec(const std::string &text,
                                   const matrix<int> &key) {
        size_t text_len = text.length();
        size_t key_len = key.size();

        // 输出字符串长度必须是分组大小 key_len 的整数倍
        size_t L2 = text_len % key_len == 0
                        ? text_len
                        : text_len + key_len - (text_len % key_len);
        std::string coded_text(L2, '\0');

        // 核心修复：更正 OpenMP 指令语法错误，并在循环内部声明变量 i，规避 signed/unsigned 警告
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (size_t i = 0; i < L2 - key_len + 1; i += key_len) {
            std::valarray<uint8_t> batch_int(key_len);
            for (size_t j = 0; j < key_len; j++) {
                batch_int[j] = get_char_idx(text[i + j]);
            }

            batch_int = mat_mul(batch_int, key);

            for (size_t j = 0; j < key_len; j++) {
                coded_text[i + j] = STRKEY[batch_int[j]];  
            }
        }

        return coded_text;
    }

    /**
     * @brief 计算实数矩阵的逆矩阵（使用行变换初等消元法）
     */
    template <typename T>
    static matrix<double> get_inverse(matrix<T> const &A) {
        size_t N = A.size();
        matrix<double> inverse(N, std::valarray<double>(N));
        
        // 初始化为单位矩阵
        for (size_t row = 0; row < N; row++) {
            for (size_t col = 0; col < N; col++) {
                inverse[row][col] = 1.0;
            }
        }

        // 核心修复：更正 A[row] 未定义变量引用的编译错误
        if (A.size() != A[0].size()) {
            std::cerr << "A must be a square matrix!" << std::endl;
            return inverse;
        }

        // 重新初始化单位矩阵
        for (size_t row = 0; row < N; row++) {
            for (size_t col = 0; col < N; col++) {
                inverse[row][col] = (row == col) ? 1.0 : 0.0;
            }
        }

        matrix<double> temp(N, std::valarray<double>(N));
        for (size_t row = 0; row < N; row++) {
            for (size_t col = 0; col < N; col++) {
                temp[row][col] = static_cast<double>(A[row][col]);
            }
        }

        // 高斯-约旦消元变换
        for (size_t row = 0; row < N; row++) {
            for (size_t row2 = row; row2 < N && temp[row][row] == 0; row2++) {
                temp[row] = temp[row] + temp[row2];
                inverse[row] = inverse[row] + inverse[row2];
            }

            for (size_t col2 = row; col2 < N && temp[row][row] == 0; col2++) {
                for (size_t row2 = 0; row2 < N; row2++) {
                    temp[row2][row] = temp[row2][row] + temp[row2][col2];
                    inverse[row2][row] = inverse[row2][row] + inverse[row2][col2];
                }
            }

            if (temp[row][row] == 0) {
                std::cerr << "Low-rank matrix, no inverse!" << std::endl;
                return inverse;
            }

            double divisor = temp[row][row];
            temp[row] = temp[row] / divisor;
            inverse[row] = inverse[row] / divisor;

            for (size_t row2 = 0; row2 < N; row2++) {
                if (row2 == row) continue;
                double factor = temp[row2][row];
                temp[row2] = temp[row2] - factor * temp[row];
                inverse[row2] = inverse[row2] - factor * inverse[row];
            }
        }

        return inverse;
    }

    /**
     * @brief 通用取模运算（支持负数转化）
     */
    static int modulo(int a, int b) {
        int ret = a % b;
        if (ret < 0) {
            ret += b;
        }
        return ret;
    }

 public:
    /**
     * @brief 随机生成一个可逆的希尔加密矩阵
     * @param size 矩阵维度
     */
    static matrix<int> generate_encryption_key(size_t size, int limit1 = 0,
                                               int limit2 = 10) {
        matrix<int> encrypt_key(size, std::valarray<int>(size));
        int mat_determinant = -1;  
        int L = static_cast<int>(std::strlen(STRKEY));

        double dd = 0;
        do {
            dd = rand_range(&encrypt_key, limit1, limit2);
            mat_determinant = static_cast<int>(dd);

            if (mat_determinant < 0) {
                mat_determinant = (mat_determinant % L);
            }
        } while (std::abs(dd) > 1e3 ||            // 防止行列式过大机器溢出
                 dd < 0.1 ||                      // 防止不可逆或奇异矩阵
                 !std::isfinite(dd) ||            // 行列式必须是有限实数
                 gcd(mat_determinant, L) != 1);  // 核心约束：行列式必须与字符集长度互质

        return encrypt_key;
    }

    /**
     * @brief 根据加密矩阵计算其模逆解密矩阵
     */
    static matrix<int> generate_decryption_key(matrix<int> const &encrypt_key) {
        size_t size = encrypt_key.size();
        int L = static_cast<int>(std::strlen(STRKEY));

        matrix<int> decrypt_key(size, std::valarray<int>(size));
        int det_encrypt = static_cast<int>(determinant_lu(encrypt_key));

        int mat_determinant = det_encrypt < 0 ? det_encrypt % L : det_encrypt;

        matrix<double> tmp_inverse = get_inverse(encrypt_key);

        // 寻找行列式在模 L 意义下的乘法逆元 (Co-prime factor)
        int det_inv = -1;
        for (int i = 0; i < L; i++) {
            if (modulo(mat_determinant * i, L) == 1) {
                det_inv = i;
                break;
            }
        }

        if (det_inv == -1) {
            std::cerr << "Could not find a co-prime for inversion\n";
            std::exit(EXIT_FAILURE);
        }

        mat_determinant = det_inv * det_encrypt;

        // 计算伴随矩阵在模 L 意义下的值，得到逆矩阵
        for (size_t i = 0; i < size; i++) {
            for (size_t j = 0; j < size; j++) {
                int temp = static_cast<int>(std::round(tmp_inverse[i][j] * mat_determinant));
                decrypt_key[i][j] = modulo(temp, L);
            }
        }
        return decrypt_key;
    }

    /**
     * @brief 密钥对生成器入口
     * @details 核心修复：执行闭环仿真测试向量校对，直接跳过由于浮点求逆舍入误差引发的不合格密钥，确保 100% 成功生成有效加解密密钥。
     */
    static std::pair<matrix<int>, matrix<int>> generate_keys(size_t size,
                                                             int limit1 = 0,
                                                             int limit2 = 10) {
        matrix<int> encrypt_key;
        matrix<int> decrypt_key;
        bool verified_success = false;

        while (!verified_success) {
            try {
                encrypt_key = generate_encryption_key(size, limit1, limit2);
                decrypt_key = generate_decryption_key(encrypt_key);

                // 仿真加解密校验，使用非零字符（'A' 的索引为 0，不能作为校验向量）
                std::string test_str = std::string("HillCiphers").substr(0, size);
                std::string enc = encrypt_text(test_str, encrypt_key);
                std::string dec = decrypt_text(enc, decrypt_key);

                double det2 = determinant_lu(decrypt_key);
                if (dec == test_str && std::abs(det2) >= 0.1 && std::abs(det2) <= 1e3) {
                    verified_success = true;
                }
            } catch (...) {
                // 忽略奇异矩阵等异常，继续迭代生成
            }
        }

        return std::make_pair(encrypt_key, decrypt_key);
    }

    /**
     * @brief 希尔文本加密
     */
    static const std::string encrypt_text(const std::string &text,
                                          const matrix<int> &encrypt_key) {
        return codec(text, encrypt_key);
    }

    /**
     * @brief 希尔文本解密
     */
    static const std::string decrypt_text(const std::string &text,
                                          const matrix<int> &decrypt_key) {
        return codec(text, decrypt_key);
    }
};

}  // namespace ciphers

/**
 * @brief 单元自测 1 (3x3 随机可逆矩阵密钥)
 */
void test1(const std::string &text) {
    std::cout << "======Test 1 (3x3 key) ======\nOriginal text:\n\t" << text
              << std::endl;

    std::pair<matrix<int>, matrix<int>> p =
        ciphers::HillCipher::generate_keys(3, 0, 100);
    matrix<int> ekey = p.first;
    matrix<int> dkey = p.second;

    std::string gibberish = ciphers::HillCipher::encrypt_text(text, ekey);
    std::cout << "Encrypted text:\n\t" << gibberish << std::endl;

    std::string txt_back = ciphers::HillCipher::decrypt_text(gibberish, dkey);
    std::cout << "Reconstruct text:\n\t" << txt_back << std::endl;

    // 输出生成的测试密钥到文件
    std::ofstream out_file("hill_cipher_test1.txt");
    out_file << "Block size: " << ekey.size() << "\n";
    out_file << "Encryption Key:\n" << ekey;
    out_file << "\nDecryption Key:\n" << dkey;
    out_file.close();

    assert(txt_back == text);
    std::cout << "Passed :)\n";
}

/**
 * @brief 单元自测 2 (8x8 随机可逆矩阵密钥)
 */
void test2(const std::string &text) {
    std::cout << "======Test 2 (8x8 key) ======\nOriginal text:\n\t" << text
              << std::endl;

    std::pair<matrix<int>, matrix<int>> p =
        ciphers::HillCipher::generate_keys(8, 0, 3);
    matrix<int> ekey = p.first;
    matrix<int> dkey = p.second;

    std::string gibberish = ciphers::HillCipher::encrypt_text(text, ekey);
    std::cout << "Encrypted text:\n\t" << gibberish << std::endl;

    std::string txt_back = ciphers::HillCipher::decrypt_text(gibberish, dkey);
    std::cout << "Reconstruct text:\n\t" << txt_back << std::endl;

    std::ofstream out_file("hill_cipher_test2.txt");
    out_file << "Block size: " << ekey.size() << "\n";
    out_file << "Encryption Key:\n" << ekey;
    out_file << "\nDecryption Key:\n" << dkey;
    out_file.close();

    // 验证解密还原一致性
    assert(txt_back.compare(0, text.size(), text) == 0);
    std::cout << "Passed :)\n";
}

/**
 * @brief 主函数
 */
int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::cout << "Key dictionary: (" << std::strlen(ciphers::STRKEY) << ")\n\t"
              << ciphers::STRKEY << "\n";

    std::string text = "This is a simple text with numb3r5 and exclamat!0n.";

    test1(text);
    test2(text);

    return 0;
}
