/**
 * @file
 * @brief Implementation for finding the [Inorder successor of a binary search tree](https://www.youtube.com/watch?v=5cPbNCrdotA) (二叉搜索树中序后继节点查找算法实现)
 *
 * @details
 * 中序后继节点是指在对二叉树进行中序遍历时，紧跟在当前节点后面的下一个节点。
 * 对于二叉搜索树（BST）中的某个节点，查找其后继节点主要分为以下两种情况：
 *
 * ### 情况 1：当前节点存在右子树
 * 其后继节点必定是右子树中键值最小的节点，即右子树中“最左下”的节点。
 *
 * ### 情况 2：当前节点没有右子树
 * 后继节点一定是其某个祖先节点。我们从根节点开始向下查找该目标节点，
 * 在查找路径上，最后一次向左拐弯时的父节点（即最近的一个将该目标节点包含在左子树中的祖先节点）即为中序后继。
 *
 * 时间复杂度: $O(h)$，其中 $h$ 是 BST 的树高。
 * 空间复杂度: $O(h)$ (由于递归插入/搜索的函数栈深度限制)
 *
 * @note
 * 【死代码与类型安全防卫审计】：
 * 1. **无关的拷贝粘贴垃圾代码**：测试命名空间 `tests` 内包含了无关的 `using ...::circular_linked_list::CircularLinkedList`。
 *    这是从其他实现文件拷贝时遗留的死代码。
 *    **修复**：清理冗余的不相关 `using` 声明。
 * 2. **内存管理防护**：在自测试用例运行完毕后，必须使用 `deallocate` 深度优先遍历递归释放所有已分配的节点内存，防止内存泄漏。
 *
 * @author [Nitin Sharma](https://github.com/foo290)
 */

#include <cassert>   
#include <iostream>  
#include <vector>    

namespace operations_on_datastructures {
namespace inorder_traversal_of_bst {

/**
 * @brief 二叉搜索树节点类
 */
class Node {
 public:
    int64_t data;  ///< 节点存储的数值
    Node *left;    ///< 指向左子树的指针
    Node *right;   ///< 指向右子树的指针
};

/**
 * @brief 在堆中分配并创建一个新的节点
 * @param data 新节点的数值
 * @return 指向新建节点的指针
 */
Node *makeNode(int64_t data) {
    Node *node = new Node();
    node->data = data;      
    node->left = nullptr;   
    node->right = nullptr;  
    return node;
}

/**
 * @brief 在 BST 中递归插入一个键值数据
 * @param root 当前子树的根节点指针
 * @param data 待插入的数据值
 * @return 插入节点后新的子树根节点指针
 */
Node *Insert(Node *root, int64_t data) {
    if (root == nullptr) {
        root = makeNode(data);
    } else if (data <= root->data) {
        root->left = Insert(root->left, data);
    } else {
        root->right = Insert(root->right, data);
    }
    return root;
}

/**
 * @brief 递归检索指定数值在 BST 中对应的节点
 * @param root 当前子树根节点
 * @param data 待查找的数值
 * @return 找到则返回节点指针，否则返回 nullptr
 */
Node *getNode(Node *root, int64_t data) {
    if (root == nullptr) {
        return nullptr;
    } else if (root->data == data) {
        return root;  
    } else if (data > root->data) {
        return getNode(root->right, data);
    } else {
        return getNode(root->left, data);
    }
}

/**
 * @brief 查找当前子树下的最小键值节点（即最左子节点）
 */
Node *findMinNode(Node *root) {
    if (root == nullptr) {
        return root;
    }
    while (root->left != nullptr) {
        root = root->left;
    }
    return root;
}

/**
 * @brief 递归中序遍历打印整棵二叉树
 */
void printInorder(Node *root) {
    if (root == nullptr) {
        return;
    }
    printInorder(root->left);  
    std::cout << root->data << " ";
    printInorder(root->right);  
}

/**
 * @brief 依据数据序列向量快速构建一棵 BST
 */
Node *makeBST(Node *root, const std::vector<int64_t> &data) {
    for (int64_t values : data) {
        root = Insert(root, values);
    }
    return root;
}

/**
 * @brief 获取指定数值节点的中序后继节点
 * @param root BST 根节点
 * @param data 查找目标值
 * @return 中序后继节点指针，若无后继或节点不存在则返回 nullptr
 */
Node *getInorderSuccessor(Node *root, int64_t data) {
    Node *current = getNode(root, data);
    if (current == nullptr) {
        return nullptr;
    }

    // 情况 1：存在右子树，后继为右子树的最小值
    if (current->right != nullptr) {
        return findMinNode(current->right);
    }
    // 情况 2：不存在右子树，寻找最近的将当前节点划在左侧的分叉祖先
    else {
        Node *successor = nullptr;
        Node *ancestor = root;

        while (ancestor != current && ancestor != nullptr) {
            if (current->data < ancestor->data) {
                successor = ancestor;
                ancestor = ancestor->left;  
            } else {
                ancestor = ancestor->right;
            }
        }
        return successor;  
    }
}

/**
 * @brief 递归释放 BST 的所有节点内存
 */
void deallocate(Node *rootNode) {
    if (rootNode == nullptr) {
        return;
    }
    deallocate(rootNode->left);
    deallocate(rootNode->right);
    delete rootNode;
}

}  // namespace inorder_traversal_of_bst
}  // namespace operations_on_datastructures

/**
 * @class TestCases
 * @brief 封装 BST 后继节点测试用例
 */
class TestCases {
 private:
    template <typename T>
    void log(T msg) {
        std::cout << "[TESTS] : ---> " << msg << std::endl;
    }

 public:
    void runTests() {
        log("Running Tests...");
        testCase_1();
        testCase_2();
        testCase_3();
        log("Test Cases over!");
        std::cout << std::endl;
    }

    /**
     * @brief 边界测试：获取最大（即中序遍历最后一位）节点的后继，应返回 nullptr
     */
    void testCase_1() {
        const operations_on_datastructures::inorder_traversal_of_bst::Node *expectedOutput = nullptr;

        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        log("This is test case 1 : ");
        log("Description:");
        log("   EDGE CASE : Printing inorder successor for last node in the BST, Output will be nullptr.");

        operations_on_datastructures::inorder_traversal_of_bst::Node *root = nullptr;
        std::vector<int64_t> node_data{20, 3, 5, 6, 2, 23, 45, 78, 21};

        root = operations_on_datastructures::inorder_traversal_of_bst::makeBST(root, node_data);

        std::cout << "Inorder sequence is : ";
        operations_on_datastructures::inorder_traversal_of_bst::printInorder(root);
        std::cout << std::endl;

        operations_on_datastructures::inorder_traversal_of_bst::Node *inorderSuccessor = 
            operations_on_datastructures::inorder_traversal_of_bst::getInorderSuccessor(root, 78);

        log("Checking assert expression...");
        assert(inorderSuccessor == expectedOutput);
        log("Assertion check passed!");

        operations_on_datastructures::inorder_traversal_of_bst::deallocate(root);  

        log("[PASS] : TEST CASE 1 PASS!");
        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    }

    /**
     * @brief 基础测试：获取根节点 20 的后继，应为 21
     */
    void testCase_2() {
        const int expectedOutput = 21;

        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        log("This is test case 2 : ");

        operations_on_datastructures::inorder_traversal_of_bst::Node *root = nullptr;
        std::vector<int64_t> node_data{20, 3, 5, 6, 2, 23, 45, 78, 21};

        root = operations_on_datastructures::inorder_traversal_of_bst::makeBST(root, node_data);

        std::cout << "Inorder sequence is : ";
        operations_on_datastructures::inorder_traversal_of_bst::printInorder(root);
        std::cout << std::endl;

        operations_on_datastructures::inorder_traversal_of_bst::Node *inorderSuccessor = 
            operations_on_datastructures::inorder_traversal_of_bst::getInorderSuccessor(root, 20);

        log("Checking assert expression...");
        assert(inorderSuccessor->data == expectedOutput);
        log("Assertion check passed!");

        operations_on_datastructures::inorder_traversal_of_bst::deallocate(root);  

        log("[PASS] : TEST CASE 2 PASS!");
        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    }

    /**
     * @brief 基础测试：获取叶节点 90 的后继，应为其祖先 110
     */
    void testCase_3() {
        const int expectedOutput = 110;

        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        log("This is test case 3 : ");

        operations_on_datastructures::inorder_traversal_of_bst::Node *root = nullptr;
        std::vector<int64_t> node_data{89, 67, 32, 56, 90, 123, 120, 110, 115, 6, 78, 7, 10};

        root = operations_on_datastructures::inorder_traversal_of_bst::makeBST(root, node_data);

        std::cout << "Inorder sequence is : ";
        operations_on_datastructures::inorder_traversal_of_bst::printInorder(root);
        std::cout << std::endl;

        operations_on_datastructures::inorder_traversal_of_bst::Node *inorderSuccessor = 
            operations_on_datastructures::inorder_traversal_of_bst::getInorderSuccessor(root, 90);

        log("Checking assert expression...");
        assert(inorderSuccessor->data == expectedOutput);
        log("Assertion check passed!");

        operations_on_datastructures::inorder_traversal_of_bst::deallocate(root);  

        log("[PASS] : TEST CASE 3 PASS!");
        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    }
};

static void test() {
    TestCases tc;
    tc.runTests();
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试用例

    operations_on_datastructures::inorder_traversal_of_bst::Node *root = nullptr;  
    std::vector<int64_t> node_data{3, 4, 5, 89, 1, 2};  

    int64_t targetElement = 4;  
    root = operations_on_datastructures::inorder_traversal_of_bst::makeBST(root, node_data);  

    operations_on_datastructures::inorder_traversal_of_bst::Node *inorderSuccessor = 
        operations_on_datastructures::inorder_traversal_of_bst::getInorderSuccessor(root, targetElement);

    std::cout << "In-order sequence is : ";
    operations_on_datastructures::inorder_traversal_of_bst::printInorder(root);
    std::cout << std::endl;

    if (inorderSuccessor == nullptr) {
        std::cout << "Inorder successor for last node is NULL" << std::endl;
    } else {
        std::cout << "Target element is : " << targetElement << std::endl;
        std::cout << "Inorder successor for target element is : " << inorderSuccessor->data << std::endl;
    }

    operations_on_datastructures::inorder_traversal_of_bst::deallocate(root);  

    return 0;
}
