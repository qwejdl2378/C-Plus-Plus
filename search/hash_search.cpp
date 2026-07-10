/**
 * \file
 * \brief Hash Search Algorithm (哈希查找算法实现 - 最佳时间复杂度可达 O(1))
 *
 * \copyright 2020 Arctic2333
 *
 * 在该算法中，我们使用“除留余数法”构建哈希函数，并采用“拉链法（链地址法）”解决冲突。
 * 即：当不同的键计算得到相同的哈希索引（同义词）时，将它们串联在同一个单链表上。
 *
 * @warning 
 * 该程序仅供教学和学习之用。由于大量使用 `malloc` 而未在程序结束前调用对应的 `free` 释放内存，
 * 存在内存泄漏缺陷。在实际项目中，推荐使用高级的 std::unordered_map 等标准库容器。
 */
#include <cstdlib>
#include <iostream>

#define MAX 6      ///< 待插入的测试数据总个数
#define HASHMAX 5  ///< 哈希表的最大槽位长度

int data[MAX] = {1, 10, 15, 5, 8, 7};  //!< 测试数据集

/**
 * @brief 单向链表节点结构体 (拉链冲突链表)
 */
typedef struct list {
    int key;            //!< 节点存储的键值
    struct list* next;  //!< 指向链表下一个节点的指针
} node,                 /**< 节点重命名 */
    *link;              ///< 指针重命名

node hashtab[HASHMAX];  ///< 存储拉链头节点的数组

/**
 * @brief 哈希函数：采用除留余数法
 * \param [in] key 键值
 * \returns 计算得到的哈希索引值 [0, HASHMAX-1]
 */
int h(int key) { return key % HASHMAX; }

/**
 * @brief 将一个键值插入哈希表中（采用头插法插入对应槽位的单链表）
 * \param [in] key 待插入的键值
 * \warning 此处动态分配的内存 `n` 在程序中从未被 `free`，造成了内存泄漏缺陷。
 */
void create_list(int key) {  // 构造哈希表
    link p, n;
    int index;
    n = (link)malloc(sizeof(node)); // 申请新节点内存
    n->key = key;
    n->next = NULL;
    
    index = h(key); // 计算哈希槽位索引
    p = hashtab[index].next;
    
    if (p != NULL) {
        // 如果槽位链表不为空，使用头插法连接新节点
        n->next = p;
        hashtab[index].next = n;
    } else {
        // 如果为空，新节点直接作为槽位链表首节点
        hashtab[index].next = n;
    }
}

/**
 * @brief 在哈希表中查找指定键值是否存在
 * @details
 * 1. 先计算 key 对应的哈希索引 index。
 * 2. 顺着 `hashtab[index].next` 链表开始向后线性遍历查找。
 * 3. `counter` 统计本次查询探测对比的次数。
 * 
 * @param key 待查找的键值
 * @param counter 查找次数输出指针
 * @return `1` 查找成功；`0` 查找失败
 */
int hash_search(int key, int* counter) {
    link pointer;
    int index;

    *counter = 0;
    index = h(key);
    pointer = hashtab[index].next;

    std::cout << "data[" << index << "]:";

    while (pointer != NULL) {
        (*counter)++; // 累加查找步数
        std::cout << "data[" << pointer->key << "]:";
        if (pointer->key == key)
            return 1; // 查找到目标，返回 1
        else
            pointer = pointer->next;
    }

    return 0; // 链表末尾，未找到，返回 0
}

/** 
 * @brief 主函数
 * @returns 0
 */
int main() {
    link p;
    int key = 0, index, i, counter;  // key 代表要查找的值
    index = 0;

    // 1. 逐步将 data 中的元素插入哈希表，建立冲突链表
    while (index < MAX) {
        create_list(data[index]);
        index++;
    }

    // 2. 遍历输出当前哈希表中的所有元素布局，展示拉链结构
    for (i = 0; i < HASHMAX; i++) {
        std::cout << "hashtab [" << i << "]\n";

        p = hashtab[i].next;

        while (p != NULL) {
            std::cout << "please int key:";
            if (p->key > 0)
                std::cout << "[" << p->key << "]";
            p = p->next;
        }
        std::cout << std::endl;
    }

    // 3. 执行查找逻辑，检索是否存在 10
    while (key != -1) {
        key = 10;
        if (hash_search(key, &counter))
            std::cout << "search time = " << counter << std::endl;
        else
            std::cout << "no found!\n";
        key = -1;  // 退出测试
    }

    // 注意：实际应用中应当在此循环释放 `hashtab` 中每个拉链的 `malloc` 内存。
    return 0;
}
