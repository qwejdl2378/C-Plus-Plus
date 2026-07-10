/*
    A simple class for Circular Linear Linked List (循环单链表实现)
*/
#include "cll.h"
using namespace std;

/* 构造函数 */
cll::cll() {
    head = NULL;
    total = 0;
}

cll::~cll() { 
    // 析构函数，此处保留原样
}

/* 遍历打印整个循环链表，并输出元素总数 */
void cll::display() {
    if (head == NULL)
        cout << "List is empty !" << endl;
    else {
        cout << "CLL list: ";
        node *current = head;
        // 依据记录的总个数 total 进行遍历，防止死循环
        for (int i = 0; i < total; i++) {
            cout << current->data << " -> ";
            current = current->next;
        }
        cout << head->data << endl; // 打印回绕至头节点的信息
        cout << "Total element: " << total << endl;
    }
}

/* 在链表头部（head）插入新元素 */
void cll::insert_front(int new_data) {
    node *newNode;
    newNode = new node;
    newNode->data = new_data;
    newNode->next = NULL;
    if (head == NULL) {
        head = newNode;
        head->next = head; // 只有一个节点时，next 指向自身
    } else {
        node *current = head;
        // 遍历寻找当前循环单链表的尾节点（其 next 指向 head）
        while (current->next != head) {
            current = current->next;
        }
        newNode->next = head;      // 新节点 next 指向原头节点
        current->next = newNode;   // 原尾节点 next 指向新节点
        head = newNode;            // 更新头指针指向新节点
    }
    total++;
}

/* 在链表尾部插入新元素 */
void cll::insert_tail(int new_data) {
    node *newNode;
    newNode = new node;
    newNode->data = new_data;
    newNode->next = NULL;
    if (head == NULL) {
        head = newNode;
        head->next = head;
    } else {
        node *current = head;
        // 遍历寻找当前尾节点
        while (current->next != head) {
            current = current->next;
        }
        current->next = newNode;   // 原尾节点指向新节点
        newNode->next = head;      // 新尾节点指向头节点，保持循环特性
    }
    total++;
}

/* 获取链表的节点总数 */
int cll::get_size() { return total; }

/**
 * @brief 在循环链表中查找是否存在目标值
 * @note 原代码中采用 `current->next != head` 循环判断，这会导致无法检查尾节点自身的数据值。
 * 读者在学习使用时需注意此处的边界遗漏问题。
 */
bool cll::find_item(int item_to_find) {
    if (head == NULL) {
        cout << "List is empty !" << endl;
        return false;
    } else {
        node *current = head;
        while (current->next != head) {
            if (current->data == item_to_find)
                return true;
            current = current->next;
        }
        return false;
    }
}

/* 运算符重载：解引用操作，返回 head 节点存储的数据 */
int cll::operator*() { return head->data; }

/**
 * @brief 重载前置自增运算符 ++
 * @details 注意：该实现并非单纯的迭代器移动，而是起到了“弹出（删除）头节点”的作用。
 * 1. 寻找尾节点。
 * 2. 将尾节点的 next 指向原 head 的下一个节点。
 * 3. 头指针指向下一个节点，并递减总数 total。（此实现未 delete 释放被弹出节点的内存，存在内存泄漏，读者需注意）。
 */
void cll::operator++() {
    if (head == NULL) {
        cout << "List is empty !" << endl;
    } else {
        node *current = head;
        while (current->next != head) {
            current = current->next;
        }
        current->next = head->next; // 尾节点重定向
        head = head->next;           // 头指针移至下一个节点
    }
    total--;
}
