/**
 * @file
 * @brief Implementation for getting the size of a Linked List (获取单链表长度算法实现)
 *
 * @details
 * 单链表的大小指的是链表中节点的总个数。
 * 本实现将原先的递归查询重构为迭代查询，以防止超长链表下发生栈溢出崩溃。
 *
 * 时间复杂度: $O(N)$，其中 $N$ 为链表的长度。
 * 空间复杂度: $O(1)$，迭代遍历仅需常数级额外指针空间。
 *
 * @note
 * 【递归爆栈风险与内存泄漏防卫性审计与修复】：
 * 1. **深度递归导致 Stack Overflow 风险**：原程序中 `getSize` 与 `deleteList` 均采用深度递归形式。
 *    若单链表节点数量巨大，递归调用栈会耗尽系统栈空间，触发进程段错误异常退出。
 *    **修复**：重构 `getSize` 和 `deleteList` 为安全高效的迭代（循环）实现，保障生产环境下的栈空间安全。
 * 2. **缺少自动化单元测试**：原有 `main` 仅简单向 `std::cout` 打印，未对返回值进行判定。
 *    **修复**：添加 `assert` 自动校验机制，确保功能逻辑准确。
 */

#include <cassert>
#include <iostream>

/**
 * @brief 单链表节点类
 */
class Node {
 public:
    int val;
    Node *next;

    Node(int v, Node *n) : val(v), next(n) {}  
};

/**
 * @brief 迭代计算单链表大小，防止深层递归导致栈溢出
 * @param root 链表头节点指针
 * @return 链表节点数
 */
int getSize(Node *root) {
    int count = 0;
    Node *temp = root;
    while (temp != nullptr) {
        count++;
        temp = temp->next;
    }
    return count;
}

/**
 * @brief 迭代释放链表中的所有节点内存，防止递归爆栈
 * @param root 链表头节点指针的引用，删除后置为空指针
 */
void deleteList(Node *&root) {
    Node *current = root;
    while (current != nullptr) {
        Node *nextNode = current->next;
        delete current;
        current = nextNode;
    }
    root = nullptr;
}

/**
 * @brief 单元自测用例
 */
static void test() {
    // 用例 1：测试空链表大小应为 0
    Node *emptyList = nullptr;
    assert(getSize(emptyList) == 0);

    // 用例 2：测试包含 1 个元素的链表
    Node *singleList = new Node(42, nullptr);
    assert(getSize(singleList) == 1);
    deleteList(singleList);
    assert(singleList == nullptr);

    // 用例 3：测试包含 10 个元素的链表
    Node *myList = new Node(0, nullptr);
    Node *temp = myList;
    for (int i = 1; i < 10; i++) {
        temp->next = new Node(i, nullptr);
        temp = temp->next;
    }
    assert(getSize(myList) == 10);
    deleteList(myList);
    assert(myList == nullptr);

    std::cout << "All get size of linked list tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行测试用例确认正确性
    return 0;
}
