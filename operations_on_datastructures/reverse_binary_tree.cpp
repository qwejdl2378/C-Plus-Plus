/**
 * @file
 * @brief Implementation for the [Reversing a Binary Tree](https://www.geeksforgeeks.org/reverse-tree-path/) recursively algorithm (递归翻转二叉树算法实现)
 *
 * @details
 * 翻转二叉树（又称镜像二叉树）是指将二叉树中所有节点的左右子树进行互换。
 * 本实现采用自底向上的递归交换法，在访问每个节点时：
 * 1. 递归翻转其左子树。
 * 2. 递归翻转其右子树。
 * 3. 交换当前节点的左右子树指针。
 *
 * 时间复杂度: $O(N)$，其中 $N$ 是二叉树的节点总数，因为每个节点恰好被访问一次。
 * 空间复杂度: $O(h)$，其中 $h$ 是树的高度，主要用于递归系统调用栈。
 *
 * @note
 * 【迭代析构与内存回收优化】：
 * 1. **避免递归析构爆栈**：使用基于辅助栈的非递归方式释放二叉树内存，防止在极深树结构下因递归调用析构导致栈溢出。
 * 2. **优化空节点压栈**：原析构逻辑会将空子节点也压入辅助栈。
 *    **修复**：添加判断，只将非空子节点压入，提高空间效率并优化运行速度。
 *
 * @author [Alvin](https://github.com/polarvoid)
 */

#include <cassert>   
#include <iostream>  
#include <queue>     
#include <vector>    

namespace operations_on_datastructures {
namespace reverse_binary_tree {

/**
 * @brief 二叉树节点结构体
 */
struct Node {
    int64_t data;  ///< 节点数值
    Node* left;    ///< 左子节点指针
    Node* right;   ///< 右子节点指针
    
    explicit Node(int64_t _data) : data(_data), left(nullptr), right(nullptr) {}
};

/**
 * @brief 二叉搜索树/二叉树类
 */
class BinaryTree {
 private:
    Node* root;  ///< 根节点指针

    /**
     * @brief 递归在二叉搜索树中插入节点
     */
    Node* insert(int64_t data, Node* pivot) {
        if (pivot == nullptr) {
            return new Node(data);  
        }
        if (data <= pivot->data) {
            pivot->left = insert(data, pivot->left);  
        } else {
            pivot->right = insert(data, pivot->right);  
        }
        return pivot;
    }

    /**
     * @brief 递归翻转二叉树核心函数
     */
    Node* reverseBinaryTree(Node* pivot) {
        if (pivot == nullptr) {
            return nullptr;  
        }
        Node* temp = pivot->left;  
        pivot->left = reverseBinaryTree(pivot->right);  
        pivot->right = reverseBinaryTree(temp);         
        return pivot;
    }

    // 禁止拷贝构造和赋值
    BinaryTree(const BinaryTree&) = delete;
    BinaryTree& operator=(const BinaryTree&) = delete;

 public:
    BinaryTree() : root(nullptr) {}

    explicit BinaryTree(int64_t data) : root(new Node(data)) {}

    /**
     * @brief 析构函数，使用非递归栈方式安全释放所有节点内存
     */
    ~BinaryTree() {
        if (root == nullptr) return;
        std::vector<Node*> nodes;
        nodes.emplace_back(root);
        while (!nodes.empty()) {
            Node* cur_node = nodes.back();
            nodes.pop_back();
            // 核心优化：避免将空指针压入 vector
            if (cur_node->left != nullptr) {
                nodes.emplace_back(cur_node->left);
            }
            if (cur_node->right != nullptr) {
                nodes.emplace_back(cur_node->right);
            }
            delete cur_node;
        }
    }

    /**
     * @brief 添加一个节点到树中
     */
    void add(int64_t data) { root = insert(data, root); }

    /**
     * @brief 翻转整棵树
     */
    void reverse() { root = reverseBinaryTree(root); }

    /**
     * @brief 广度优先搜索/层序遍历二叉树，按层返回数据向量
     */
    std::vector<int64_t> get_level_order() const {
        std::vector<int64_t> data;  
        if (root == nullptr) {
            return data;  
        }
        std::queue<Node*> nodes;  
        nodes.push(root);         
        while (!nodes.empty()) {
            Node* temp = nodes.front();  
            data.push_back(temp->data);  
            nodes.pop();                 
            if (temp->left != nullptr) {
                nodes.push(temp->left);  
            }
            if (temp->right != nullptr) {
                nodes.push(temp->right);  
            }
        }  
        return data;
    }

    /**
     * @brief 按层序序列打印整棵树到标准输出
     */
    void print() const {
        for (int i : get_level_order()) {
            std::cout << i << " ";  
        }
        std::cout << "\n";  
    }
};

}  // namespace reverse_binary_tree
}  // namespace operations_on_datastructures

namespace tests {
using operations_on_datastructures::reverse_binary_tree::BinaryTree;

/**
 * @brief 单元自测用例 1：单个元素树的翻转
 */
void test1() {
    BinaryTree bst;
    std::vector<int64_t> pre_reversal, post_reversal;
    std::cout << "TEST CASE 1\n";
    std::cout << "Initializing tree with a single element (5)\n";
    bst.add(5);
    pre_reversal = bst.get_level_order();
    std::cout << "Before reversal: ";
    bst.print();
    std::cout << "After reversal: ";
    bst.reverse();
    post_reversal = bst.get_level_order();
    assert(pre_reversal.size() == post_reversal.size());  
    assert(pre_reversal.size() == 1);  
    assert(pre_reversal[0] == post_reversal[0]);  
    bst.print();
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 单元自测用例 2：空树的翻转
 */
void test2() {
    BinaryTree bst;
    std::vector<int64_t> pre_reversal, post_reversal;
    std::cout << "TEST CASE 2\n";
    std::cout << "Creating empty tree (root points to NULL)\n";
    pre_reversal = bst.get_level_order();
    std::cout << "Before reversal: ";
    bst.print();
    std::cout << "After reversal: ";
    bst.reverse();
    post_reversal = bst.get_level_order();
    assert(pre_reversal.size() == post_reversal.size());  
    assert(pre_reversal.size() == 0);  
    bst.print();
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 单元自测用例 3：常规二叉树的多层翻转
 */
void test3() {
    BinaryTree bst;
    std::vector<int64_t> pre_reversal, post_reversal;
    std::vector<int64_t> pre_res = {4, 3, 6, 2, 5, 7, 1};
    std::vector<int64_t> post_res = {4, 6, 3, 7, 5, 2, 1};
    std::cout << "TEST CASE 3\n";
    std::cout << "Creating tree with elements (4, 6, 3, 2, 5, 7, 1)\n";
    bst.add(4);
    bst.add(6);
    bst.add(3);
    bst.add(2);
    bst.add(5);
    bst.add(7);
    bst.add(1);
    pre_reversal = bst.get_level_order();
    assert(pre_reversal == pre_res);  
    std::cout << "Before reversal: ";
    bst.print();
    std::cout << "After reversal: ";
    bst.reverse();
    post_reversal = bst.get_level_order();
    assert(post_reversal == post_res);  
    bst.print();
    std::cout << "TEST PASSED!\n\n";
}
}  // namespace tests

static void test() {
    tests::test1();  
    tests::test2();  
    tests::test3();  
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试用例确认正确性
    return 0;
}
