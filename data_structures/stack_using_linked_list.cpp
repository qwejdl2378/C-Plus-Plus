#include <iostream>

/**
 * @brief 链表节点结构体定义
 */
struct node {
    int val;     ///< 节点存储的值
    node *next;  ///< 指向下一个节点的指针
};

node *top_var;   ///< 全局指针：指向当前的栈顶节点

/**
 * @brief 压入新元素到栈顶 (Push)
 * @param x 待插入的元素值
 */
void push(int x) {
    node *n = new node;
    n->val = x;
    n->next = top_var; ///< 新节点指向旧栈顶
    top_var = n;       ///< 更新栈顶指针为新节点
}

/**
 * @brief 弹出栈顶元素 (Pop)
 */
void pop() {
    if (top_var == nullptr) {
        std::cout << "\nUnderflow"; // 栈空下溢
    } else {
        node *t = top_var;
        std::cout << "\n" << t->val << " deleted";
        top_var = top_var->next; // 栈顶移向下一个节点
        delete t;                // 释放被弹出的节点内存
    }
}

/**
 * @brief 遍历并打印栈中当前的所有元素
 */
void show() {
    node *t = top_var;
    while (t != nullptr) {
        std::cout << t->val << "\n";
        t = t->next;
    }
}

/**
 * @brief 主函数，提供交互式的栈操作菜单
 * @returns 0
 */
int main() {
    int ch = 0, x = 0;
    do {
        std::cout << "\n0. Exit or Ctrl+C";
        std::cout << "\n1. Push";
        std::cout << "\n2. Pop";
        std::cout << "\n3. Print";
        std::cout << "\nEnter Your Choice: ";
        std::cin >> ch;
        switch (ch) {
            case 0:
                break;
            case 1:
                std::cout << "\nInsert : ";
                std::cin >> x;
                push(x);
                break;
            case 2:
                pop();
                break;
            case 3:
                show();
                break;
            default:
                std::cout << "Invalid option!\n";
                break;
        }
    } while (ch != 0);

    return 0;
}
