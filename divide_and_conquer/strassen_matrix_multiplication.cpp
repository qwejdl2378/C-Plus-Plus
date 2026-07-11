/**
 * @brief [Strassen's algorithm](https://en.wikipedia.org/wiki/Strassen_algorithm) matrix multiplication (施特拉森矩阵乘法分治算法)
 * @details
 * 施特拉森（Strassen）算法是矩阵乘法的一种经典分治算法。
 * 传统的矩阵乘法需要进行 8 次子矩阵乘法，时间复杂度为 O(N^3)。
 * 施特拉森算法通过将子矩阵乘法次数减少到 7 次，使得时间复杂度降至 O(N^log2(7)) ≈ O(N^2.8073)。
 * 
 * ### 施特拉森算法原理
 * 设有两个分块矩阵进行乘法：
 * [ A  B ]   [ E  F ]   [ C11 C12 ]
 * [ C  D ] * [ G  H ] = [ C21 C22 ]
 * 计算以下 7 个中间乘积（递归调用）：
 *   P1 = A * (F - H)
 *   P2 = (A + B) * H
 *   P3 = (C + D) * E
 *   P4 = D * (G - E)
 *   P5 = (A + D) * (E + H)
 *   P6 = (B - D) * (G + H)
 *   P7 = (A - C) * (E + F)
 * 组合最终子块：
 *   C11 = P5 + P4 - P2 + P6
 *   C12 = P1 + P2
 *   C21 = P3 + P4
 *   C22 = P1 + P5 - P3 - P7
 * 
 * @author [AshishYUO](https://github.com/AshishYUO)
 */
#include <cassert>   /// 用于 assert 断言
#include <chrono>    /// 用于时间测量
#include <iostream>  /// 用于输入输出
#include <tuple>     /// 用于 std::tuple 元组
#include <vector>    /// 用于 std::vector 动态数组

/**
 * @namespace divide_and_conquer
 * @brief 分治算法命名空间
 */
namespace divide_and_conquer {

/**
 * @namespace strassens_multiplication
 * @brief 施特拉森矩阵乘法相关命名空间
 */
namespace strassens_multiplication {

/// 定义最大尺寸的静态常量
constexpr size_t MAX_SIZE = ~0ULL;

/**
 * @class Matrix
 * @brief 支持施特拉森和朴素算法的二维矩阵类模板
 * @tparam T 元素类型（通过 SFINAE 限制仅限整型和浮点型）
 */
template <typename T,
          typename = typename std::enable_if<
              std::is_integral<T>::value || std::is_floating_point<T>::value,
              bool>::type>
class Matrix {
    std::vector<std::vector<T>> _mat; // 二维数据向量

 public:
    /**
     * @brief 构造函数：指定长宽相等的正方形矩阵，元素初始化为 0
     * @param size 矩阵大小
     */
    template <typename Integer,
              typename = typename std::enable_if<
                  std::is_integral<Integer>::value, Integer>::type>
    explicit Matrix(const Integer size) {
        for (size_t i = 0; i < size; ++i) {
            _mat.emplace_back(std::vector<T>(size, 0));
        }
    }

    /**
     * @brief 构造函数：任意长宽矩阵，元素初始化为 0
     * @param rows 行数
     * @param cols 列数
     */
    template <typename Integer,
              typename = typename std::enable_if<
                  std::is_integral<Integer>::value, Integer>::type>
    Matrix(const Integer rows, const Integer cols) {
        for (size_t i = 0; i < rows; ++i) {
            _mat.emplace_back(std::vector<T>(cols, 0));
        }
    }

    /**
     * @brief 获取矩阵的大小
     * @returns {行数, 列数} 的 std::pair
     */
    inline std::pair<size_t, size_t> size() const {
        return {_mat.size(), _mat[0].size()};
    }

    /**
     * @brief 下标运算符重载，获取第 index 行
     * @param index 行数索引
     * @returns 第 index 行的 std::vector 引用
     */
    template <typename Integer,
              typename = typename std::enable_if<
                  std::is_integral<Integer>::value, Integer>::type>
    inline std::vector<T> &operator[](const Integer index) {
        return _mat[index];
    }

    /**
     * @brief 切片函数：获取矩阵中指定的子矩阵
     * @param row_start 行起始位置
     * @param row_end 行结束位置
     * @param col_start 列起始位置
     * @param col_end 列结束位置
     * @returns 新生成的切片子矩阵
     */
    Matrix slice(const size_t row_start, const size_t row_end = MAX_SIZE,
                 const size_t col_start = MAX_SIZE,
                 const size_t col_end = MAX_SIZE) const {
        const size_t h_size =
            (row_end != MAX_SIZE ? row_end : _mat.size()) - row_start;
        const size_t v_size = (col_end != MAX_SIZE ? col_end : _mat[0].size()) -
                              (col_start != MAX_SIZE ? col_start : 0);
        Matrix result = Matrix<T>(h_size, v_size);

        const size_t v_start = (col_start != MAX_SIZE ? col_start : 0);
        for (size_t i = 0; i < h_size; ++i) {
            for (size_t j = 0; j < v_size; ++j) {
                result._mat[i][j] = _mat[i + row_start][j + v_start];
            }
        }
        return result;
    }

    /**
     * @brief 水平合并：将另一个矩阵拼接在当前矩阵右侧
     * @param other 待合并矩阵
     */
    template <typename Number, typename = typename std::enable_if<
                                   std::is_integral<Number>::value ||
                                       std::is_floating_point<Number>::value,
                                   Number>::type>
    void h_stack(const Matrix<Number> &other) {
        assert(_mat.size() == other._mat.size());
        for (size_t i = 0; i < other._mat.size(); ++i) {
            for (size_t j = 0; j < other._mat[i].size(); ++j) {
                _mat[i].push_back(other._mat[i][j]);
            }
        }
    }

    /**
     * @brief 垂直合并：将另一个矩阵拼接在当前矩阵下方
     * @param other 待合并矩阵
     */
    template <typename Number, typename = typename std::enable_if<
                                   std::is_integral<Number>::value ||
                                       std::is_floating_point<Number>::value,
                                   Number>::type>
    void v_stack(const Matrix<Number> &other) {
        assert(_mat[0].size() == other._mat[0].size());
        for (size_t i = 0; i < other._mat.size(); ++i) {
            _mat.emplace_back(std::vector<T>(other._mat[i].size()));
            for (size_t j = 0; j < other._mat[i].size(); ++j) {
                _mat.back()[j] = other._mat[i][j];
            }
        }
    }

    /**
     * @brief 运算符重载 + ：两个矩阵相加
     * @param other 加数矩阵
     * @returns 新相加后的矩阵
     */
    template <typename Number, typename = typename std::enable_if<
                                   std::is_integral<Number>::value ||
                                       std::is_floating_point<Number>::value,
                                   bool>::type>
    Matrix operator+(const Matrix<Number> &other) const {
        assert(this->size() == other.size());
        Matrix C = Matrix<Number>(_mat.size(), _mat[0].size());
        for (size_t i = 0; i < _mat.size(); ++i) {
            for (size_t j = 0; j < _mat[i].size(); ++j) {
                C._mat[i][j] = _mat[i][j] + other._mat[i][j];
            }
        }
        return C;
    }

    /**
     * @brief 运算符重载 +=
     */
    template <typename Number, typename = typename std::enable_if<
                                   std::is_integral<Number>::value ||
                                       std::is_floating_point<Number>::value,
                                   bool>::type>
    Matrix &operator+=(const Matrix<Number> &other) const {
        assert(this->size() == other.size());
        for (size_t i = 0; i < _mat.size(); ++i) {
            for (size_t j = 0; j < _mat[i].size(); ++j) {
                _mat[i][j] += other._mat[i][j];
            }
        }
        return this;
    }

    /**
     * @brief 运算符重载 - ：两个矩阵相减
     */
    template <typename Number, typename = typename std::enable_if<
                                   std::is_integral<Number>::value ||
                                       std::is_floating_point<Number>::value,
                                   bool>::type>
    Matrix operator-(const Matrix<Number> &other) const {
        assert(this->size() == other.size());
        Matrix C = Matrix<Number>(_mat.size(), _mat[0].size());
        for (size_t i = 0; i < _mat.size(); ++i) {
            for (size_t j = 0; j < _mat[i].size(); ++j) {
                C._mat[i][j] = _mat[i][j] - other._mat[i][j];
            }
        }
        return C;
    }

    /**
     * @brief 运算符重载 -=
     */
    template <typename Number, typename = typename std::enable_if<
                                   std::is_integral<Number>::value ||
                                       std::is_floating_point<Number>::value,
                                   bool>::type>
    Matrix &operator-=(const Matrix<Number> &other) const {
        assert(this->size() == other.size());
        for (size_t i = 0; i < _mat.size(); ++i) {
            for (size_t j = 0; j < _mat[i].size(); ++j) {
                _mat[i][j] -= other._mat[i][j];
            }
        }
        return this;
    }

    /**
     * @brief 运算符重载 * ：两个矩阵相乘
     * @details
     * 会自动检测矩阵是否满足施特拉森算法的基础条件（正方形且边长为偶数）。
     * 若满足，则使用施特拉森快速乘法，否则使用朴素矩阵乘法。
     */
    template <typename Number, typename = typename std::enable_if<
                                   std::is_integral<Number>::value ||
                                       std::is_floating_point<Number>::value,
                                   bool>::type>
    inline Matrix operator*(const Matrix<Number> &other) const {
        assert(_mat[0].size() == other._mat.size());
        auto size = this->size();
        const size_t row = size.first, col = size.second;
        return (row == col && (row & 1) == 0)
                   ? this->strassens_multiplication(other)
                   : this->naive_multiplication(other);
    }

    /**
     * @brief 运算符重载 * ：标量乘法
     */
    template <typename Number, typename = typename std::enable_if<
                                   std::is_integral<Number>::value ||
                                       std::is_floating_point<Number>::value,
                                   bool>::type>
    inline Matrix operator*(const Number other) const {
        Matrix C = Matrix<Number>(_mat.size(), _mat[0].size());
        for (size_t i = 0; i < _mat.size(); ++i) {
            for (size_t j = 0; j < _mat[i].size(); ++j) {
                C._mat[i][j] = _mat[i][j] * other;
            }
        }
        return C;
    }

    /**
     * @brief 运算符重载 *= ：标量乘法赋值
     */
    template <typename Number, typename = typename std::enable_if<
                                   std::is_integral<Number>::value ||
                                       std::is_floating_point<Number>::value,
                                   bool>::type>
    Matrix &operator*=(const Number other) const {
        for (size_t i = 0; i < _mat.size(); ++i) {
            for (size_t j = 0; j < _mat[i].size(); ++j) {
                _mat[i][j] *= other;
            }
        }
        return this;
    }

    /**
     * @brief 朴素矩阵相乘（O(N^3) 实现）
     */
    template <typename Number, typename = typename std::enable_if<
                                   std::is_integral<Number>::value ||
                                       std::is_floating_point<Number>::value,
                                   bool>::type>
    Matrix naive_multiplication(const Matrix<Number> &other) const {
        Matrix C = Matrix<Number>(_mat.size(), other._mat[0].size());

        for (size_t i = 0; i < _mat.size(); ++i) {
            for (size_t k = 0; k < _mat[0].size(); ++k) {
                for (size_t j = 0; j < other._mat[0].size(); ++j) {
                    C._mat[i][j] += _mat[i][k] * other._mat[k][j];
                }
            }
        }
        return C;
    }

    /**
     * @brief 施特拉森矩阵相乘分治算法
     * @details
     * 当矩阵规模小于等于 64 或者当前大小为奇数时，分治带来的开销会超过优化优势，
     * 此时退化使用 naive_multiplication。
     */
    template <typename Number, typename = typename std::enable_if<
                                   std::is_integral<Number>::value ||
                                       std::is_floating_point<Number>::value,
                                   bool>::type>
    Matrix strassens_multiplication(const Matrix<Number> &other) const {
        const size_t size = _mat.size();
        if (size <= 64ULL || (size & 1ULL)) {
            return this->naive_multiplication(other);
        } else {
            // 切割子块矩阵
            const Matrix<Number>
                A = this->slice(0ULL, size >> 1, 0ULL, size >> 1),
                B = this->slice(0ULL, size >> 1, size >> 1, size),
                C = this->slice(size >> 1, size, 0ULL, size >> 1),
                D = this->slice(size >> 1, size, size >> 1, size),
                E = other.slice(0ULL, size >> 1, 0ULL, size >> 1),
                F = other.slice(0ULL, size >> 1, size >> 1, size),
                G = other.slice(size >> 1, size, 0ULL, size >> 1),
                H = other.slice(size >> 1, size, size >> 1, size);

            // 递归求 7 个中间乘积
            Matrix P1 = A.strassens_multiplication(F - H);
            Matrix P2 = (A + B).strassens_multiplication(H);
            Matrix P3 = (C + D).strassens_multiplication(E);
            Matrix P4 = D.strassens_multiplication(G - E);
            Matrix P5 = (A + D).strassens_multiplication(E + H);
            Matrix P6 = (B - D).strassens_multiplication(G + H);
            Matrix P7 = (A - C).strassens_multiplication(E + F);

            // 构建最终的 4 个子块
            Matrix C11 = P5 + P4 - P2 + P6;
            Matrix C12 = P1 + P2;
            Matrix C21 = P3 + P4;
            Matrix C22 = P1 + P5 - P3 - P7;

            // 合并这 4 个子块
            C21.h_stack(C22);
            C11.h_stack(C12);
            C11.v_stack(C21);

            return C11;
        }
    }

    /**
     * @brief 矩阵相等性比较
     */
    bool operator==(const Matrix<T> &other) const {
        if (_mat.size() != other._mat.size() ||
            _mat[0].size() != other._mat[0].size()) {
            return false;
        }
        for (size_t i = 0; i < _mat.size(); ++i) {
            for (size_t j = 0; j < _mat[i].size(); ++j) {
                if (_mat[i][j] != other._mat[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * @brief 输出流重载
     */
    friend std::ostream &operator<<(std::ostream &out, const Matrix<T> &mat) {
        for (auto &row : mat._mat) {
            for (auto &elem : row) {
                out << elem << " ";
            }
            out << "\n";
        }
        return out << "\n";
    }
};

}  // namespace strassens_multiplication
}  // namespace divide_and_conquer

/**
 * @brief 单元自测用例（测试 512 x 512 的矩阵，比对施特拉森和朴素算法的用时与结果）
 */
static void test() {
    const size_t s = 512;
    auto matrix_demo =
        divide_and_conquer::strassens_multiplication::Matrix<size_t>(s, s);

    for (size_t i = 0; i < s; ++i) {
        for (size_t j = 0; j < s; ++j) {
            matrix_demo[i][j] = i + j;
        }
    }

    auto matrix_demo2 =
        divide_and_conquer::strassens_multiplication::Matrix<size_t>(s, s);
    for (size_t i = 0; i < s; ++i) {
        for (size_t j = 0; j < s; ++j) {
            matrix_demo2[i][j] = 2 + i + j;
        }
    }

    auto start = std::chrono::system_clock::now();
    auto Mat3 = matrix_demo2 * matrix_demo;
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> time = (end - start);
    std::cout << "Strassen time: " << time.count() << "s" << std::endl;

    start = std::chrono::system_clock::now();
    auto conf = matrix_demo2.naive_multiplication(matrix_demo);
    end = std::chrono::system_clock::now();

    time = end - start;
    std::cout << "Normal time: " << time.count() << "s" << std::endl;

    assert(Mat3 == conf); // 验证施特拉森矩阵计算值与常规乘法完全一致
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行自测
    return 0;
}
