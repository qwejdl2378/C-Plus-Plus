/**
 * @file
 * @brief [Trie (字典树) Data Structure](https://iq.opengenus.org/autocomplete-using-trie-data-structure/) with search variants (支持多种查询变体的字典树实现)
 *
 * @details
 * 字典树（Trie，又称前缀树）是一种哈希树的变体，用于统计、排序和保存大量的字符串。
 * 本实现支持以下核心操作：
 * 1. 插入单词 (Insert)
 * 2. 删除单词 (Delete & DeleteFrom)
 * 3. 检索单词是否存在 (SearchPresence)
 * 4. 前缀模糊匹配补全提示 (SearchSuggestions)
 * 5. 基于词频的热词前缀补全提示，返回 Top-3 最常搜索的补全项 (SearchFreqSuggestions)
 *
 * 仅支持小写/大写英文字母（不区分大小写，内部统一转换为小写）。
 *
 * 时间复杂度:
 * - 插入/删除/检索: $O(L)$，其中 $L$ 是单词的长度。
 * 空间复杂度: $O(N \cdot 26)$，其中 $N$ 是所有单词的字符节点总数。
 *
 * @note
 * 【内存泄漏与越界读写重大安全 Bug 审计与修复】：
 * 1. **`DeleteFrom` 递归删除逻辑失效与越界读写崩溃 Bug**：
 *    原实现中 `DeleteFrom` 用于递归释放被删单词的独占节点链。原判定条件为 `if (delete_string.size() == remove_index)`。
 *    - 如果初始调用的 `remove_index` 小于字符串长度，该判定为假，函数会直接退出，导致**所有相关字符节点被遗留在内存中，造成严重的内存泄漏**。
 *    - 如果条件凑巧满足，函数会执行 `delete_string[remove_index]`。由于 `remove_index == size()`，这超出了字符串的有效索引边界（读取到 `\0`），
 *      导致计算出 `letter_index = -97`，从而触发 **`vector` 数组越界访问崩溃**。
 *    **修复**：重构 `DeleteFrom` 逻辑，在 `remove_index < delete_string.size()` 时才进行下一层递归，并在返回时安全执行 `delete delete_from` 逐级释放，彻底修复内存泄漏与数组越界。
 * 2. **自测函数内存泄漏**：
 *    原测试代码 `test()` 使用 `new Tnode()` 在堆上分配根节点，但结束时从未释放。
 *    **修复**：使用栈分配的 `Tnode root` 对象，利用 C++ 的析构函数 `~Tnode()` 自动、安全地释放整棵树占用的所有动态内存。
 *
 * @author [Ghanashyam](https://github.com/g-s-k-zoro)
 */

#include <algorithm>  
#include <cassert>    
#include <cctype>     
#include <cstdint>    
#include <cstring>    
#include <iostream>   
#include <queue>      
#include <vector>

namespace operations_on_datastructures {
namespace trie_operations {

/**
 * @class Tnode
 * @brief 字典树节点类
 */
class Tnode {
 private:
    static constexpr uint8_t ENGLISH_ALPHABET_SIZE = 26;
    std::vector<Tnode *> english;  ///< 指向子节点的指针数组
    bool endOfWord;               ///< 标记是否为单词的结尾
    uint32_t frequency;           ///< 该单词被成功搜索的频次

 public:
    Tnode() {
        english.resize(ENGLISH_ALPHABET_SIZE, nullptr);
        endOfWord = false;
        frequency = 0;
    }

    // 拷贝构造函数
    Tnode(const Tnode &node) {
        english = node.english;
        endOfWord = node.endOfWord;
        frequency = node.frequency;
    }

    Tnode &operator=(const Tnode &node) = default;
    Tnode(Tnode &&) = default;
    Tnode &operator=(Tnode &&) = default;

    /**
     * @brief 统计当前节点拥有的子节点数量
     */
    inline uint8_t numberOfChildren(Tnode *node) const {
        assert(node != nullptr && "Node pointer cannot be null!");
        return ENGLISH_ALPHABET_SIZE -
               std::count(node->english.begin(), node->english.end(), nullptr);
    }

    // 成员函数声明
    void Insert(const std::string &entry);
    void Delete(const std::string &entry);
    void DeleteFrom(Tnode *delete_from, const std::string &delete_string, size_t remove_index);
    bool SearchPresence(const std::string &key);
    void SuggestAutocomplete(Tnode *new_root, const std::string &prefix) const;
    void SearchSuggestions(const std::string &key);
    void SuggestFreqAutocomplete(
        Tnode *new_root, const std::string &prefix,
        std::priority_queue<std::pair<int, std::string> > *suggestions) const;
    void SearchFreqSuggestions(const std::string &key);
    void SelectionTop_3(
        std::priority_queue<std::pair<int, std::string> > *suggestions) const;

    /**
     * @brief 析构函数，递归释放子节点持有的内存空间
     */
    ~Tnode() {
        for (int i = 0; i < ENGLISH_ALPHABET_SIZE; i++) {
            if (english[i] != nullptr) {
                delete english[i];
            }
        }
    }
};

/**
 * @brief 向字典树中插入一个单词
 */
void Tnode::Insert(const std::string &entry) {
    Tnode *cur_pos = this;
    for (char i : entry) {
        int letter_index = std::tolower(static_cast<unsigned char>(i)) - 97;
        if (letter_index < 0 || letter_index >= ENGLISH_ALPHABET_SIZE) {
            continue; // 过滤非英文字符
        }
        if (cur_pos->english[letter_index] == nullptr) {
            cur_pos->english[letter_index] = new Tnode();
        }
        cur_pos = cur_pos->english[letter_index];
    }
    cur_pos->endOfWord = true;
}

/**
 * @brief 核心修复：递归释放冗余后缀单词节点路径，杜绝内存泄漏与数组越界
 */
void Tnode::DeleteFrom(Tnode *delete_from, const std::string &delete_string, size_t remove_index) {
    if (delete_from == nullptr) {
        return;
    }
    // 递归删除下一层节点
    if (remove_index < delete_string.size()) {
        int letter_index = std::tolower(static_cast<unsigned char>(delete_string[remove_index])) - 97;
        if (letter_index >= 0 && letter_index < ENGLISH_ALPHABET_SIZE) {
            DeleteFrom(delete_from->english[letter_index], delete_string, remove_index + 1);
            // 核心修复：子节点被释放后，将父节点的指针置为空，防止在析构函数中触发 Double Free
            delete_from->english[letter_index] = nullptr;
        }
    }
    delete delete_from;
}

/**
 * @brief 从字典树中安全地删除一个单词
 */
void Tnode::Delete(const std::string &entry) {
    Tnode *cur_pos = this;
    Tnode *delete_from = this;  
    int delete_from_index = -1; 
    size_t n = entry.size();

    for (size_t i = 0; i < n; i++) {
        int letter_index = std::tolower(static_cast<unsigned char>(entry[i])) - 97;
        if (letter_index < 0 || letter_index >= ENGLISH_ALPHABET_SIZE || cur_pos->english[letter_index] == nullptr) {
            std::cout << "Entry not Found" << std::endl;
            return;
        }

        // 如果节点有其他分叉或者本身也是另一个单词的结尾，则不能彻底删除其祖先链路
        if (numberOfChildren(cur_pos) > 1 || cur_pos->endOfWord) {
            delete_from = cur_pos;      
            delete_from_index = static_cast<int>(i) - 1;  
        }
        cur_pos = cur_pos->english[letter_index];
    }

    if (!cur_pos->endOfWord) {
        std::cout << "Entry not Found" << std::endl;
        return;
    }

    // 情况 1：如果待删词节点还有子树节点，只需撤销其单词结尾标记与频次即可
    if (numberOfChildren(cur_pos) > 0) {
        cur_pos->endOfWord = false;
        cur_pos->frequency = 0;
        return;
    }

    // 情况 2：需要切断分支并释放无用子节点
    int letter_index = std::tolower(static_cast<unsigned char>(entry[delete_from_index + 1])) - 97;
    cur_pos = delete_from->english[letter_index];
    delete_from->english[letter_index] = nullptr;

    // 递归安全销毁多余节点
    if (n > static_cast<size_t>(delete_from_index + 2)) {
        DeleteFrom(cur_pos, entry, delete_from_index + 2);
    } else {
        delete cur_pos;
    }
}

/**
 * @brief 检索一个单词是否存在于字典树中，若存在则使其搜索频次自增
 */
bool Tnode::SearchPresence(const std::string &key) {
    Tnode *cur_pos = this;
    for (char i : key) {
        int letter_index = std::tolower(static_cast<unsigned char>(i)) - 97;
        if (letter_index < 0 || letter_index >= ENGLISH_ALPHABET_SIZE || cur_pos->english[letter_index] == nullptr) {
            return false;
        }
        cur_pos = cur_pos->english[letter_index];
    }
    if (cur_pos->endOfWord) {
        (cur_pos->frequency)++;
        return true;
    }
    return false;
}

/**
 * @brief 递归获取当前子树下匹配给定前缀的所有自动补全提示项
 */
void Tnode::SuggestAutocomplete(Tnode *new_root, const std::string &prefix) const {
    for (int i = 0; i < ENGLISH_ALPHABET_SIZE; i++) {
        if (new_root->english[i] != nullptr) {
            if (new_root->english[i]->endOfWord) {
                std::cout << prefix + static_cast<char>(i + 97) << std::endl;
            }
            SuggestAutocomplete(new_root->english[i], prefix + static_cast<char>(i + 97));
        }
    }
}

/**
 * @brief 检索并显示以给定 key 为最长公共前缀的所有补全提示
 */
void Tnode::SearchSuggestions(const std::string &key) {
    Tnode *cur_pos = this;
    Tnode *prev_pos = this;  
    std::string prefix = "";  

    for (char i : key) {
        int letter_index = std::tolower(static_cast<unsigned char>(i)) - 97;
        prev_pos = cur_pos;  

        if (letter_index < 0 || letter_index >= ENGLISH_ALPHABET_SIZE || cur_pos->english[letter_index] == nullptr) {
            SuggestAutocomplete(prev_pos, prefix);
            std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
            return;
        }
        prefix += static_cast<char>(std::tolower(static_cast<unsigned char>(i)));
        cur_pos = cur_pos->english[letter_index];
    }
    if (cur_pos->endOfWord) {
        std::cout << key << std::endl;
        (cur_pos->frequency)++;
    }

    (void)prev_pos;  
    SuggestAutocomplete(cur_pos, prefix);
    std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
}

/**
 * @brief 输出堆中频次最高的前 3 个自动补全候选词
 */
void Tnode::SelectionTop_3(
    std::priority_queue<std::pair<int, std::string> > *suggestions) const {
    int n = suggestions->size();
    int Top = n < 3 ? n : 3;
    while (Top--) {
        std::cout << suggestions->top().second << " (Freq: " << suggestions->top().first << ")" << std::endl;
        suggestions->pop();
    }
}

/**
 * @brief 递归搜集前缀匹配的所有项，并以 (频次, 单词) 存入最大堆中
 */
void Tnode::SuggestFreqAutocomplete(
    Tnode *new_root, const std::string &prefix,
    std::priority_queue<std::pair<int, std::string> > *suggestions) const {
    for (int i = 0; i < ENGLISH_ALPHABET_SIZE; i++) {
        if (new_root->english[i] != nullptr) {
            if (new_root->english[i]->endOfWord) {
                suggestions->push(std::make_pair(
                    new_root->english[i]->frequency, prefix + static_cast<char>(i + 97)));
            }
            SuggestFreqAutocomplete(new_root->english[i], prefix + static_cast<char>(i + 97), suggestions);
        }
    }
}

/**
 * @brief 搜集并输出最长前缀匹配且检索频次最高的前三个热词候选
 */
void Tnode::SearchFreqSuggestions(const std::string &key) {
    Tnode *cur_pos = this;
    Tnode *prev_pos = this;  
    std::string prefix = "";  
    std::priority_queue<std::pair<int, std::string> > suggestions;  
    std::priority_queue<std::pair<int, std::string> > *Suggestions = &suggestions;

    for (char i : key) {
        int letter_index = std::tolower(static_cast<unsigned char>(i)) - 97;
        prev_pos = cur_pos;  

        if (letter_index < 0 || letter_index >= ENGLISH_ALPHABET_SIZE || cur_pos->english[letter_index] == nullptr) {
            SuggestFreqAutocomplete(prev_pos, prefix, Suggestions);
            SelectionTop_3(Suggestions);
            std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
            return;
        }
        prefix += static_cast<char>(std::tolower(static_cast<unsigned char>(i)));
        cur_pos = cur_pos->english[letter_index];
    }
    if (cur_pos->endOfWord) {
        (cur_pos->frequency)++;
        suggestions.push(std::make_pair(cur_pos->frequency, key));
    }

    (void)prev_pos;  
    SuggestFreqAutocomplete(cur_pos, prefix, Suggestions);
    SelectionTop_3(Suggestions);

    std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - " << std::endl;
}

}  // namespace trie_operations
}  // namespace operations_on_datastructures

/**
 * @brief 单元自测用例
 */
static void test() {
    // 核心修复：改用栈上分配的 Tnode 根节点，自动触发析构释放资源，消除内存泄漏
    operations_on_datastructures::trie_operations::Tnode root;
    std::vector<std::string> inputs = {
        "abcde", "sss",    "ssss",  "ssst", "sssu", "sssv",
        "sst",   "ssts",   "sstt",  "sstu", "tutu", "tutuv",
        "tutuu", "tutuvs", "tutus", "tvst", "tvsu", "vvvv"};

    for (auto &i : inputs) {
        root.Insert(i);
    }
    
    // 验证单词是否存在
    assert(root.SearchPresence("vvvv"));
    std::cout << "vvvv presence: " << root.SearchPresence("vvvv") << std::endl;
    
    // 删除单词
    root.Delete("vvvv");
    
    // 再次验证其不存在
    assert(!root.SearchPresence("vvvv"));
    std::cout << "vvvv presence after deletion: " << root.SearchPresence("vvvv") << std::endl;

    std::cout << "tutu presence: " << root.SearchPresence("tutu") << std::endl;
    root.SearchSuggestions("tutu");
    std::cout << "tutu presence after query: " << root.SearchPresence("tutu") << std::endl;

    root.SearchSuggestions("tutuv");
    std::cout << "tutuv presence after query: " << root.SearchPresence("tutuv") << std::endl;

    root.SearchSuggestions("tutuvs");

    std::cout << "Search frequency suggestions for prefix 'tu':" << std::endl;
    root.SearchFreqSuggestions("tu");  

    std::cout << "All suggestions for empty search:" << std::endl;
    root.SearchSuggestions("");  
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
