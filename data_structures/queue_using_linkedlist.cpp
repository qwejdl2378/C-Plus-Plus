/*
    Write a program to implement Queue using linkedlist. (使用链表实现队列)
*/
#include <iostream>

/**
 * @brief 链表节点结构体定义
 */
struct linkedlist {
    int data;            ///< 节点数据
    linkedlist *next;    ///< 指向下一个节点的指针
};

/**
 * @brief 队列实现类（类名虽命名为 stack_linkedList，但其实际接口和底层表现均是队列）
 */
class stack_linkedList {
 public:
    linkedlist *front;  ///< 指向队首节点的指针
    linkedlist *rear;   ///< 指向队尾节点的指针

    /**
     * @brief 构造函数，将队首和队尾初始化为 NULL
     */
    stack_linkedList() { front = rear = NULL; }

    void enqueue(int);  ///< 入队操作
    int dequeue();      ///< 出队操作
    void display();     ///< 打印队列元素
};

/**
 * @brief 元素入队
 * @param ele 待加入的整数元素
 */
void stack_linkedList::enqueue(int ele) {
    linkedlist *temp = new linkedlist();
    temp->data = ele;
    temp->next = NULL;

    if (front == NULL) {
        // 如果队列为空，则 front 和 rear 都指向该新节点
        front = rear = temp;
    } else {
        // 挂载到 rear 之后，更新 rear 指针
        rear->next = temp;
        rear = temp;
    }
}

/**
 * @brief 元素出队
 * @returns 弹出的元素值（若为空则输出错误提示，返回无定义/历史遗留值）
 */
int stack_linkedList::dequeue() {
    linkedlist *temp;
    int ele = 0;
    if (front == NULL) {
        std::cout << "\nQueue (Stack class) is empty";
    } else {
        temp = front;
        ele = temp->data;
        if (front == rear) {
            // 如果当前队列仅有 1 个元素，出队后更新 rear 为 NULL
            rear = rear->next;
        }
        front = front->next; // 队首指针右移
        delete (temp);       // 释放内存
    }
    return ele;
}

/**
 * @brief 打印队列当前全部元素
 */
void stack_linkedList::display() {
    if (front == NULL) {
        std::cout << "\nQueue is empty";
    } else {
        linkedlist *temp;
        temp = front;
        while (temp != NULL) {
            std::cout << temp->data << " ";
            temp = temp->next;
        }
    }
}

/**
 * @brief 主函数，提供交互式的队列操作菜单
 * @returns 0
 */
int main() {
    int op, data;
    stack_linkedList ob;
    std::cout << "\n1. enqueue(Insertion) ";
    std::cout << "\n2. dequeue(Deletion)";
    std::cout << "\n3. Display";
    std::cout << "\n4. Exit";

    while (1) {
        std::cout << "\nEnter your choice ";
        std::cin >> op;
        if (op == 1) {
            std::cout << "Enter data ";
            std::cin >> data;
            ob.enqueue(data);
        } else if (op == 2) {
            data = ob.dequeue();
        } else if (op == 3) {
            ob.display();
        } else if (op == 4) {
            exit(0);
        } else {
            std::cout << "\nWrong choice ";
        }
    }
    return 0;
}
