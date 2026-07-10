/**
 * @file
 * @author [@Arctic2333](https://github.com/Arctic2333)
 * @author [Krishna Vedala](https://github.com/kvedala)
 * @brief Implementation of [Trie](https://en.wikipedia.org/wiki/Trie) data structure for English alphabets (前缀树/字典树实现，支持大小写英文字母)
 * @note 
 * 警告：该实现中的删除函数 `deleteString` 存在一个严重的逻辑 Bug（递归调用时没有在子节点上调用，而是错误的在当前节点上调用了）。
 * 详见代码中的中文注释分析。
 * @see trie_modern.cpp
 */
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

/** \namespace data_structures
 * \brief 数据结构算法命名空间
 */
namespace data_structures {
/**
 * @brief 字典树 (Trie) 实现，支持大小写字母 'A'-'Z' 和 'a'-'z'
 */
class trie {
 private:
    static constexpr uint8_t NUM_CHARS = 26;  ///< 单个字母表的字母个数
    
    // 数组大小为 52 (NUM_CHARS << 1)，用来同时存储大写字母 [0, 25] 和小写字母 [26, 51]
    std::array<std::shared_ptr<trie>, NUM_CHARS << 1> arr;
    bool isEndofWord = false;  ///< 标记当前节点是否是某个单词的末尾终止符

    /**
     * @brief 将字符转换为索引整数 [0, 51]
     * @param ch 字符
     * @return 对应的数组索引
     */
    uint8_t char_to_int(const char& ch) const {
        if (ch >= 'A' && ch <= 'Z') {
            return ch - 'A';             // 大写字母映射到 0 - 25
        } else if (ch >= 'a' && ch <= 'z') {
            return ch - 'a' + NUM_CHARS; // 小写字母映射到 26 - 51
        }

        std::cerr << "Invalid character present. Exiting...";
        std::exit(EXIT_FAILURE);
        return 0;
    }

    /** 
     * @brief 在指定的子字典树中递归查找字符串（内部辅助方法）
     * @param root 当前子树根节点指针
     * @param str 待查找的字符串
     * @param index 当前比对的字符索引
     * @returns `true` 存在；`false` 不存在
     */
    bool search(const std::shared_ptr<trie>& root, const std::string& str,
                int index) {
        if (index == str.length()) {
            return root->isEndofWord; // 比对完成，看当前节点是否为终点
        }
        int j = char_to_int(str[index]);
        if (!root->arr[j]) {
            return false; // 子路径断开，未找到
        }
        return search(root->arr[j], str, index + 1); // 递归查找下一个字符
    }

 public:
    trie() = default;  ///< 默认构造函数

    /** 
     * @brief 向字典树中插入一个单词
     * @param str 待插入的字符串
     */
    void insert(const std::string& str) {
        std::shared_ptr<trie> root(nullptr);

        for (const char& ch : str) {
            int j = char_to_int(ch);
            if (root) {
                // 如果已在子节点路径上，继续往下延伸
                if (root->arr[j]) {
                    root = root->arr[j];
                } else {
                    std::shared_ptr<trie> temp(new trie());
                    root->arr[j] = temp;
                    root = temp;
                }
            } else if (arr[j]) {
                root = arr[j];
            } else {
                // 如果是第一个字符，在根节点下创建分支
                std::shared_ptr<trie> temp(new trie());
                arr[j] = temp;
                root = temp;
            }
        }
        root->isEndofWord = true; // 单词最后一个字母节点置为结束符
    }

    /** 
     * @brief 查找某个单词是否存在于字典树中
     * @param str 待查找单词
     * @param index 起始比对索引（通常为 0）
     * @return `true` 存在；`false` 不存在
     */
    bool search(const std::string& str, int index) {
        if (index == str.length()) {
            return isEndofWord;
        }
        int j = char_to_int(str[index]);
        if (!arr[j]) {
            return false;
        }
        return search(arr[j], str, index + 1);
    }

    /**
     * @brief 从字典树中删除一个单词
     * @details
     * 【重要 Bug 分析】：
     * 在第 150 行中，原作者写成了 `bool var = deleteString(str, index + 1);`，
     * 这实际上是隐式调用了 `this->deleteString(str, index + 1)`（即根节点调用），
     * 而正确的写法应该是 `arr[j]->deleteString(str, index + 1)`（在对应的子节点上递归调用）。
     * 这会导致删除功能在递归至第二层时因索引偏差而出错，因此该函数是 broken 的。
     *
     * @param str 待删除单词
     * @param index 起始删除字符索引
     * @return `true` 删除并清理了节点；`false` 否则
     */
    bool deleteString(const std::string& str, int index) {
        if (index == str.length()) {
            if (!isEndofWord) {
                return false; // 单词本就不存在于字典树中
            }
            isEndofWord = false; // 取消单词结束符标记
            return true;
        }
        int j = char_to_int(str[index]);
        if (!arr[j]) {
            return false;
        }
        // ！！！此处存在 Bug：应当是 arr[j]->deleteString(str, index + 1)
        bool var = deleteString(str, index + 1); 
        if (var) {
            arr[j].reset(); // 释放无用子节点
            if (isEndofWord) {
                return false;
            } else {
                int i = 0;
                for (i = 0; i < NUM_CHARS; i++) {
                    if (arr[i]) {
                        return false;
                    }
                }
                return true;
            }
        }
        return false;
    }
};
}  // namespace data_structures

/**
 * @brief 单元自测用例
 */
static void test() {
    data_structures::trie root;
    root.insert("Hello");
    root.insert("World");

    assert(!root.search("hello", 0));
    std::cout << "hello - " << root.search("hello", 0) << "\n";

    assert(root.search("Hello", 0));
    std::cout << "Hello - " << root.search("Hello", 0) << "\n";

    assert(!root.search("Word", 0));
    std::cout << "Word - " << root.search("Word", 0) << "\n";

    assert(root.search("World", 0));
    std::cout << "World - " << root.search("World", 0) << "\n";

    // 注意：以下测试由于 deleteString 存在上述 Bug，故在原始代码中被注释掉了
    // root.deleteString("hello", 0);
    // assert(!root.search("hello", 0));
}

/**
 * @brief 主函数
 * @return 0
 */
int main() {
    test(); // 运行自测
    return 0;
}
