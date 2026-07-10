/*
 * Simple data structure CLL (Circular Linear Linked List) (循环单链表声明)
 * */
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <iostream>

#ifndef CLL_H
#define CLL_H

/**
 * @brief 循环单链表节点结构体
 */
struct node {
    int data;     ///< 节点数值
    node* next;   ///< 指向下一个节点的指针
};

/**
 * @brief 循环单链表类定义
 */
class cll {
 public:
    cll(); /* 默认构造函数 */
    ~cll();/* 析构函数 */
    
    void display(); /* 显示循环链表中的所有节点 */

    /******************************************************
     * 常用链表操作方法
     *******************************************************/
    void insert_front(int new_data);  /* 在队首（head）插入一个新值 */
    void insert_tail(int new_data);   /* 在队尾插入一个新值 */
    int get_size();                   /* 获取链表的节点总数 */
    bool find_item(int item_to_find); /* 在链表中查找是否存在指定元素 */

    /******************************************************
     * 运算符重载方法（支持简单的迭代行为）
     *******************************************************/
    int operator*(); /* 解引用运算符：返回 head 节点中的数据 */
    /* 前置递增运算符重载，使头指针 head 沿着环形链表向前移动一个位置 */
    void operator++();

 protected:
    node* head; ///< 指向循环链表头节点（即当前访问的哨兵起始节点）的指针
    int total;  ///< 记录链表中的元素总节点数
};
#endif
