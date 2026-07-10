#include <iostream>
#include <list>

using namespace std;

/**
 * @brief 树节点结构体定义
 */
struct node {
    int val;         ///< 节点存储的整数值
    node *left;      ///< 指向左子节点的指针
    node *right;     ///< 指向右子节点的指针
};

/**
 * @brief 交互式地向二叉树中插入一个新节点
 * @details 用户可以通过输入 'l' (左) 或 'r' (右) 来手动指定新节点在当前节点的哪个子树分支下插入。
 *
 * @param curr 当前节点 n 的父节点指针
 * @param n 当前遍历比对的节点指针
 * @param x 待插入的键值
 * @param pos 当前指示的子树方向标记 ('l' 或 'r')
 */
void CreateTree(node *curr, node *n, int x, char pos) {
    if (n != NULL) {
        // 如果当前子节点不为空，提示用户继续选择左侧还是右侧分支递归下去
        char ch;
        cout << "\nLeft or Right of " << n->val << " : ";
        cin >> ch;
        if (ch == 'l')
            CreateTree(n, n->left, x, ch);
        else if (ch == 'r')
            CreateTree(n, n->right, x, ch);
    } else {
        // 如果遇到了空子节点，在此处创建新节点并挂载到父节点 curr 下的对应位置
        node *t = new node;
        t->val = x;
        t->left = NULL;
        t->right = NULL;
        if (pos == 'l') {
            curr->left = t;
        } else if (pos == 'r') {
            curr->right = t;
        }
    }
}

/**
 * @brief 广度优先遍历 (Breadth-First Traversal, BFT)，使用 std::list 作为队列实现
 * @param n 树根节点指针
 */
void BFT(node *n) {
    list<node *> queue; // 创建辅助队列

    queue.push_back(n); // 根节点入队

    while (!queue.empty()) {
        n = queue.front();
        cout << n->val << "  "; // 访问队头节点值
        queue.pop_front();      // 出队

        // 将当前节点的左右子节点入队
        if (n->left != NULL)
            queue.push_back(n->left);
        if (n->right != NULL)
            queue.push_back(n->right);
    }
}

/**
 * @brief 前序遍历 (Pre-order Traversal) 根 -> 左 -> 右
 * @param n 当前处理的节点
 */
void Pre(node *n) {
    if (n != NULL) {
        cout << n->val << "  ";
        Pre(n->left);
        Pre(n->right);
    }
}

/**
 * @brief 中序遍历 (In-order Traversal) 左 -> 根 -> 右
 * @param n 当前处理的节点
 */
void In(node *n) {
    if (n != NULL) {
        In(n->left);
        cout << n->val << "  ";
        In(n->right);
    }
}

/**
 * @brief 后序遍历 (Post-order Traversal) 左 -> 右 -> 根
 * @param n 当前处理的节点
 */
void Post(node *n) {
    if (n != NULL) {
        Post(n->left);
        Post(n->right);
        cout << n->val << "  ";
    }
}

/**
 * @brief 主函数，提供交互菜单以手动生成和遍历二叉树
 * @returns 0
 */
int main() {
    int value;
    int ch;
    node *root = new node;
    cout << "\nEnter the value of root node :";
    cin >> value;
    root->val = value;
    root->left = NULL;
    root->right = NULL;
    do {
        cout << "\n1. Insert";
        cout << "\n2. Breadth First";
        cout << "\n3. Preorder Depth First";
        cout << "\n4. Inorder Depth First";
        cout << "\n5. Postorder Depth First"
             << "\n0. Exit";

        cout << "\nEnter Your Choice : ";
        cin >> ch;
        switch (ch) {
        case 1:
            int x;
            char pos;
            cout << "\nEnter the value to be Inserted : ";
            cin >> x;
            cout << "\nLeft or Right of Root : ";
            cin >> pos;
            if (pos == 'l')
                CreateTree(root, root->left, x, pos);
            else if (pos == 'r')
                CreateTree(root, root->right, x, pos);
            break;
        case 2:
            BFT(root);
            break;
        case 3:
            Pre(root);
            break;
        case 4:
            In(root);
            break;
        case 5:
            Post(root);
            break;
        }
    } while (ch != 0);
    return 0;
}
