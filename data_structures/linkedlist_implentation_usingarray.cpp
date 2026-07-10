/**
 * \file
 * \brief Linked list implementation using Arrays (基于静态数组的单链表实现)
 *
 * 区别于传统的指针式链表，基于数组实现的链表：
 * 1. 使用索引 -1 来表示 NULL 空指针。
 * 2. 具有容量限制（本例中最大容量为 100 个节点）。
 * 3. 具有自主的空闲节点池分配管理（通过维护一个 free 链表 `avail` 极大地实现了节点重用）。
 */

#include <iostream>

/**
 * @brief 链表节点结构体（基于数组索引）
 */
struct Node {
    int data; ///< 节点数据值
    int next; ///< 下一个节点的数组索引位置（-1 代表 NULL）
};

Node AvailArray[100];  ///< 静态数组，充当链表节点的物理存储池

int head = -1;  ///< 头节点的数组索引位置（初始为 -1）
int avail = 0;  ///< 指向当前空闲节点链表的队头索引

/**
 * @brief 初始化静态节点存储池。将所有 100 个节点通过 next 指针依次串连，构建最初的空闲节点链表
 */
void initialise_list() {
    for (int i = 0; i <= 98; i++) {
        AvailArray[i].next = i + 1;
    }
    AvailArray[99].next = -1;  // 最后一个节点的 next 设为 -1，代表空闲节点链表结束
}

/**
 * @brief 从空闲节点链表中分配获取一个可用的新节点位置
 * @return 分配给新节点的数组索引
 */
int getnode() {
    int NodeIndexToBeReturned = avail;
    avail = AvailArray[avail].next; // 将空闲队列移至下一个空闲位置
    return NodeIndexToBeReturned;
}

/**
 * @brief 释放一个节点，将其重新挂载回空闲节点链表的头部，以便后续重用该位置
 * @param nodeToBeDeleted 待释放节点的数组索引
 */
void freeNode(int nodeToBeDeleted) {
    AvailArray[nodeToBeDeleted].next = avail;
    avail = nodeToBeDeleted;
}

/**
 * @brief 头插法：向链表最前面插入一个新元素
 * @param data 待插入的数据值
 */
void insertAtTheBeginning(int data) {
    int newNode = getnode(); // 申请一个空闲节点索引
    AvailArray[newNode].data = data;
    AvailArray[newNode].next = head; // 新节点指向原头节点
    head = newNode;                 // 更新头指针为新节点索引
}

/**
 * @brief 尾插法：向链表末尾追加插入一个新元素
 * @param data 待插入的数据值
 */
void insertAtTheEnd(int data) {
    int newNode = getnode(); // 申请一个空闲节点索引
    int temp = head;
    
    if (temp == -1) {
        // 如果链表为空，直接设为头节点
        AvailArray[newNode].data = data;
        AvailArray[newNode].next = -1;
        head = newNode;
        return;
    }

    // 遍历查找到链表的尾部节点（其 next 为 -1）
    while (AvailArray[temp].next != -1) {
        temp = AvailArray[temp].next;
    }
    
    // 挂载新节点
    AvailArray[newNode].data = data;
    AvailArray[newNode].next = -1;
    AvailArray[temp].next = newNode;
}

/**
 * @brief 正向遍历并显示链表中的所有节点数据
 */
void display() {
    int temp = head;
    while (temp != -1) {
        std::cout << AvailArray[temp].data << "->";
        temp = AvailArray[temp].next;
    }
    std::cout << "-1" << std::endl;
}

/**
 * @brief 主函数，提供交互式的插入与展示菜单
 * @returns 0
 */
int main() {
    initialise_list(); // 初始化静态节点池
    int x, y, z;
    for (;;) {
        std::cout << "1. Insert At The Beginning" << std::endl;
        std::cout << "2. Insert At The End" << std::endl;
        std::cout << "3. Display" << std::endl;
        std::cout << "4. Exit" << std::endl;
        std::cout << "Enter Your choice" << std::endl;
        std::cin >> z;
        switch (z) {
        case 1:
            std::cout << "Enter the number you want to enter" << std::endl;
            std::cin >> x;
            insertAtTheBeginning(x);
            break;
        case 2:
            std::cout << "Enter the number you want to enter" << std::endl;
            std::cin >> y;
            insertAtTheEnd(y);
            break;
        case 3:
            std::cout
                << "The linked list contains the following element in order"
                << std::endl;
            display();
            break;
        case 4:
            return 0;
        default:
            std::cout << "The entered choice is not correct" << std::endl;
        }
    }
    return 0;
}
