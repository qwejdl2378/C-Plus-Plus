/**
 * @file
 * @brief [Bloom Filter](https://en.wikipedia.org/wiki/Bloom_filter) generic implementation in C++ (布隆过滤器泛型算法实现)
 * @details 布隆过滤器是一种空间效率极高的概率型数据结构。
 * 查询操作会返回两种可能的结果：
 * 1) "可能在集合中" (Possibly in set)。
 * 2) "绝对不在集合中" (Definitely not in set)。
 *
 * 它的核心优势是能够避免执行“高开销操作”（例如磁盘 I/O 或数据库复杂查询）。如果布隆过滤器判断某个元素不存在，
 * 我们就可以 100% 确信它不存在，从而直接跳过昂贵的后续操作。
 * 基础布隆过滤器不支持删除元素。
 *
 * @author [DanArmor](https://github.com/DanArmor)
 */

#include <cassert>           /// 用于 assert 断言
#include <functional>        /// 用于 std::function 存储哈希函数
#include <initializer_list>  /// 用于 std::initializer_list 构造列表
#include <string>            /// 用于字符串测试
#include <vector>            /// 用于 std::vector 容器
#include <iostream>          /// 用于输入输出

/**
 * @namespace data_structures
 * @brief 数据结构算法命名空间
 */
namespace data_structures {
/**
 * @brief 专为布隆过滤器定制的简易位图实现 (Bitset)
 */
class Bitset {
 private:
    std::vector<std::size_t> data;  ///< 存储位图数据的物理数组（每块由 std::size_t 构成）
    
    // 注意：此处原代码设计中 blockSize = sizeof(std::size_t) 得到的是字节数（如 8 字节，即 8 位），
    // 导致每个块实际上只被利用了前 8 个 bit 位，其余 56 个 bit 被空置了。这是一种空间浪费的设计，
    // 读者在学习和实际工程应用中，一般应将其设为：sizeof(std::size_t) * 8（即 64 位）。
    static const std::size_t blockSize = sizeof(std::size_t);  

 public:
    explicit Bitset(std::size_t);
    std::size_t size();
    void add(std::size_t);
    bool contains(std::size_t);
};

/**
 * @brief 获取位图的内部物理数据块个数
 */
std::size_t Bitset::size() { return data.size(); }

/**
 * @brief 构造函数
 * @param initSize 初始分配的物理数据块（Block）个数
 */
Bitset::Bitset(std::size_t initSize) : data(initSize) {}

/**
 * @brief 将指定位置 x 处的 bit 位设为 1
 * @param x bit 位的偏移位置
 */
void Bitset::add(std::size_t x) {
    std::size_t blockIndex = x / blockSize;
    if (blockIndex >= data.size()) {
        data.resize(blockIndex + 1); // 自动扩容位图
    }
    data[blockIndex] |= 1 << (x % blockSize); // 将目标位设为 1
}

/**
 * @brief 检查指定位置 x 处的 bit 位是否为 1
 * @param x bit 位的偏移位置
 * @return `true` 代表该位已被置为 1；`false` 否则
 */
bool Bitset::contains(std::size_t x) {
    std::size_t blockIndex = x / blockSize;
    if (blockIndex >= data.size()) {
        return false;
    }
    return data[blockIndex] & (1 << (x % blockSize));
}

/**
 * @brief 布隆过滤器模板类
 * @tparam T 待过滤和排重的数据类型
 */
template <typename T>
class BloomFilter {
 private:
    Bitset set;  ///< 内部维护的位图对象
    std::vector<std::function<std::size_t(T)>> hashFunks;  ///< 注册的哈希函数列表

 public:
    /**
     * @brief 构造函数
     * @param size 位图的初始块大小
     * @param funks 初始化哈希函数列表
     */
    BloomFilter(std::size_t size,
                std::initializer_list<std::function<std::size_t(T)>> funks);
    
    void add(T);       ///< 向过滤器中添加元素
    bool contains(T);  ///< 查询元素是否可能存在于过滤器中
};

template <typename T>
BloomFilter<T>::BloomFilter(
    std::size_t size,
    std::initializer_list<std::function<std::size_t(T)>> funks)
    : set(size), hashFunks(funks) {}

/**
 * @brief 添加元素 x 到过滤器中
 * @details 依次对 x 调用全部的哈希函数，根据算出来的每个哈希值，将其映射到位图对应的一批位置并置为 1。
 * @param x 待添加的元素值
 */
template <typename T>
void BloomFilter<T>::add(T x) {
    for (std::size_t i = 0; i < hashFunks.size(); i++) {
        // 计算映射位置并添加到位图
        set.add(hashFunks[i](x) % (sizeof(std::size_t) * set.size()));
    }
}

/**
 * @brief 检查元素 x 是否存在
 * @details 只要有任何一个哈希映射位在位图中为 0，就 100% 确定元素不存在 (返回 false)。
 * 只有所有哈希位都为 1 时，才返回 true (代表可能存在，会有极小的假阳性误判概率)。
 * @param x 待查询的元素值
 * @return `true` 可能存在；`false` 绝对不存在
 */
template <typename T>
bool BloomFilter<T>::contains(T x) {
    for (std::size_t i = 0; i < hashFunks.size(); i++) {
        if (set.contains(hashFunks[i](x) %
                         (sizeof(std::size_t) * set.size())) == false) {
            return false; // 任意一位为 0 即代表肯定不存在
        }
    }
    return true;
}

/**
 * @brief 经典的 DJB2 哈希函数实现（用于字符串）
 * @param s 字符串
 * @returns 哈希值
 */
static std::size_t hashDJB2(std::string const& s) {
    std::size_t hash = 5381;
    for (char c : s) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

/**
 * @brief 辅助哈希函数（用于字符串）
 */
static std::size_t hashStr(std::string const& s) {
    std::size_t hash = 37;
    std::size_t primeNum1 = 54059;
    std::size_t primeNum2 = 76963;
    for (char c : s) {
        hash = (hash * primeNum1) ^ (c * primeNum2);
    }
    return hash;
}

/**
 * @brief 整数哈希函数一
 */
std::size_t hashInt_1(int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

/**
 * @brief 整数哈希函数二
 */
std::size_t hashInt_2(int x) {
    auto y = static_cast<std::size_t>(x);
    y = (y ^ (y >> 30)) * static_cast<std::size_t>(0xbf58476d1ce4e5b9);
    y = (y ^ (y >> 27)) * static_cast<std::size_t>(0x94d049bb133111eb);
    y = y ^ (y >> 31);
    return y;
}
}  // namespace data_structures

/**
 * @brief 字符串类型泛型测试
 */
static void test_bloom_filter_string() {
    data_structures::BloomFilter<std::string> filter(
        10, {data_structures::hashDJB2, data_structures::hashStr});
    std::vector<std::string> toCheck{"hello", "world", "!"};
    std::vector<std::string> toFalse{"false", "world2", "!!!"};
    for (const auto& x : toCheck) {
        filter.add(x);
    }
    for (const auto& x : toFalse) {
        assert(filter.contains(x) == false);
    }
    for (const auto& x : toCheck) {
        assert(filter.contains(x));
    }
}

/**
 * @brief 整数类型泛型测试
 */
static void test_bloom_filter_int() {
    data_structures::BloomFilter<int> filter(
        20, {data_structures::hashInt_1, data_structures::hashInt_2});
    std::vector<int> toCheck{100, 200, 300, 50};
    std::vector<int> toFalse{1, 2, 3, 4, 5, 6, 7, 8};
    for (int x : toCheck) {
        filter.add(x);
    }
    for (int x : toFalse) {
        assert(filter.contains(x) == false);
    }
    for (int x : toCheck) {
        assert(filter.contains(x));
    }
}

/**
 * @brief 位图独立测试（布隆过滤器依赖位图的准确性）
 */
static void test_bitset() {
    data_structures::Bitset set(2);
    std::vector<std::size_t> toCheck{0, 1, 5, 8, 63, 64, 67, 127};
    for (auto x : toCheck) {
        set.add(x);
        assert(set.contains(x));
    }
    assert(set.contains(128) == false);
    assert(set.contains(256) == false);
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test_bitset();
    test_bloom_filter_string();
    test_bloom_filter_int();
    
    std::cout << "All tests have successfully passed!\n";
    return 0;
}
