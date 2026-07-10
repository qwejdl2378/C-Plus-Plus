#include <iostream>

/**
 * @brief 单链表节点结构体定义
 */
struct node {
    int data;           ///< 节点存储的值
    struct node* next;  ///< 指向下一个链表节点的指针
};

/**
 * @brief 循环队列（基于循环单链表，队尾节点 rear->next 指回队首 front）
 */
class Queue {
    node* front = nullptr;  ///< 指向队首节点的指针
    node* rear = nullptr;   ///< 指向队尾节点的指针

    Queue(const Queue&) = delete;            ///< 禁用拷贝构造函数
    Queue& operator=(const Queue&) = delete; ///< 禁用赋值运算符

 public:
    Queue() = default;

    /**
     * @brief 析构函数，循环调用 dequeue() 释放所有节点的内存
     */
    ~Queue() {
        while (front) {
            dequeue();
        }
    }

 private:
    /**
     * @brief 辅助函数：当队列为空时创建第一个节点，并让 front 与 rear 都指向它
     * @param val 节点的值
     */
    void createNode(int val) {
        auto* nn = new node;
        nn->data = val;
        nn->next = nullptr;
        front = nn;
        rear = nn;
    }

 public:
    /**
     * @brief 元素入队
     * @param val 待插入的值
     */
    void enqueue(int val) {
        if (front == nullptr || rear == nullptr) {
            createNode(val); // 如果为空，初始化创建节点
        } else {
            node* nn = new node;
            nn->data = val;
            rear->next = nn;   ///< 旧队尾指向新节点
            nn->next = front;  ///< 新节点指向队首（构成循环链表关系）
            rear = nn;         ///< 更新队尾指针为新节点
        }
    }

    /**
     * @brief 元素出队
     */
    void dequeue() {
        if (front == nullptr) {
            return; // 队列为空，直接返回
        }
        const node* const n = front;
        if (front == rear) {
            // 队列中仅有 1 个元素，弹出后队列置空
            front = nullptr;
            rear = nullptr;
        } else {
            // 队首指针移向下一个元素，并更新队尾 rear->next 环形链表指向新的队首
            front = front->next;
            rear->next = front;
        }
        delete n; // 释放原队首节点内存
    }

    /**
     * @brief 遍历并打印循环队列中的所有有效元素
     */
    void traverse() {
        if (front == nullptr) {
            return;
        }
        const node* ptr = front;
        do {
            std::cout << ptr->data << ' ';
            ptr = ptr->next;
        } while (ptr != front); // 由于是循环单链表，当指针绕回 front 时结束遍历
        std::cout << '\n';
    }
};

/**
 * @brief 主函数
 * @returns 0
 */
int main(void) {
    Queue q;
    q.enqueue(10);
    q.enqueue(20);
    q.enqueue(30);
    q.enqueue(40);
    q.enqueue(50);
    q.enqueue(60);
    q.enqueue(70);
    q.traverse();
    q.dequeue();
    q.traverse();
    return 0;
}