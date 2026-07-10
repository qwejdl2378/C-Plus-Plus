/**
 * \file
 * \brief A simple tree implementation using nodes (AVL 树实现)
 *
 * \todo update code to use C++ STL library features and OO structure
 * \warning This program is a poor implementation and does not utilize any of
 * the C++ STL features.
 */
#include <algorithm>  /// for std::max
#include <iostream>   /// for std::cout
#include <queue>      /// for std::queue

// 定义 AVL 树的节点结构
using node = struct node {
    int data;           // 节点存储的数据
    int height;         // 节点当前的高度（用于平衡因子的计算）
    struct node *left;  // 指向左子树的指针
    struct node *right; // 指向右子树的指针
};

/**
 * @brief 创建并返回一个新节点
 * @param[in] data 存储在节点中的数值
 * @return 指向新创建节点的指针
 */
node *createNode(int data) {
    node *nn = new node();
    nn->data = data;
    nn->height = 0;
    nn->left = nullptr;
    nn->right = nullptr;
    return nn;
}

/**
 * @brief 计算树的高度
 * @param[in] root 树的根节点
 * @return 树的高度（空树为0，否则为左右子树最大高度加1）
 */
int height(node *root) {
    if (root == nullptr) {
        return 0;
    }
    return 1 + std::max(height(root->left), height(root->right));
}

/**
 * @brief 获取节点的平衡因子
 * @param[in] root 树的根节点
 * @return 左右子树的高度差（左子树高度 - 右子树高度）
 */
int getBalance(node *root) { return height(root->left) - height(root->right); }

/**
 * @brief 右旋转操作（LL 型失衡时使用，或者 LR 型失衡的第二步）
 * 
 *     root                  t
 *     / \                  / \
 *    t   u   ====>        L  root
 *   / \                      / \
 *  L   u2                   u2  u
 * 
 * @param root 需要旋转的失衡子树根节点
 * @return 旋转后的新子树根节点
 */
node *rightRotate(node *root) {
    node *t = root->left;
    node *u = t->right;
    t->right = root;
    root->left = u;
    return t;
}

/**
 * @brief 左旋转操作（RR 型失衡时使用，或者 RL 型失衡的第二步）
 * 
 *   root                    t
 *   /  \                   / \
 *  u    t     ====>     root  R
 *      / \              /  \
 *     t2  R            u   t2
 * 
 * @param root 需要旋转的失衡子树根节点
 * @return 旋转后的新子树根节点
 */
node *leftRotate(node *root) {
    node *t = root->right;
    node *u = t->left;
    t->left = root;
    root->right = u;
    return t;
}

/**
 * @brief 获取树中包含最小值的节点
 * @param root 树的根节点
 * @returns 包含最小值的节点指针（最左侧的叶子或半叶子节点）
 */
node *minValue(node *root) {
    if (root->left == nullptr) {
        return root;
    }
    return minValue(root->left);
}

/**
 * @brief 将新元素插入 AVL 树，并在插入后自动进行旋转平衡
 * @param root 树的根节点
 * @param[in] item 需要插入的元素值
 * @return 更新后的树根节点指针
 */
node *insert(node *root, int item) {
    // 1. 执行标准的 BST 插入步骤
    if (root == nullptr) {
        return createNode(item);
    }
    if (item < root->data) {
        root->left = insert(root->left, item);
    } else {
        root->right = insert(root->right, item);
    }

    // 2. 获取当前节点的平衡因子，检测是否失衡
    int b = getBalance(root);

    // 如果左子树比右子树高超过 1
    if (b > 1) {
        // LR 型：新节点插入在左子树的右侧，先对左子树左旋，转换为 LL 型
        if (getBalance(root->left) < 0) {
            root->left = leftRotate(root->left);  // Left-Right Case
        }
        // LL 型：直接右旋
        return rightRotate(root);  // Left-Left Case
    } 
    // 如果右子树比左子树高超过 1
    else if (b < -1) {
        // RL 型：新节点插入在右子树的左侧，先对右子树右旋，转换为 RR 型
        if (getBalance(root->right) > 0) {
            root->right = rightRotate(root->right);  // Right-Left Case
        }
        // RR 型：直接左旋
        return leftRotate(root);  // Right-Right Case
    }
    return root;
}

/**
 * @brief 从 AVL 树中删除指定元素
 * @param root 树的根节点
 * @param[in] element 需要被删除的元素值
 * @return 更新后的树根节点指针
 */
node *deleteNode(node *root, int element) {
    if (root == nullptr) {
        return root;
    }

    // 1. 标准的 BST 删除步骤
    if (element < root->data) {
        root->left = deleteNode(root->left, element);
    } else if (element > root->data) {
        root->right = deleteNode(root->right, element);
    } else {
        // 找到待删除节点
        // 情况 A & B: 待删除节点是叶子节点，或只有一个子节点
        if (!root->right || !root->left) {
            node *temp = !root->right ? root->left : root->right;
            delete root;
            return temp;
        }
        // 情况 C: 待删除节点有两个子节点
        // 找到右子树中最小的节点（中序后继），用其数值覆盖当前节点，然后递归删除该中序后继
        node *temp = minValue(root->right);
        root->data = temp->data;
        root->right = deleteNode(root->right, temp->data);
    }
    // 2. 删除后进行树的自平衡处理
    return root;
}

/**
 * @brief 释放树中所有节点的内存
 * @param root 树的根节点
 */
void deleteAllNodes(const node *const root) {
    if (root) {
        deleteAllNodes(root->left);
        deleteAllNodes(root->right);
        delete root;
    }
}

/**
 * @brief 使用队列层序遍历（BFS）并打印 AVL 树
 * @param[in] root 树的根节点
 */
void levelOrder(node *root) {
    std::queue<node *> q;
    q.push(root);
    while (!q.empty()) {
        root = q.front();
        std::cout << root->data << " ";
        q.pop();
        if (root->left) {
            q.push(root->left);
        }
        if (root->right) {
            q.push(root->right);
        }
    }
}

/**
 * @brief 主函数，演示 AVL 树的插入、删除及平衡效果
 * @returns 0 表示程序正常退出
 */
int main() {
    // 测试 AVL 树
    node *root = nullptr;
    int i = 0;
    // 依次插入 1 到 7，AVL 树会在过程中进行旋转保持平衡
    for (i = 1; i <= 7; i++) root = insert(root, i);
    std::cout << "LevelOrder: ";
    levelOrder(root);

    // 删除节点 1，观察层序遍历结果
    root = deleteNode(root, 1);  // Deleting key with value 1
    std::cout << "\nLevelOrder: ";
    levelOrder(root);

    // 删除节点 4，观察平衡调整后的层序遍历结果
    root = deleteNode(root, 4);  // Deleting key with value 4
    std::cout << "\nLevelOrder: ";
    levelOrder(root);

    // 清理所有节点内存
    deleteAllNodes(root);
    return 0;
}
