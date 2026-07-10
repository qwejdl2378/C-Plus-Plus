#include <iostream>
using namespace std;

/**
 * @brief 单链表节点结构体定义
 */
struct node {
    int val;         ///< 节点存储的整数值
    node *next;      ///< 指向下一个链表节点的指针
};

node *front, *rear; ///< 全局指针：front 指向队头，rear 指向队尾

/**
 * @brief 元素入队
 * @param x 待插入的元素值
 */
void Enque(int x) {
    if (rear == NULL) {
        // 如果队列为空，创建第一个节点，并将 front 和 rear 都指向它
        node *n = new node;
        n->val = x;
        n->next = NULL;
        rear = n;
        front = n;
    } else {
        // 队列不为空，在 rear 节点后挂载新节点，并将 rear 移动指向新节点
        node *n = new node;
        n->val = x;
        n->next = NULL;
        rear->next = n;
        rear = n;
    }
}

/**
 * @brief 元素出队
 */
void Deque() {
    if (rear == NULL && front == NULL) {
        cout << "\nUnderflow"; // 下溢：队列为空无法出队
    } else {
        // 保存当前队头节点，将 front 移动指向下一个节点，最后释放原队头节点内存
        node *t = front;
        cout << "\n" << t->val << " deleted";
        front = front->next;
        delete t;
        // 如果出队后队列变为空，重置 rear 也指向 NULL
        if (front == NULL)
            rear = NULL;
    }
}

/**
 * @brief 打印队列中的所有元素
 */
void show() {
    node *t = front;
    while (t != NULL) {
        cout << t->val << "\t";
        t = t->next;
    }
}

/**
 * @brief 主函数，提供交互菜单
 * @returns 0
 */
int main() {
    int ch, x;
    do {
        cout << "\n1. Enque";
        cout << "\n2. Deque";
        cout << "\n3. Print"
             << "\n0. Exit";
        cout << "\nEnter Your Choice : ";
        cin >> ch;
        if (ch == 1) {
            cout << "\nInsert : ";
            cin >> x;
            Enque(x);
        } else if (ch == 2) {
            Deque();
        } else if (ch == 3) {
            show();
        }
    } while (ch != 0);

    return 0;
}
