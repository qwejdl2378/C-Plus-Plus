/**
 * @file
 * @brief Implementation of [Huffman Coding](https://en.wikipedia.org/wiki/Huffman_coding) algorithm (哈夫曼编码树算法实现)
 *
 * @details
 * 哈夫曼编码（Huffman Coding）是一种著名的贪心压缩编码算法，用于无损数据压缩。
 *
 * ### 贪心策略 (Greedy Choice)
 * 1. 每次选择当前频次（权重）最低的两个节点，将其合并为一个新节点。
 * 2. 新节点的频次为两个子节点频次之和。
 * 3. 这种局部最优的不断合并，最终构建出一棵带权路径长度（WPL）最短的二叉树——哈夫曼树。
 * 4. 从根节点出发，向左分支记为 '0'，向右分支记为 '1'，即可得到每个字符的唯一前缀码（哈夫曼编码）。频次越高，编码长度越短，从而实现高效压缩。
 *
 * 时间复杂度: $O(N \log N)$ (构建优先队列需要 $O(N \log N)$，每次合并两节点需要 $O(\log N)$)
 * 空间复杂度: $O(N)$ (存储哈夫曼树节点)
 *
 * @note
 * 【空输入死循环与非法解引用 Bug 审计与修复】：
 * 1. **空输入死循环崩溃 Bug**：如果传入的字符集大小 `size <= 0`，
 *    优先队列 `minHeap` 的初始大小为 0。
 *    循环条件 `while (minHeap.size() != 1)` 会成立（因为 0 != 1），进入循环体。
 *    随后调用 `minHeap.top()`，对空的优先队列进行解引用操作，立即触发**段错误（Segmentation Fault）**崩溃。
 *    **修复**：在函数入口处添加防御性拦截：`if (size <= 0 || data == nullptr || freq == nullptr) return;`。
 * 2. **NULL 关键字现代化**：将传统的 `NULL` 更正为 C++11 标准的 `nullptr`。
 *
 * @author Unknown author
 */

#include <iostream>
#include <queue>
#include <vector>
#include <string>

/**
 * @brief 哈夫曼树节点结构体
 */
struct MinHeapNode {
    char data;          ///< 存放的字符（内部节点默认使用 '$'）
    unsigned freq;      ///< 字符出现频次（权重）
    MinHeapNode *left;  ///< 左子节点指针
    MinHeapNode *right; ///< 右子节点指针

    /**
     * @brief 节点构造函数
     */
    MinHeapNode(char data, unsigned freq) {
        left = right = nullptr; // 核心修复：使用 nullptr 代替 NULL
        this->data = data;
        this->freq = freq;
    }
};

/**
 * @brief 深度优先释放哈夫曼树占用的堆内存空间
 * @param root 树根节点指针
 */
void deleteAll(const MinHeapNode* const root) {
    if (root) {
        deleteAll(root->left);
        deleteAll(root->right);
        delete root; // 递归释放每个节点
    }
}

/**
 * @brief 优先队列比较仿函数（小顶堆）
 */
struct compare {
    bool operator()(const MinHeapNode* const l,
                    const MinHeapNode* const r) const {
        return l->freq > r->freq;
    }
};

/**
 * @brief 深度优先遍历哈夫曼树，生成并输出每个字符的编码结果
 * @param root 当前遍历的树节点
 * @param str 当前累积的二进制编码路径字符串
 */
void printCodes(struct MinHeapNode* root, const std::string& str) {
    if (!root) {
        return;
    }

    // 如果是叶子节点（即实际包含字符的节点，字符不是预设的内部节点标记 '$'）
    if (root->data != '$') {
        std::cout << root->data << ": " << str << "\n";
    }

    // 递归遍历左右子树
    printCodes(root->left, str + "0");
    printCodes(root->right, str + "1");
}

/**
 * @brief 贪心构建哈夫曼编码的主函数
 * @param data 字符数组
 * @param freq 字符对应的频次数组
 * @param size 字符集的总大小
 */
void HuffmanCodes(const char data[], const int freq[], int size) {
    // 核心修复：空输入安全防线，杜绝由于 size 为 0 导致空队列解引用的段错误崩溃
    if (size <= 0 || data == nullptr || freq == nullptr) {
        return;
    }

    struct MinHeapNode *left = nullptr, *right = nullptr;

    // 创建一个小顶堆，并把所有单字符节点压入堆中
    std::priority_queue<MinHeapNode*, std::vector<MinHeapNode*>, compare> minHeap;

    for (int i = 0; i < size; ++i) {
        minHeap.push(new MinHeapNode(data[i], freq[i]));
    }

    // 贪心合并过程：重复提取频次最低的两个节点，合并后重新压回，直到只剩一个根节点
    while (minHeap.size() > 1) {
        // 弹出第一小
        left = minHeap.top();
        minHeap.pop();

        // 弹出第二小
        right = minHeap.top();
        minHeap.pop();

        // 合并生成新的内部节点，权重为二者之和
        auto* const top = new MinHeapNode('$', left->freq + right->freq);

        top->left = left;
        top->right = right;

        minHeap.push(top);
    }

    // 从哈夫曼树的根节点出发打印出所有字符的哈夫曼编码
    printCodes(minHeap.top(), "");
    
    // 深度优先递归清理内存，防范内存泄露
    deleteAll(minHeap.top());
}

/**
 * @brief 主函数
 */
int main() {
    char arr[] = {'a', 'b', 'c', 'd', 'e', 'f'};
    int freq[] = {5, 9, 12, 13, 16, 45};
    int size = sizeof(arr) / sizeof(arr[0]);

    std::cout << "Huffman Codes for characters:" << std::endl;
    HuffmanCodes(arr, freq, size);

    return 0;
}
