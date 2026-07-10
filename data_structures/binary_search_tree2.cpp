/**
 * @file
 * @brief A generic [binary search tree](https://en.wikipedia.org/wiki/Binary_search_tree) implementation. (泛型智能指针二叉搜索树实现)
 * Here you can find more information about the algorithm: [Scaler - Binary Search tree](https://www.scaler.com/topics/data-structures/binary-search-tree/).
 * @see binary_search_tree.cpp
 */

#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

/**
 * @brief 泛型二叉搜索树类模板
 * @tparam T 二叉搜索树的键值（Key）类型
 */
template <class T>
class binary_search_tree {
 private:
    /**
     * @brief 内部表示树节点的结构体
     */
    struct bst_node {
        T value; /**< 节点存储的值 */
        std::unique_ptr<bst_node> left; /**< 指向左子节点的唯一智能指针 */
        std::unique_ptr<bst_node> right; /**< 指向右子节点的唯一智能指针 */

        /**
         * @brief 构造函数，简化节点构建
         * @param _value 构建节点的初始值
         */
        explicit bst_node(T _value) {
            value = _value;
            left = nullptr;
            right = nullptr;
        }
    };

    std::unique_ptr<bst_node> root_; /**< 指向二叉搜索树根节点的指针 */
    std::size_t size_ = 0; /**< 树中包含的元素/节点总数 */

    /**
     * @brief 递归辅助函数：寻找子树中的最大值
     * @param node 当前进行搜索的子树节点指针
     * @param ret_value 传引用参数，用于返回找到的最大值
     * @return `true` 如果搜索成功；`false` 代表树为空
     */
    bool find_max(std::unique_ptr<bst_node>& node, T& ret_value) {
        if (!node) {
            return false;
        } else if (!node->right) {
            // 如果右子树为空，当前节点就是最大值节点
            ret_value = node->value;
            return true;
        }
        // 递归向右子树寻找最大值
        return find_max(node->right, ret_value);
    }

    /**
     * @brief 递归辅助函数：寻找子树中的最小值
     * @param node 当前进行搜索的子树节点指针
     * @param ret_value 传引用参数，用于返回找到的最小值
     * @return `true` 如果搜索成功；`false` 代表树为空
     */
    bool find_min(std::unique_ptr<bst_node>& node, T& ret_value) {
        if (!node) {
            return false;
        } else if (!node->left) {
            // 如果左子树为空，当前节点就是最小值节点
            ret_value = node->value;
            return true;
        }
        // 递归向左子树寻找最小值
        return find_min(node->left, ret_value);
    }

    /**
     * @brief 递归辅助函数：将一个值插入到二叉搜索树中
     * @param node 当前递归检查 of 节点智能指针引用
     * @param new_value 待插入的新值
     * @return `true` 如果成功插入；`false` 如果值已存在导致插入失败
     */
    bool insert(std::unique_ptr<bst_node>& node, T new_value) {
        // 如果当前是根节点且树为空，则在此创建根节点
        if (root_ == node && !root_) {
            root_ = std::unique_ptr<bst_node>(new bst_node(new_value));
            return true;
        }

        if (new_value < node->value) {
            // 新值小于当前值，往左子树移动
            if (!node->left) {
                // 如果左子树为空，直接创建左子节点插入
                node->left = std::unique_ptr<bst_node>(new bst_node(new_value));
                return true;
            } else {
                // 递归往左子树插入
                return insert(node->left, new_value);
            }
        } else if (new_value > node->value) {
            // 新值大于当前值，往右子树移动
            if (!node->right) {
                // 如果右子树为空，直接创建右子节点插入
                node->right =
                    std::unique_ptr<bst_node>(new bst_node(new_value));
                return true;
            } else {
                // 递归往右子树插入
                return insert(node->right, new_value);
            }
        } else {
            // 新值等于当前值，不允许插入重复键，返回 false
            return false;
        }
    }

    /**
     * @brief 递归辅助函数：从二叉搜索树中删除指定值
     * @param parent 当前处理节点 node 的父节点引用
     * @param node 当前进行递归比对的节点引用
     * @param rm_value 待删除的目标值
     * @return `true` 如果成功删除该值；`false` 如果目标值不存在
     */
    bool remove(std::unique_ptr<bst_node>& parent,
                std::unique_ptr<bst_node>& node, T rm_value) {
        if (!node) {
            return false; // 找不到目标值，删除失败
        }

        if (node->value == rm_value) {
            // 找到需要删除的目标节点
            if (node->left && node->right) {
                // 情况 3：节点同时拥有左右子节点
                // 策略：寻找左子树中的最大值节点，拷贝其值到当前节点，然后将左子树中原最大值节点删除
                T successor_node_value{};
                find_max(node->left, successor_node_value);
                remove(root_, root_, successor_node_value); // 递归删除替换节点
                node->value = successor_node_value;
                return true;
            } else if (node->left || node->right) {
                // 情况 2：节点只有一个子节点（左子节点或右子节点）
                // 提取出那个唯一的非空子节点
                std::unique_ptr<bst_node>& non_null =
                    (node->left ? node->left : node->right);

                if (node == root_) {
                    root_ = std::move(non_null); // 若是根节点，将非空子节点提升为新根
                } else if (rm_value < parent->value) {
                    parent->left = std::move(non_null); // 将非空子节点直接链入父节点左侧
                } else {
                    parent->right = std::move(non_null); // 将非空子节点直接链入父节点右侧
                }

                return true;
            } else {
                // 情况 1：节点为叶子节点（没有子节点）
                if (node == root_) {
                    root_.reset(nullptr); // 若是根节点，直接销毁整棵树
                } else if (rm_value < parent->value) {
                    parent->left.reset(nullptr); // 将父节点的左指针清空
                } else {
                    parent->right.reset(nullptr); // 将父节点的右指针清空
                }

                return true;
            }
        } else if (rm_value < node->value) {
            // 递归至左子树中寻找并删除
            return remove(node, node->left, rm_value);
        } else {
            // 递归至右子树中寻找并删除
            return remove(node, node->right, rm_value);
        }
    }

    /**
     * @brief 递归辅助函数：检查树中是否包含指定值
     * @param node 当前搜索的子树节点引用
     * @param value 待查找的目标值
     * @return `true` 包含该值；`false` 不包含
     */
    bool contains(std::unique_ptr<bst_node>& node, T value) {
        if (!node) {
            return false;
        }

        if (value < node->value) {
            return contains(node->left, value);
        } else if (value > node->value) {
            return contains(node->right, value);
        } else {
            return true;
        }
    }

    /**
     * @brief 递归辅助函数：中序遍历二叉树
     * @param callback 对每个节点值调用的函数对象（回调）
     * @param node 当前处理节点引用
     */
    void traverse_inorder(std::function<void(T)> callback,
                          std::unique_ptr<bst_node>& node) {
        if (!node) {
            return;
        }

        traverse_inorder(callback, node->left);
        callback(node->value);
        traverse_inorder(callback, node->right);
    }

    /**
     * @brief 递归辅助函数：前序遍历二叉树
     * @param callback 回调函数
     * @param node 当前处理节点引用
     */
    void traverse_preorder(std::function<void(T)> callback,
                           std::unique_ptr<bst_node>& node) {
        if (!node) {
            return;
        }

        callback(node->value);
        traverse_preorder(callback, node->left);
        traverse_preorder(callback, node->right);
    }

    /**
     * @brief 递归辅助函数：后序遍历二叉树
     * @param callback 回调函数
     * @param node 当前处理节点引用
     */
    void traverse_postorder(std::function<void(T)> callback,
                            std::unique_ptr<bst_node>& node) {
        if (!node) {
            return;
        }

        traverse_postorder(callback, node->left);
        traverse_postorder(callback, node->right);
        callback(node->value);
    }

 public:
    /**
     * @brief 构造一个新的泛型二叉搜索树对象
     */
    binary_search_tree() {
        root_ = nullptr;
        size_ = 0;
    }

    /**
     * @brief 向二叉搜索树插入一个新键值
     * @param new_value 待插入值
     * @return `true` 插入成功；`false` 插入失败
     */
    bool insert(T new_value) {
        bool result = insert(root_, new_value);
        if (result) {
            size_++;
        }
        return result;
    }

    /**
     * @brief 从树中删除一个指定值
     * @param rm_value 待删除值
     * @return `true` 删除成功；`false` 删除失败
     */
    bool remove(T rm_value) {
        bool result = remove(root_, root_, rm_value);
        if (result) {
            size_--;
        }
        return result;
    }

    /**
     * @brief 检查二叉树中是否包含目标值
     * @param value 目标值
     * @return `true` 包含；`false` 不包含
     */
    bool contains(T value) { return contains(root_, value); }

    /**
     * @brief 获取二叉树中的最小值
     * @param ret_value 存放返回的最小值引用
     * @return `true` 获取成功；`false` 二叉树为空
     */
    bool find_min(T& ret_value) { return find_min(root_, ret_value); }

    /**
     * @brief 获取二叉树中的最大值
     * @param ret_value 存放返回的最大值引用
     * @return `true` 获取成功；`false` 二叉树为空
     */
    bool find_max(T& ret_value) { return find_max(root_, ret_value); }

    /**
     * @brief 获取二叉搜索树的元素总节点数
     * @return 节点总数
     */
    std::size_t size() { return size_; }

    /**
     * @brief 以中序遍历的顺序返回树中的所有元素
     * @return 排序好的元素 std::vector
     */
    std::vector<T> get_elements_inorder() {
        std::vector<T> result;
        traverse_inorder([&](T node_value) { result.push_back(node_value); },
                         root_);
        return result;
    }

    /**
     * @brief 以前序遍历的顺序返回树中的所有元素
     * @return 前序排列的元素 std::vector
     */
    std::vector<T> get_elements_preorder() {
        std::vector<T> result;
        traverse_preorder([&](T node_value) { result.push_back(node_value); },
                          root_);
        return result;
    }

    /**
     * @brief 以后序遍历的顺序返回树中的所有元素
     * @return 后序排列的元素 std::vector
     */
    std::vector<T> get_elements_postorder() {
        std::vector<T> result;
        traverse_postorder([&](T node_value) { result.push_back(node_value); },
                           root_);
        return result;
    }
};

/**
 * @brief 测试 insert() 插入函数的单元测试集
 */
static void test_insert() {
    std::cout << "Testing BST insert...";

    binary_search_tree<int> tree;
    bool res = tree.insert(5);
    int min = -1, max = -1;
    assert(res);
    assert(tree.find_max(max));
    assert(tree.find_min(min));
    assert(max == 5);
    assert(min == 5);
    assert(tree.size() == 1);

    tree.insert(4);
    tree.insert(3);
    tree.insert(6);
    assert(tree.find_max(max));
    assert(tree.find_min(min));
    assert(max == 6);
    assert(min == 3);
    assert(tree.size() == 4);

    // 无法插入重复值 4
    bool fail_res = tree.insert(4);
    assert(!fail_res);
    assert(tree.size() == 4);

    std::cout << "ok" << std::endl;
}

/**
 * @brief 测试 remove() 删除函数的单元测试集
 */
static void test_remove() {
    std::cout << "Testing BST remove...";

    binary_search_tree<int> tree;
    tree.insert(5);
    tree.insert(4);
    tree.insert(3);
    tree.insert(6);

    bool res = tree.remove(5);
    int min = -1, max = -1;
    assert(res);
    assert(tree.find_max(max));
    assert(tree.find_min(min));
    assert(max == 6);
    assert(min == 3);
    assert(tree.size() == 3);
    assert(tree.contains(5) == false);

    tree.remove(4);
    tree.remove(3);
    tree.remove(6);
    assert(tree.size() == 0);
    assert(tree.contains(6) == false);

    bool fail_res = tree.remove(5);
    assert(!fail_res);
    assert(tree.size() == 0);

    std::cout << "ok" << std::endl;
}

/**
 * @brief 测试 contains() 查找存在性函数的单元测试集
 */
static void test_contains() {
    std::cout << "Testing BST contains...";

    binary_search_tree<int> tree;
    tree.insert(5);
    tree.insert(4);
    tree.insert(3);
    tree.insert(6);

    assert(tree.contains(5));
    assert(tree.contains(4));
    assert(tree.contains(3));
    assert(tree.contains(6));
    assert(!tree.contains(999));

    std::cout << "ok" << std::endl;
}

/**
 * @brief 测试 find_min() 寻找最小值函数的单元测试集
 */
static void test_find_min() {
    std::cout << "Testing BST find_min...";

    int min = 0;
    binary_search_tree<int> tree;
    assert(!tree.find_min(min));

    tree.insert(5);
    tree.insert(4);
    tree.insert(3);
    tree.insert(6);

    assert(tree.find_min(min));
    assert(min == 3);

    std::cout << "ok" << std::endl;
}

/**
 * @brief 测试 find_max() 寻找最大值函数的单元测试集
 */
static void test_find_max() {
    std::cout << "Testing BST find_max...";

    int max = 0;
    binary_search_tree<int> tree;
    assert(!tree.find_max(max));

    tree.insert(5);
    tree.insert(4);
    tree.insert(3);
    tree.insert(6);

    assert(tree.find_max(max));
    assert(max == 6);

    std::cout << "ok" << std::endl;
}

/**
 * @brief 测试 get_elements_inorder() 中序数组的单元测试集
 */
static void test_get_elements_inorder() {
    std::cout << "Testing BST get_elements_inorder...";

    binary_search_tree<int> tree;
    tree.insert(5);
    tree.insert(4);
    tree.insert(3);
    tree.insert(6);

    std::vector<int> expected = {3, 4, 5, 6};
    std::vector<int> actual = tree.get_elements_inorder();
    assert(actual == expected);

    std::cout << "ok" << std::endl;
}

/**
 * @brief 测试 get_elements_preorder() 前序数组的单元测试集
 */
static void test_get_elements_preorder() {
    std::cout << "Testing BST get_elements_preorder...";

    binary_search_tree<int> tree;
    tree.insert(5);
    tree.insert(4);
    tree.insert(3);
    tree.insert(6);

    std::vector<int> expected = {5, 4, 3, 6};
    std::vector<int> actual = tree.get_elements_preorder();
    assert(actual == expected);

    std::cout << "ok" << std::endl;
}

/**
 * @brief 测试 get_elements_postorder() 后序数组的单元测试集
 */
static void test_get_elements_postorder() {
    std::cout << "Testing BST get_elements_postorder...";

    binary_search_tree<int> tree;
    tree.insert(5);
    tree.insert(4);
    tree.insert(3);
    tree.insert(6);

    std::vector<int> expected = {3, 4, 6, 5};
    std::vector<int> actual = tree.get_elements_postorder();
    assert(actual == expected);

    std::cout << "ok" << std::endl;
}

/**
 * @brief 主函数，依次运行所有单元测试用例
 * @returns 0
 */
int main() {
    test_insert();
    test_remove();
    test_contains();
    test_find_max();
    test_find_min();
    test_get_elements_inorder();
    test_get_elements_preorder();
    test_get_elements_postorder();
}
