/**
 * @file
 * @brief A demo 2-3-4 tree implementation (2-3-4树的演示实现)
 * @details
 * 2-3-4树是一种自平衡的搜索树，与红黑树具有同构性（每个红黑树都可以等价转换为一个2-3-4树）。
 * 通过学习2-3-4树，有助于深入理解红黑树平衡旋转和染色的数学原理。
 * [2–3–4 tree](https://en.wikipedia.org/wiki/2%E2%80%933%E2%80%934_tree)
 * [2-3-4 Trees: A Visual Introduction](https://www.educative.io/page/5689413791121408/80001)
 *
 * @author [liuhuan](https://github.com/fedom)
 */
#include <array>     /// 用于 std::array
#include <cassert>   /// 用于 assert 断言
#include <fstream>   /// 用于文件输出流 (std::ofstream)
#include <iostream>  /// 用于标准输出
#include <memory>    /// 用于智能指针 (std::unique_ptr)
#include <queue>     /// 用于层序遍历队列 (std::queue)
#include <string>    /// 用于 std::to_string

/**
 * @namespace data_structures
 * @brief 数据结构命名空间
 */
namespace data_structures {
/**
 * @namespace tree_234
 * @brief 2-3-4 树相关的命名空间
 */
namespace tree_234 {

/** @brief 2-3-4 树的节点类 */
class Node {
 public:
    /**
     * @brief 节点构造函数
     * @param item 节点保存的第一个初始值
     */
    explicit Node(int64_t item)
        : items({{item, 0, 0}}),
          children({{nullptr, nullptr, nullptr, nullptr}}),
          count(1) {}

    /**
     * @brief 获取当前节点中的有效键值数
     * @return 键值个数 (1代表2-node，2代表3-node，3代表4-node)
     */
    int8_t GetCount() { return count; }

    /**
     * @brief 设置节点有效键值个数（通常仅用于拆分和合并时的底层重组操作）
     * @param c 设置的目标个数
     */
    void SetCount(int8_t c) { count = c; }

    /**
     * @brief 判断该节点是否为叶子节点
     * @return `true` 为叶子节点；`false` 否则
     */
    bool IsLeaf() { return children[0] == nullptr; }

    /**
     * @brief 判断该节点是否已满 (即 4-node，包含 3 个键值)
     * @return `true` 代表已满；`false` 否则
     */
    bool IsFull() { return count == 3; }

    /**
     * @brief 判断该节点是否为 2-node (仅包含 1 个键值)
     * @return `true` 代表是；`false` 否则
     */
    bool Is2Node() { return count == 1; }

    /**
     * @brief 判断该节点是否为 3-node 或 4-node (在删除键值处理下溢时非常有用)
     * @return `true` 代表是；`false` 否则
     */
    bool Is34Node() { return count == 2 || count == 3; }

    /**
     * @brief 检查节点是否包含目标值 item
     */
    bool Contains(int64_t item) {
        for (int8_t i = 0; i < count; i++) {
            if (item == items[i]) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief 获取目标值 item 在节点中的 0 起始索引位置
     * @param item 待查找的键值
     * @return 0 到 2 之间的索引，若不存在则返回 -1
     */
    int8_t GetItemIndex(int64_t item) {
        for (int8_t i = 0; i < count; i++) {
            if (items[i] == item) {
                return i;
            }
        }
        return -1;
    }

    /**
     * @brief 获取当前节点中的最大值（最右侧的值）
     */
    int64_t GetMaxItem() { return items[count - 1]; }

    /**
     * @brief 获取当前节点中的最小值（最左侧的值）
     */
    int64_t GetMinItem() { return items[0]; }

    /**
     * @brief 获取指定索引处的键值
     */
    int64_t GetItem(int8_t index) { return items[index]; }

    /**
     * @brief 设置指定索引处的键值
     */
    void SetItem(int8_t index, int64_t new_item) {
        assert(index >= 0 && index <= 2);
        items[index] = new_item;
    }

    /**
     * @brief 将值 item 插入到节点内部的正确有序位置，并返回对应的索引位置。
     * @details 插入时，调用者需要手动维护其左右孩子指针的平衡与指向。
     *
     * @param item 待插入的值
     * @return 插入成功的索引位置；如果已存在则返回 -1
     */
    int InsertItem(int item) {
        assert(!IsFull());

        if (Contains(item)) {
            return -1;
        }

        int8_t i = 0;
        for (i = 0; i < count; i++) {
            if (items[i] > item) {
                break;
            }
        }

        InsertItemByIndex(i, item, nullptr, true);
        return i;
    }

    /**
     * @brief 按指定索引位置强行插入一个新值及孩子节点指针
     *
     * @param index 插入位置的索引
     * @param item 插入的数值
     * @param with_child 伴随插入的孩子节点指针
     * @param to_left `true` 表示将 with_child 设为新元素左侧孩子；`false` 设为右侧孩子
     */
    void InsertItemByIndex(int8_t index, int64_t item, Node *with_child,
                           bool to_left = true) {
        assert(count < 3 && index >= 0 && index < 3);

        // 数据元素向后平移
        for (int8_t i = count - 1; i >= index; i--) {
            items[i + 1] = items[i];
        }
        items[index] = item;

        // 孩子指针向后平移
        int8_t start_index = to_left ? index : index + 1;
        for (int8_t i = count; i >= start_index; i--) {
            children[i + 1] = children[i];
        }
        children[start_index] = with_child;

        count++;
    }

    /**
     * @brief 根据指定索引位置删除一个键值，并选择保留哪侧的孩子节点指针
     *
     * @param index 待删除键值的索引
     * @param keep_left `true` 保留左孩子并删除右孩子指针；`false` 反之
     * @return 被移除的孩子节点指针
     */
    Node *RemoveItemByIndex(int8_t index, bool keep_left) {
        assert(index >= 0 && index < count);
        Node *removed_child = keep_left ? children[index + 1] : children[index];
        for (int8_t i = index; i < count - 1; i++) {
            items[i] = items[i + 1];
        }

        for (int8_t i = keep_left ? index + 1 : index; i < count; i++) {
            children[i] = children[i + 1];
        }

        count--;
        return removed_child;
    }

    /**
     * @brief 获取某孩子指针在当前节点 children 数组中的索引位置
     */
    int8_t GetChildIndex(Node *child) {
        for (int8_t i = 0; i < count + 1; i++) {
            if (children[i] == child) {
                return i;
            }
        }
        return -1;
    }

    /**
     * @brief 获取指定索引处的孩子节点指针
     */
    Node *GetChild(int8_t index) { return children[index]; }

    /**
     * @brief 设置指定索引位置的孩子指针
     */
    void SetChild(int8_t index, Node *child) { children[index] = child; }

    /**
     * @brief 获取最右侧孩子节点指针
     */
    Node *GetRightmostChild() { return children[count]; }

    /**
     * @brief 获取最左侧孩子节点指针
     */
    Node *GetLeftmostChild() { return children[0]; }

    /**
     * @brief 获取某一项键值其对应的左孩子指针
     */
    Node *GetItemLeftChild(int8_t item_index) {
        if (item_index < 0 || item_index > count - 1) {
            return nullptr;
        }
        return children[item_index];
    }

    /**
     * @brief 获取某一项键值其对应的右孩子指针
     */
    Node *GetItemRightChild(int8_t item_index) {
        if (item_index < 0 || item_index > count - 1) {
            return nullptr;
        }
        return children[item_index + 1];
    }

    /**
     * @brief 沿着键值大小区间寻找可能包含元素 item 的下一个子树分支节点
     * @param item 待查找元素
     * @return 匹配的下一个孩子节点指针
     */
    Node *GetNextPossibleChild(int64_t item) {
        int i = 0;
        for (i = 0; i < count; i++) {
            if (items[i] > item) {
                break;
            }
        }
        return children[i];
    }

 private:
    std::array<int64_t, 3> items;  ///< 最多包含 3 个元素的值数组
    std::array<Node *, 4> children;  ///< 最多包含 4 个分支的孩子节点指针数组
    int8_t count = 0;  ///< 记录当前节点内实际保存的有效键值数
};

/** @brief 2-3-4 树类定义 */
class Tree234 {
 public:
    Tree234() = default;
    Tree234(const Tree234 &) = delete;
    Tree234(const Tree234 &&) = delete;
    Tree234 &operator=(const Tree234 &) = delete;
    Tree234 &operator=(const Tree234 &&) = delete;

    ~Tree234();

    /**
     * @brief 向2-3-4树插入一个键值 item
     */
    void Insert(int64_t item);

    /**
     * @brief 从2-3-4树中删除一个键值 item
     * @return `true` 代表删除成功；`false` 未找到该元素导致删除失败
     */
    bool Remove(int64_t item);

    /** @brief 中序遍历并打印树中元素 */
    void Traverse();

    /**
     * @brief 打印树的结构信息，并输出为 Graphviz 格式的 .dot 文件
     * @param file_name 输出文件名，若为 nullptr 则默认存为 "out.dot"
     */
    void Print(const char *file_name = nullptr);

 private:
    /**
     * @brief 前分裂（Pre-split）插入算法实现
     * @details 在自顶向下寻找插入位置的过程中，一旦遇到满节点 (4-node)，直接将其分裂，
     * 从而保证递归向下时绝不会遇到满的祖先节点，简化了上溢回溯的过程。
     */
    void InsertPreSplit(int64_t item);

    /**
     * @brief 后合并（Post-merge）插入算法实现
     * @details 先自顶向下插入到叶子节点，若导致上溢则从下往上逐层分裂并合并。
     */
    void InsertPostMerge(int64_t item);

    /**
     * @brief 递归插入辅助函数（后合并法）
     */
    Node *Insert(Node *tree, int64_t item);

    /**
     * @brief 后合并法中的合并操作，将溢出的向上节点合并到当前层父节点
     */
    Node *MergeNode(Node *dst_node, Node *node);

    /**
     * @brief 向未满的节点合并一个新分裂出来的 2-node
     */
    void MergeNodeNotFull(Node *dst_node, Node *node);

    /**
     * @brief 将一个满的 4-node 分裂为 1个父节点和2个孩子节点，并返回新父节点
     */
    Node *SplitNode(Node *node);

    /**
     * @brief 寻找指定子树中的最大值
     */
    int64_t GetTreeMaxItem(Node *tree);

    /**
     * @brief 寻找指定子树中的最小值
     */
    int64_t GetTreeMinItem(Node *tree);

    /**
     * @brief 尝试对指定节点执行左旋转操作（从右侧兄弟借调元素）
     * @return `true` 旋转成功；`false` 兄弟节点为 2-node 无法借调
     */
    bool TryLeftRotate(Node *parent, Node *to_child);

    /**
     * @brief 尝试对指定节点执行右旋转操作（从左侧兄弟借调元素）
     * @return `true` 旋转成功；`false` 兄弟节点为 2-node 无法借调
     */
    bool TryRightRotate(Node *parent, Node *to_child);

    /**
     * @brief 执行具体的右旋转键值重组操作
     */
    void RightRotate(Node *parent, int8_t index);

    /**
     * @brief 执行具体的左旋转键值重组操作
     */
    void LeftRotate(Node *parent, int8_t index);

    /**
     * @brief 预合并（Pre-merge）删除算法的核心递归执行函数
     * @details 自顶向下寻找被删除节点，如果遇到 2-node 子节点，为了防止删除后下溢，
     * 提前通过旋转或与兄弟节点合并来消除 2-node，保证被删除节点肯定位于一个 >= 3-node 中。
     */
    bool RemovePreMerge(Node *node, int64_t item);

    /**
     * @brief 合并父节点指定索引位置的键值及其左右两侧的孩子节点
     * @return 合并后的新 4-node 节点指针
     */
    Node *Merge(Node *parent, int8_t index);

    /**
     * @brief 递归释放树节点内存
     */
    void DeleteNode(Node *tree);

    /**
     * @brief 中序遍历并打印树的辅助函数
     */
    void Traverse(Node *tree);

    /**
     * @brief 打印节点信息到 .dot 文件的辅助递归函数
     */
    void PrintNode(std::ofstream &ofs, Node *node, int64_t parent_index,
                   int64_t index, int8_t parent_child_index);

    Node *root_{nullptr};  ///< 树的根节点指针
};

Tree234::~Tree234() { DeleteNode(root_); }

void Tree234::DeleteNode(Node *tree) {
    if (!tree) {
        return;
    }
    for (int8_t i = 0; i <= tree->GetCount(); i++) {
        DeleteNode(tree->GetChild(i));
    }
    delete tree;
}

void Tree234::Traverse() {
    Traverse(root_);
    std::cout << std::endl;
}

void Tree234::Traverse(Node *node) {
    if (!node) {
        return;
    }

    int8_t i = 0;
    for (i = 0; i < node->GetCount(); i++) {
        Traverse(node->GetChild(i));
        std::cout << node->GetItem(i) << ", ";
    }
    Traverse(node->GetChild(i));
}

void Tree234::InsertPreSplit(int64_t item) {
    if (!root_) {
        root_ = new Node(item);
        return;
    }

    Node *parent = nullptr;
    Node *node = root_;

    while (true) {
        if (!node) {
            std::unique_ptr<Node> tmp(new Node(item));
            MergeNodeNotFull(parent, tmp.get());
            return;
        }

        if (node->Contains(item)) {
            return;
        }

        // 如果在向下寻找路径中遇到已满的 4-node，则立即进行分裂
        if (node->IsFull()) {
            node = SplitNode(node);

            Node *cur_node = nullptr;
            if (item < node->GetItem(0)) {
                cur_node = node->GetChild(0);
            } else {
                cur_node = node->GetChild(1);
            }

            if (!parent) {
                root_ = node; // 若分裂的是原根节点，更新根节点
            } else {
                MergeNodeNotFull(parent, node);
            }

            node = cur_node;
        }

        parent = node;
        node = parent->GetNextPossibleChild(item);
    }
}

void Tree234::InsertPostMerge(int64_t item) {
    if (!root_) {
        root_ = new Node(item);
        return;
    }

    Node *split_node = Insert(root_, item);
    if (split_node) {
        root_ = split_node;
    }
}

void Tree234::Insert(int64_t item) { InsertPreSplit(item); }

Node *Tree234::Insert(Node *tree, int64_t item) {
    assert(tree != nullptr);
    std::unique_ptr<Node> split_node;

    if (tree->Contains(item)) {
        return nullptr;
    }

    Node *next_node = tree->GetNextPossibleChild(item);
    if (next_node) {
        split_node.reset(Insert(next_node, item));
    } else {
        split_node.reset(new Node(item));
    }

    if (split_node) {
        return MergeNode(tree, split_node.get());
    }

    return nullptr;
}

Node *Tree234::MergeNode(Node *dst_node, Node *node) {
    assert(dst_node != nullptr && node != nullptr);

    if (!dst_node->IsFull()) {
        MergeNodeNotFull(dst_node, node);
        return nullptr;
    }

    dst_node = SplitNode(dst_node);

    if (node->GetItem(0) < dst_node->GetItem(0)) {
        MergeNodeNotFull(dst_node->GetChild(0), node);
    } else {
        MergeNodeNotFull(dst_node->GetChild(1), node);
    }

    return dst_node;
}

void Tree234::MergeNodeNotFull(Node *dst_node, Node *node) {
    assert(dst_node && node && !dst_node->IsFull() && node->Is2Node());

    int8_t i = dst_node->InsertItem(node->GetItem(0));
    dst_node->SetChild(i, node->GetChild(0));
    dst_node->SetChild(i + 1, node->GetChild(1));
}

Node *Tree234::SplitNode(Node *node) {
    assert(node->GetCount() == 3);

    Node *left = node;
    Node *right = new Node(node->GetItem(2));
    right->SetChild(0, node->GetChild(2));
    right->SetChild(1, node->GetChild(3));

    Node *parent = new Node(node->GetItem(1));
    parent->SetChild(0, left);
    parent->SetChild(1, right);

    left->SetCount(1);
    return parent;
}

bool Tree234::TryLeftRotate(Node *parent, Node *to_child) {
    int to_child_index = parent->GetChildIndex(to_child);

    if (to_child_index >= parent->GetCount()) {
        return false;
    }

    Node *right_sibling = parent->GetChild(to_child_index + 1);
    if (right_sibling->Is2Node()) {
        return false;
    }

    LeftRotate(parent, to_child_index);
    return true;
}

bool Tree234::TryRightRotate(Node *parent, Node *to_child) {
    int8_t to_child_index = parent->GetChildIndex(to_child);

    if (to_child_index <= 0) {
        return false;
    }

    Node *left_sibling = parent->GetChild(to_child_index - 1);
    if (left_sibling->Is2Node()) {
        return false;
    }

    RightRotate(parent, to_child_index - 1);
    return true;
}

void Tree234::RightRotate(Node *parent, int8_t index) {
    Node *left = parent->GetItemLeftChild(index);
    Node *right = parent->GetItemRightChild(index);

    assert(left && left->Is34Node());
    assert(right && right->Is2Node());

    right->InsertItemByIndex(0, parent->GetItem(index),
                             left->GetRightmostChild(), true);
    parent->SetItem(index, left->GetMaxItem());
    left->RemoveItemByIndex(left->GetCount() - 1, true);
}

void Tree234::LeftRotate(Node *parent, int8_t index) {
    Node *left = parent->GetItemLeftChild(index);
    Node *right = parent->GetItemRightChild(index);

    assert(right && right->Is34Node());
    assert(left && left->Is2Node());

    left->InsertItemByIndex(left->GetCount(), parent->GetItem(index),
                            right->GetLeftmostChild(), false);
    parent->SetItem(index, right->GetMinItem());
    right->RemoveItemByIndex(0, false);
}

Node *Tree234::Merge(Node *parent, int8_t index) {
    assert(parent);

    Node *left_child = parent->GetItemLeftChild(index);
    Node *right_child = parent->GetItemRightChild(index);

    assert(left_child->Is2Node() && right_child->Is2Node());

    int64_t item = parent->GetItem(index);

    // 1. 合并父节点的项与右孩子到左孩子节点中，构成 4-node
    left_child->SetItem(1, item);
    left_child->SetItem(2, right_child->GetItem(0));
    left_child->SetChild(2, right_child->GetChild(0));
    left_child->SetChild(3, right_child->GetChild(1));

    left_child->SetCount(3);

    // 2. 从父节点中删除合并下去的元素
    parent->RemoveItemByIndex(index, true);

    // 3. 释放被合并消隐的右孩子节点
    delete right_child;

    return left_child;
}

bool Tree234::Remove(int64_t item) { return RemovePreMerge(root_, item); }

bool Tree234::RemovePreMerge(Node *node, int64_t item) {
    while (node) {
        if (node->IsLeaf()) {
            if (node->Contains(item)) {
                if (node->Is2Node()) {
                    delete node;
                    root_ = nullptr;
                } else {
                    node->RemoveItemByIndex(node->GetItemIndex(item), true);
                }
                return true;
            }
            return false;
        }

        // 处理内部节点
        if (node->Contains(item)) {
            int8_t index = node->GetItemIndex(item);

            Node *left_child = node->GetItemLeftChild(index);
            Node *right_child = node->GetItemRightChild(index);
            assert(left_child && right_child);

            if (left_child->Is2Node() && right_child->Is2Node()) {
                // 左右孩子均为 2-node，必须对它们进行预合并处理
                if (node->Is2Node()) {
                    Node *new_root = Merge(node, index);
                    delete root_;
                    root_ = new_root;
                    node = root_;
                    continue;
                }

                do {
                    if (index > 0) {
                        Node *left_sibling = node->GetItemLeftChild(index - 1);
                        if (left_sibling->Is34Node()) {
                            RightRotate(node, index - 1);
                            break;
                        }
                    }

                    if (index < node->GetCount() - 1) {
                        Node *right_sibling =
                            node->GetItemRightChild(index + 1);
                        if (right_sibling->Is34Node()) {
                            LeftRotate(node, index + 1);
                            break;
                        }
                    }

                    if (index > 0) {
                        Merge(node, index - 1);
                        break;
                    }

                    Merge(node, index + 1);
                } while (false);
            }

            // 更新由于合并或旋转发生改变的孩子节点引用
            left_child = node->GetItemLeftChild(index);
            right_child = node->GetItemRightChild(index);

            if (left_child->Is34Node()) {
                int64_t predecessor_item = GetTreeMaxItem(left_child);
                node->SetItem(node->GetItemIndex(item), predecessor_item);

                node = left_child;
                item = predecessor_item;
                continue;
            }

            if (right_child->Is34Node()) {
                int64_t successor_item = GetTreeMinItem(right_child);
                node->SetItem(node->GetItemIndex(item), successor_item);
                node = right_child;
                item = successor_item;
                continue;
            }
        }

        Node *next_node = node->GetNextPossibleChild(item);
        if (next_node->Is34Node()) {
            node = next_node;
            continue;
        }

        if (TryRightRotate(node, next_node)) {
            node = next_node;
            continue;
        }

        if (TryLeftRotate(node, next_node)) {
            node = next_node;
            continue;
        }

        int8_t child_index = node->GetChildIndex(next_node);
        if (child_index > 0) {
            node = Merge(node, child_index - 1);
        } else {
            node = Merge(node, child_index);
        }
    }
    return false;
}

int64_t Tree234::GetTreeMaxItem(Node *tree) {
    assert(tree);
    int64_t max = 0;
    while (tree) {
        max = tree->GetMaxItem();
        tree = tree->GetRightmostChild();
    }
    return max;
}

int64_t Tree234::GetTreeMinItem(Node *tree) {
    assert(tree);
    int64_t min = 0;
    while (tree) {
        min = tree->GetMinItem();
        tree = tree->GetLeftmostChild();
    }
    return min;
}

void Tree234::Print(const char *file_name) {
    if (!file_name) {
        file_name = "out.dot";
    }

    std::ofstream ofs;
    ofs.open(file_name);
    if (!ofs) {
        std::cout << "create tree dot file failed, " << file_name << std::endl;
        return;
    }

    ofs << "digraph G {\n";
    ofs << "node [shape=record]\n";

    int64_t index = 0;

    struct NodeInfo {
        Node *node;     ///< 树节点
        int64_t index;  ///< 层序遍历中的节点命名序列号，用于边连线
    };

    std::queue<NodeInfo> q;

    if (root_) {
        PrintNode(ofs, root_, -1, index, 0);

        NodeInfo ni{};
        ni.node = root_;
        ni.index = index;
        q.push(ni);

        while (!q.empty()) {
            NodeInfo node_info = q.front();
            q.pop();

            assert(node_info.node->GetCount() > 0);

            if (!node_info.node->IsLeaf()) {
                if (node_info.node->GetCount() > 0) {
                    PrintNode(ofs, node_info.node->GetChild(0), node_info.index,
                              ++index, 0);
                    ni.node = node_info.node->GetChild(0);
                    ni.index = index;
                    q.push(ni);

                    PrintNode(ofs, node_info.node->GetChild(1), node_info.index,
                              ++index, 1);
                    ni.node = node_info.node->GetChild(1);
                    ni.index = index;
                    q.push(ni);
                }

                if (node_info.node->GetCount() > 1) {
                    PrintNode(ofs, node_info.node->GetChild(2), node_info.index,
                              ++index, 2);
                    ni.node = node_info.node->GetChild(2);
                    ni.index = index;
                    q.push(ni);
                }

                if (node_info.node->GetCount() > 2) {
                    PrintNode(ofs, node_info.node->GetChild(3), node_info.index,
                              ++index, 3);
                    ni.node = node_info.node->GetChild(3);
                    ni.index = index;
                    q.push(ni);
                }
            }
        }
    }

    ofs << "}\n";
    ofs.close();
}

void Tree234::PrintNode(std::ofstream &ofs, Node *node, int64_t parent_index,
                        int64_t index, int8_t parent_child_index) {
    assert(node);

    switch (node->GetCount()) {
        case 1:
            ofs << "node_" << index << " [label=\"<f0> " << node->GetItem(0)
                << "\"]\n";
            break;
        case 2:
            ofs << "node_" << index << " [label=\"<f0> " << node->GetItem(0)
                << " | <f1> " << node->GetItem(1) << "\"]\n";
            break;
        case 3:
            ofs << "node_" << index << " [label=\"<f0> " << node->GetItem(0)
                << " | <f1> " << node->GetItem(1) << "| <f2> "
                << node->GetItem(2) << "\"]\n";
            break;

        default:
            break;
    }

    // 绘制父节点到子节点的连接边
    if (parent_index >= 0) {
        ofs << "node_" << parent_index << ":f"
            << (parent_child_index == 0 ? 0 : parent_child_index - 1) << ":"
            << (parent_child_index == 0 ? "sw" : "se") << " -> node_" << index
            << "\n";
    }
}
}  // namespace tree_234
}  // namespace data_structures


/** @brief 简单测试插入给定数组，然后删除其中某项，并打印树 */
static void test1() {
    std::array<int16_t, 13> arr = {3, 1, 5, 4, 2, 9, 10, 8, 7, 6, 16, 13, 14};
    data_structures::tree_234::Tree234 tree;

    for (auto i : arr) {
        tree.Insert(i);
    }

    tree.Remove(5);
    tree.Print();
}

/**
 * @brief 简单插入区间为 [0, n) 的连续数值并中序输出和打印
 * @param n 插入的上限数
 */
static void test2(int64_t n) {
    data_structures::tree_234::Tree234 tree;

    for (int64_t i = 0; i < n; i++) {
        tree.Insert(i);
    }

    tree.Traverse();
    tree.Print((std::to_string(n) + ".dot").c_str());
}

/**
 * @brief 主函数
 * @param argc 命令行参数计数
 * @param argv 命令行参数数组
 * @returns 0
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        test1();  // 执行测试一
    } else {
        test2(std::stoi(argv[1]));  // 执行测试二
    }
    return 0;
}
