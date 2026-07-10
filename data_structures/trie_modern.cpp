/**
 * @file
 *
 * @author Anmol3299
 * \brief A basic implementation of trie class to store only lower-case strings. (前缀树/字典树的现代 C++ 实现)
 */
#include <iostream>  // 用于输入输出操作
#include <memory>    // 用于智能指针 std::shared_ptr<>
#include <string>    // 用于 std::string 类

/**
 * 字典树 (Trie) 类，目前仅支持存储小写英文字母。
 * 可以通过将 ALPHABETS 修改为 128 来支持所有 ASCII 字符。
 */
class Trie {
 private:
    static constexpr size_t ALPHABETS = 26; // 小写英文字母的数量

    /**
     * Trie 树的节点结构体
     * 使用 C++11 的初始化列表效率更高，因此不需要显式的构造函数。
     */
    struct TrieNode {
        // 指针数组，大小为 26，对应 a-z。如果某字符存在，则指向其对应的子节点。
        std::shared_ptr<TrieNode> character[ALPHABETS]{nullptr};

        // 标记该节点是否是某个单词的结尾
        bool isEndOfWord{false};
    };

    /**
     * 辅助函数：检查给定节点是否拥有子节点（即是否是其他单词的前缀）
     * @param node 需要检查的节点
     * @return `true` 如果有至少一个子节点存在
     * @return `false` 如果没有任何子节点
     */
    inline static bool hasChildren(std::shared_ptr<TrieNode> node) {
        for (size_t i = 0; i < ALPHABETS; i++) {
            if (node->character[i]) {
                return true;
            }
        }
        return false;
    }

    /**
     * 递归删除单词的辅助函数。
     * 首先递归定位到要删除单词的最后一个字符所在的节点。
     * 如果单词不存在于 Trie 中，则抛出 runtime_error 异常。
     * 成功定位到最后一个字符节点后，将 isEndOfWord 置为 false。
     * 若该节点没有任何子节点，则可以安全释放该节点（返回 nullptr 使得父节点断开指向该节点的链接）；
     * 若该节点还有子节点，则必须保留当前节点以维持其他单词的完整性（返回当前节点）。
     * @param word 待删除的字符串
     * @param curr 当前递归遍历到的节点
     * @param index 当前遍历字符在字符串中的索引位置
     * @return 调整链接后，返回当前节点指针或 nullptr
     * @throw 若单词不在 Trie 中，抛出 std::runtime_error
     */
    std::shared_ptr<TrieNode> removeWordHelper(const std::string& word,
                                               std::shared_ptr<TrieNode> curr,
                                               size_t index) {
        // 递归终止条件：已经遍历到待删除单词的末尾
        if (word.size() == index) {
            if (curr->isEndOfWord) {
                curr->isEndOfWord = false; // 取消单词结尾标记
            }
            // 如果该节点没有其他子节点，则可以安全地将其从父节点断开
            if (hasChildren(curr)) {
                return curr;
            }
            return nullptr;
        }

        size_t idx = word[index] - 'a'; // 计算字符映射的数组索引

        // 如果路径中断，说明待删除单词在 Trie 中不存在
        if (!curr->character[idx]) {
            throw std::runtime_error(std::move(std::string("Word not found.")));
        }

        // 递归向下删除
        curr->character[idx] =
            removeWordHelper(word, curr->character[idx], index + 1);

        // 如果当前节点的分支仍然存在（被其他单词共享），或者该节点还有其他字符子分支，则保留当前节点
        if (curr->character[idx] || hasChildren(curr)) {
            return curr;
        }
        return nullptr;
    }

 public:
    /// 构造函数，初始化 Trie 树的根节点
    Trie() : m_root(std::make_shared<TrieNode>()) {}

    /**
     * 向 Trie 树中插入一个单词
     * @param word 待插入的字符串
     */
    void insert(const std::string& word) {
        auto curr = m_root;
        for (char ch : word) {
            size_t index = ch - 'a';

            // 如果该字符在当前层不存在，则创建对应的子节点
            if (!curr->character[index]) {
                curr->character[index] = std::make_shared<TrieNode>();
            }

            curr = curr->character[index]; // 移动指针到子节点
        }
        curr->isEndOfWord = true; // 循环结束，在单词的最后一个字符节点标记单词结尾
    }

    /**
     * 在 Trie 树中检索单词是否存在
     * @param word 待查找的字符串
     * @return `true` 单词存在且 isEndOfWord 为真
     * @return `false` 单词不存在，或者该单词只是其他更长单词的未标记前缀
     */
    bool search(const std::string& word) {
        auto curr = m_root;
        for (char ch : word) {
            size_t index = ch - 'a';

            // 只要有任一字符路径不存在，说明单词不存在于树中
            if (!curr->character[index]) {
                return false;
            }
            curr = curr->character[index];
        }
        return curr->isEndOfWord; // 必须检查是否是一个完整单词的结束标记
    }

    // 删除单词的外部接口方法
    void removeWord(const std::string& word) {
        m_root = removeWordHelper(word, m_root, 0);
    }

 private:
    // 智能指针，管理 Trie 树的根节点
    std::shared_ptr<TrieNode> m_root;
};

/**
 * 主函数，用于测试 Trie 树的插入、检索与删除功能
 */
int main() {
    Trie trie;
    trie.insert("hel");
    trie.insert("hello");
    trie.removeWord("hel"); // 删除单词 "hel"，但 "hello" 应当保留
    std::cout << trie.search("hello") << '\n'; // 输出应为 1 (true)

    return 0;
}
