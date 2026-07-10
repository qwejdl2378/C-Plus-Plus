/**
 * \file
 * \brief A simple tree implementation using structured nodes (C风格二叉搜索树的基本实现)
 *
 * \todo update code to use C++ STL library features and OO structure
 * \warning This program is a poor implementation - C style - and does not
 * utilize any of the C++ STL features.
 */
#include <iostream>

/**
 * @brief 树节点结构体
 */
struct node {
    int val;         ///< 节点存储的键值
    node *left;      ///< 指向左子树的指针
    node *right;     ///< 指向右子树的指针
};

/**
 * @brief 用于二叉树广度优先遍历 (BFT) 的队列结构体
 */
struct Queue {
    node *t[100];    ///< 固定大小为 100 的节点指针数组
    int front;       ///< 队头指针索引
    int rear;        ///< 队尾指针索引
};

Queue queue; ///< 全局队列变量

/**
 * @brief 元素入队
 * @param n 指向入队节点的指针
 */
void enqueue(node *n) { queue.t[queue.rear++] = n; }

/**
 * @brief 元素出队
 * @returns 队头出队节点的指针
 */
node *dequeue() { return (queue.t[queue.front++]); }

/**
 * @brief 向二叉搜索树中插入一个新值 x
 * @param n 当前比对的树节点
 * @param x 待插入的整数值
 */
void Insert(node *n, int x) {
    if (x < n->val) {
        // 如果 x 小于当前节点值，则应插入左子树
        if (n->left == NULL) {
            // 左子树为空，在此处创建新节点插入
            node *temp = new node;
            temp->val = x;
            temp->left = NULL;
            temp->right = NULL;
            n->left = temp;
        } else {
            // 左子树不为空，递归向左子树插入
            Insert(n->left, x);
        }
    } else {
        // 如果 x 大于等于当前节点值，则应插入右子树
        if (n->right == NULL) {
            // 右子树为空，在此处创建新节点插入
            node *temp = new node;
            temp->val = x;
            temp->left = NULL;
            temp->right = NULL;
            n->right = temp;
        } else {
            // 右子树不为空，递归向右子树插入
            Insert(n->right, x);
        }
    }
}

/**
 * @brief 寻找指定子树中的最大键值（通常用于寻找左子树中的最大节点以替换被删除节点）
 * @param n 子树根节点
 * @returns 子树中的最大整数值
 */
int findMaxInLeftST(node *n) {
    while (n->right != NULL) {
        n = n->right; // 一直向右下寻找
    }
    return n->val;
}

/**
 * @brief 从二叉搜索树中删除值为 x 的节点
 * @param p 当前节点 n 的父节点指针
 * @param n 当前递归遍历的节点指针
 * @param x 待删除的整数目标值
 */
void Remove(node *p, node *n, int x) {
    if (n->val == x) {
        // 找到了目标值节点 n，准备进行删除
        if (n->right == NULL && n->left == NULL) {
            // 情况 1：被删除节点是叶子节点（左右子树皆为空）
            if (x < p->val) {
                p->right = NULL;
            } else {
                p->left = NULL;
            }
            delete n; // 释放内存
        } else if (n->right == NULL) {
            // 情况 2a：被删除节点只有左子树
            if (x < p->val) {
                p->right = n->left;
            } else {
                p->left = n->left;
            }
            delete n;
        } else if (n->left == NULL) {
            // 情况 2b：被删除节点只有右子树
            if (x < p->val) {
                p->right = n->right;
            } else {
                p->left = n->right;
            }
            delete n;
        } else {
            // 情况 3：被删除节点同时拥有左子树和右子树
            // 策略：寻找左子树中的最大值，将其复制到当前节点，然后删除左子树中原最大值节点
            int y = findMaxInLeftST(n->left);
            n->val = y;
            Remove(n, n->left, y); // 递归删除左子树中用于替换的值为 y 的节点
        }
    } else if (x < n->val) {
        // 目标值 x 小于当前节点值，向左子树递归查找并删除
        Remove(n, n->left, x);
    } else {
        // 目标值 x 大于当前节点值，向右子树递归查找并删除
        Remove(n, n->right, x);
    }
}

/**
 * @brief 二叉树的广度优先遍历 (Breadth-First Traversal, BFT) 亦称层序遍历
 * @param n 树根节点指针
 */
void BFT(node *n) {
    if (n != NULL) {
        std::cout << n->val << "  ";
        enqueue(n->left);  // 将左子节点入队
        enqueue(n->right); // 将右子节点入队
        BFT(dequeue());    // 出队下一个节点并递归执行层序遍历
    }
}

/**
 * @brief 前序遍历 (Pre-order Traversal) 根 -> 左 -> 右
 * @param n 当前节点指针
 */
void Pre(node *n) {
    if (n != NULL) {
        std::cout << n->val << "  ";
        Pre(n->left);
        Pre(n->right);
    }
}

/**
 * @brief 中序遍历 (In-order Traversal) 左 -> 根 -> 右
 * @param n 当前节点指针
 */
void In(node *n) {
    if (n != NULL) {
        In(n->left);
        std::cout << n->val << "  ";
        In(n->right);
    }
}

/**
 * @brief 后序遍历 (Post-order Traversal) 左 -> 右 -> 根
 * @param n 当前节点指针
 */
void Post(node *n) {
    if (n != NULL) {
        Post(n->left);
        Post(n->right);
        std::cout << n->val << "  ";
    }
}

/**
 * @brief 主函数，提供交互式的菜单式二叉搜索树操作界面
 * @returns 0
 */
int main() {
    queue.front = 0;
    queue.rear = 0;
    int value;
    int ch;
    node *root = new node;
    std::cout << "\nEnter the value of root node :";
    std::cin >> value;
    root->val = value;
    root->left = NULL;
    root->right = NULL;
    do {
        std::cout << "\n1. Insert"
                  << "\n2. Delete"
                  << "\n3. Breadth First"
                  << "\n4. Preorder Depth First"
                  << "\n5. Inorder Depth First"
                  << "\n6. Postorder Depth First"
                  << "\n0. Exit";

        std::cout << "\nEnter Your Choice : ";
        std::cin >> ch;
        int x;
        switch (ch) {
        case 1:
            std::cout << "\nEnter the value to be Inserted : ";
            std::cin >> x;
            Insert(root, x);
            break;
        case 2:
            std::cout << "\nEnter the value to be Deleted : ";
            std::cin >> x;
            Remove(root, root, x);
            break;
        case 3:
            BFT(root);
            break;
        case 4:
            Pre(root);
            break;
        case 5:
            In(root);
            break;
        case 6:
            Post(root);
            break;
        }
    } while (ch != 0);

    return 0;
}
