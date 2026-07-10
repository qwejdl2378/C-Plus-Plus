#include <cstdio>
#include <cstdlib>
#include <iostream>

/**
 * @brief 双向链表节点结构体定义
 */
struct node {
    int val;       ///< 节点数据值
    node *prev;    ///< 指向前驱节点的指针
    node *next;    ///< 指向后继节点的指针
} * start;         ///< 全局头节点指针

/**
 * @brief 双向链表类实现
 */
class double_linked_list {
 public:
    /**
     * @brief 构造函数，将头节点初始化为 NULL
     */
    double_linked_list() { start = NULL; }

    void insert(int x);       ///< 向双向链表尾部插入元素
    void remove(int x);       ///< 从双向链表中删除指定元素
    void search(int x);       ///< 搜索指定元素
    void show();              ///< 正向打印双向链表
    void reverseShow();       ///< 反向逆序打印双向链表
};

/**
 * @brief 在链表尾部插入元素 x
 * @param x 待插入的值
 */
void double_linked_list::insert(int x) {
    node *t = start;
    if (start != NULL) {
        // 遍历找到链表的最后一个元素
        while (t->next != NULL) {
            t = t->next;
        }
        node *n = new node;
        t->next = n;   ///< 原尾部节点的 next 指向新节点
        n->prev = t;   ///< 新节点的 prev 指向原尾部节点
        n->val = x;
        n->next = NULL;
    } else {
        // 链表为空，创建第一个节点作为头节点
        node *n = new node;
        n->val = x;
        n->prev = NULL;
        n->next = NULL;
        start = n;
    }
}

/**
 * @brief 从链表中删除指定元素 x
 * @param x 待删除的值
 */
void double_linked_list::remove(int x) {
    node *t = start;
    // 遍历寻找值为 x 的节点
    while (t != NULL && t->val != x) {
        t = t->next;
    }
    if (t == NULL) {
        return; // 未找到目标值，直接返回
    }
    
    if (t->prev == NULL) {
        // 情况 1：目标节点是头节点 (t == start)
        if (t->next == NULL) {
            start = NULL; // 链表中只有这唯一的节点
        } else {
            start = t->next; // 头指针移向下一个节点
            start->prev = NULL;
        }
    } else if (t->next == NULL) {
        // 情况 2：目标节点是尾节点
        t->prev->next = NULL;
    } else {
        // 情况 3：目标节点在链表中间
        t->prev->next = t->next;
        t->next->prev = t->prev;
    }
    delete t; // 释放内存
}

/**
 * @brief 搜索节点值 x 是否在双向链表中
 * @param x 待搜索的值
 */
void double_linked_list::search(int x) {
    node *t = start;
    int found = 0;
    while (t != NULL) {
        if (t->val == x) {
            std::cout << "\nFound";
            found = 1;
            break;
        }
        t = t->next;
    }
    if (found == 0) {
        std::cout << "\nNot Found";
    }
}

/**
 * @brief 从头到尾正向打印双向链表
 */
void double_linked_list::show() {
    node *t = start;
    while (t != NULL) {
        std::cout << t->val << "\t";
        t = t->next;
    }
}

/**
 * @brief 从尾到头反向逆序打印双向链表
 */
void double_linked_list::reverseShow() {
    node *t = start;
    // 首先遍历找到链表的最后一个节点
    while (t != NULL && t->next != NULL) {
        t = t->next;
    }
    // 从尾节点利用 prev 指针一路回溯打印到头节点
    while (t != NULL) {
        std::cout << t->val << "\t";
        t = t->prev;
    }
}

/**
 * @brief 主函数，提供交互式的双向链表菜单
 * @returns 0
 */
int main() {
    int choice, x;
    double_linked_list ob;
    do {
        std::cout << "\n1. Insert";
        std::cout << "\n2. Delete";
        std::cout << "\n3. Search";
        std::cout << "\n4. Forward print";
        std::cout << "\n5. Reverse print"
                  << "\n0. Exit";
        std::cout << "\n\nEnter you choice : ";
        std::cin >> choice;
        switch (choice) {
        case 1:
            std::cout << "\nEnter the element to be inserted : ";
            std::cin >> x;
            ob.insert(x);
            break;
        case 2:
            std::cout << "\nEnter the element to be removed : ";
            std::cin >> x;
            ob.remove(x);
            break;
        case 3:
            std::cout << "\nEnter the element to be searched : ";
            std::cin >> x;
            ob.search(x);
            break;
        case 4:
            ob.show();
            break;
        case 5:
            ob.reverseShow();
            break;
        }
    } while (choice != 0);
    return 0;
}
