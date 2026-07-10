#include <iostream>
#include <queue>

/**************************
    @author shrutisheoran
**************************/

using namespace std;

/**
 * @brief 二叉树节点结构体定义
 */
struct Btree {
    int data;             ///< 节点保存的值
    struct Btree *left;   ///< 指向左子树的指针
    struct Btree *right;  ///< 指向右子树的指针
};

/**
 * @brief 采用层序插入（使用队列）的方式向二叉树插入一个新节点，以保证构建的是一棵完全二叉树
 * @param root 根节点指针的指针（以便在树空时修改根节点指向）
 * @param d 待插入的整数值
 */
void insert(Btree **root, int d) {
    Btree *nn = new Btree();  // 创建新节点
    nn->data = d;
    nn->left = NULL;
    nn->right = NULL;
    if (*root == NULL) {
        *root = nn;
        return;
    } else {
        queue<Btree *> q;
        // 将根节点推入队列
        q.push(*root);
        while (!q.empty()) {
            Btree *node = q.front();
            // 出队队头节点以进行检查
            q.pop();
            
            if (node->left) {
                // 如果左子树存在，入队以备后续层次查找
                q.push(node->left);
            } else {
                // 左子树为空，在此插入新节点并返回
                node->left = nn;
                return;
            }
            
            if (node->right) {
                // 如果右子树存在，入队以备后续层次查找
                q.push(node->right);
            } else {
                // 右子树为空，在此插入新节点并返回
                node->right = nn;
                return;
            }
        }
    }
}

/**
 * @brief 莫里斯中序遍历 (Morris In-order Traversal) 算法实现
 * @details 该算法利用叶子节点的空闲指针建立线索二叉树，从而不需要栈或递归，在 O(1) 辅助空间下完成中序遍历。
 * @param root 树根节点指针
 */
void morrisInorder(Btree *root) {
    Btree *curr = root;
    Btree *temp;
    while (curr) {
        if (curr->left == NULL) {
            // 如果左子树为空，说明没有比当前节点更小的前驱了，直接访问该节点，并跳转到右子树
            cout << curr->data << " ";
            curr = curr->right;
        } else {
            // 寻找当前节点在左子树中的中序前驱（左子树中最右侧的那个节点）
            temp = curr->left;
            while (temp->right && temp->right != curr) {
                temp = temp->right;
            }
            
            // 如果前驱节点的右指针为空，则建立线索指向当前节点 curr，然后转向左子树继续访问
            if (temp->right == NULL) {
                temp->right = curr; // 建立回溯线索
                curr = curr->left;
            }
            // 如果前驱节点的右指针已经指向当前节点 curr，说明左子树已经全部访问完毕，
            // 应当拆除此临时线索恢复原树状结构，访问当前节点，然后跳转到右子树
            else if (temp->right == curr) {
                cout << curr->data << " ";
                temp->right = NULL; // 拆除临时线索
                curr = curr->right;
            }
        }
    }
}

/**
 * @brief 后序递归释放二叉树内存的辅助函数
 */
void deleteAll(const Btree *const root) {
    if (root) {
        deleteAll(root->left);
        deleteAll(root->right);
        delete root;
    }
}

/**
 * @brief 主函数，演示构造 1 到 7 节点的二叉树并运行莫里斯中序遍历
 * @returns 0
 */
int main() {
    // 测试 morrisInorder 函数
    Btree *root = NULL;
    int i;
    for (i = 1; i <= 7; i++) insert(&root, i);
    cout << "Morris Inorder: ";
    morrisInorder(root);
    deleteAll(root);
    return 0;
}
