/**
 * @file
 * @author [Venkata Bharath](https://github.com/bharath000)
 * @brief Implementation of [Trie](https://en.wikipedia.org/wiki/Trie) data structure using HashMap (使用哈希表实现的字典树/前缀树)
 * @details 采用哈希表（unordered_map）存储子节点指针，不仅能显著优化稀疏字符集下的空间开销，
 * 还天然支持任意 Unicode 字符集（例如中文等，如测试用例中的“苹果”）。
 * 此外，该实现还集成了一个单词智能推荐算法 `predict_words`，可以基于给定的前缀自动预测和输出所有完整的匹配单词。
 * @see trie_modern.cpp
 */
#include <cassert>        /// 用于 assert 断言
#include <iostream>       /// 用于输入输出
#include <memory>         /// 用于 std::shared_ptr 智能指针管理
#include <stack>          /// 用于 std::stack 辅助栈
#include <unordered_map>  /// 用于 std::unordered_map 映射容器
#include <vector>         /// 用于 std::vector

/**
 * @namespace data_structures
 * @brief 数据结构算法命名空间
 */
namespace data_structures {

/**
 * @namespace trie_using_hashmap
 * @brief 哈希字典树算法命名空间
 */
namespace trie_using_hashmap {

/**
 * @brief 基于 HashMap 的字典树实现类
 */
class Trie {
 private:
    /**
     * @brief 字典树的节点结构体
     */
    struct Node {
        // 使用 unordered_map 建立字符（char16_t 宽字符）到对应子节点的映射关系
        std::unordered_map<char16_t, std::shared_ptr<Node>> children;
        bool word_end = false;  ///< 标识当前节点是否是某个单词的末尾
    };

    std::shared_ptr<Node> root_node =
        std::make_shared<Node>();  ///< 字典树的根哨兵节点

 public:
    Trie() = default;

    /**
     * @brief 向字典树中插入一个单词
     * @param word 待插入的字符串
     */
    void insert(const std::string& word) {
        std::shared_ptr<Node> curr = root_node;
        for (char ch : word) {
            // 若当前字符分支不存在，创建之
            if (curr->children.find(ch) == curr->children.end()) {
                curr->children[ch] = std::make_shared<Node>();
            }
            curr = curr->children[ch];
        }

        // 单词尾节点打上终止标记
        if (!curr->word_end && curr != root_node) {
            curr->word_end = true;
        }
    }

    /**
     * @brief 查找某个单词是否在字典树中
     * @param word 目标查询单词
     * @returns `true` 存在；`false` 不存在
     */
    bool search(const std::string& word) {
        std::shared_ptr<Node> curr = root_node;
        for (char ch : word) {
            if (curr->children.find(ch) == curr->children.end()) {
                return false;
            }
            curr = curr->children[ch];
            if (!curr) {
                return false;
            }
        }
        return curr->word_end; // 字符匹配完，看是否为独立单词的末尾
    }

    /**
     * @brief 检查树中是否存在某个前缀开头的单词
     * @param prefix 目标前缀
     * @returns `true` 存在该前缀；`false` 否则
     */
    bool startwith(const std::string& prefix) {
        std::shared_ptr<Node> curr = root_node;
        for (char ch : prefix) {
            if (curr->children.find(ch) == curr->children.end()) {
                return false;
            }
            curr = curr->children[ch];
        }
        return true;
    }

    /**
     * @brief 从字典树中删除指定单词
     * @details
     * 使用一个辅助栈 nodes 记录向下查找时沿途的所有节点。
     * 1. 取消该单词的 word_end 标识。
     * 2. 自底向上判断：如果一个节点既不代表任何单词的尾部（word_end 为 false），并且其 children 映射已空，
     *    则说明它可以被安全地从其父节点的映射表 children 中移除，依次出栈清理，避免内存泄漏。
     * @param word 待删除的单词
     */
    void delete_word(std::string word) {
        std::shared_ptr<Node> curr = root_node;
        std::stack<std::shared_ptr<Node>> nodes; // 记录查找路径
        int cnt = 0;
        for (char ch : word) {
            if (curr->children.find(ch) == curr->children.end()) {
                return; // 未找到，直接退出
            }
            if (curr->word_end) {
                cnt++;
            }

            nodes.push(curr->children[ch]);
            curr = curr->children[ch];
        }
        
        // 撤销尾节点标记
        if (nodes.top()->word_end) {
            nodes.top()->word_end = false;
        }
        
        // 向上回溯删除无用孤立的分支节点
        while (!(nodes.top()->word_end) && nodes.top()->children.empty()) {
            nodes.pop();
            if (nodes.empty()) {
                root_node->children.erase(word.back());
                break;
            }
            nodes.top()->children.erase(word.back());
            word.pop_back();
        }
    }

    /**
     * @brief 深度优先搜索（DFS）辅助函数：获取以当前节点为前缀的所有合法单词列表
     * @param results 结果存储容器
     * @param element 当前递归子树的节点
     * @param prefix 当前前缀字符串
     * @returns 递归拼凑好的所有匹配单词 vector
     */
    std::vector<std::string> get_all_words(std::vector<std::string> results,
                                           const std::shared_ptr<Node>& element,
                                           std::string prefix) {
        if (element->word_end) {
            results.push_back(prefix); // 发现完整单词，存入结果集
        }
        if (element->children.empty()) {
            return results;
        }
        // 遍历所有可能的字符路径，递归向下收集单词
        for (auto const& x : element->children) {
            std::string key = "";
            key = x.first;
            prefix += key;

            results =
                get_all_words(results, element->children[x.first], prefix);

            prefix.pop_back(); // 回溯恢复前缀状态
        }

        return results;
    }

    /**
     * @brief 根据给定前缀，智能预测并推荐所有的匹配单词
     * @param prefix 目标前缀
     * @return 所有可能匹配单词的 std::vector 数组
     */
    std::vector<std::string> predict_words(const std::string& prefix) {
        std::vector<std::string> result;
        std::shared_ptr<Node> curr = root_node;
        
        // 1. 沿前缀路径定位到前缀最后一个字符所在的字典树节点
        for (char ch : prefix) {
            if (curr->children.find(ch) == curr->children.end()) {
                return result; // 不存在该前缀，返回空
            }
            curr = curr->children[ch];
        }

        // 2. 特判：如果该前缀节点本身就是单词尾部，且没有子分支，直接返回
        if (curr->word_end && curr->children.empty()) {
            result.push_back(prefix);
            return result;
        }

        // 3. 收集该节点以下所有的合法单词
        result = get_all_words(result, curr, prefix);
        return result;
    }
};
}  // namespace trie_using_hashmap
}  // namespace data_structures

/**
 * @brief 单元自测用例集
 */
static void test() {
    data_structures::trie_using_hashmap::Trie obj;
    obj.insert("app");
    obj.insert("abscond");
    obj.insert("about");
    obj.insert("apps");
    obj.insert("apen");
    obj.insert("apples");
    obj.insert("apple");
    obj.insert("approach");
    obj.insert("bus");
    obj.insert("buses");
    obj.insert("Apple");
    obj.insert("Bounce");

    assert(!obj.search("appy"));
    std::cout << "appy is not a word in trie" << std::endl;

    assert(!obj.search("car"));
    std::cout << "car is not a word in trie" << std::endl;
    assert(obj.search("app"));
    assert(obj.search("apple"));
    assert(obj.search("apples"));
    assert(obj.search("apps"));
    assert(obj.search("apen"));
    assert(obj.search("approach"));
    assert(obj.search("about"));
    assert(obj.search("abscond"));
    assert(obj.search("bus"));
    assert(obj.search("buses"));
    assert(obj.search("Bounce"));
    assert(obj.search("Apple"));

    std::cout << "All the Inserted words are present in the trie" << std::endl;

    // 前缀匹配检测
    assert(!obj.startwith("approachs"));
    assert(obj.startwith("approach"));
    assert(obj.startwith("about"));
    assert(!obj.startwith("appy"));
    assert(obj.startwith("abscond"));
    assert(obj.startwith("bus"));
    assert(obj.startwith("buses"));
    assert(obj.startwith("Bounce"));
    assert(obj.startwith("Apple"));
    assert(obj.startwith("abs"));
    assert(obj.startwith("b"));
    assert(obj.startwith("bus"));
    assert(obj.startwith("Bo"));
    assert(obj.startwith("A"));
    assert(!obj.startwith("Ca"));
    assert(!obj.startwith("C"));

    std::cout << "All the tests passed for startwith method" << std::endl;

    // 智能预测单词检测
    std::vector<std::string> pred_words = obj.predict_words("a");
    for (const std::string& str : obj.predict_words("a")) {
        std::cout << str << std::endl;
    }
    assert(pred_words.size() == 8);
    std::cout << "Returned all words that start with prefix a " << std::endl;
    
    pred_words = obj.predict_words("app");
    for (const std::string& str : pred_words) {
        std::cout << str << std::endl;
    }
    assert(pred_words.size() == 5);
    std::cout << "Returned all words that start with prefix app " << std::endl;
    
    pred_words = obj.predict_words("A");
    for (const std::string& str : pred_words) {
        std::cout << str << std::endl;
    }
    assert(pred_words.size() == 1);
    std::cout << "Returned all words that start with prefix A " << std::endl;
    
    pred_words = obj.predict_words("bu");
    for (const std::string& str : pred_words) {
        std::cout << str << std::endl;
    }
    assert(pred_words.size() == 2);
    std::cout << "Returned all words that start with prefix bu " << std::endl;

    // 删除单词测试
    obj.delete_word("app");
    assert(!obj.search("app"));
    std::cout << "word app is deleted sucessful" << std::endl;

    pred_words = obj.predict_words("app");
    for (const std::string& str : pred_words) {
        std::cout << str << std::endl;
    }
    assert(pred_words.size() == 4);
    std::cout << "app is deleted sucessful" << std::endl;

    // 中文支持测试
    obj.insert("苹果");
    assert(obj.startwith("苹"));
    pred_words = obj.predict_words("h");
    assert(pred_words.size() == 0);
    std::cout << "No word starts with prefix h in trie" << std::endl;

    std::cout << "All tests passed" << std::endl;
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行单元自测
    return 0;
}
