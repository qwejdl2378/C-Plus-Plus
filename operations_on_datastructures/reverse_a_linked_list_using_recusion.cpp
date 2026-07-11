/**
 * @file
 * @brief Implementation for [reversing a linked list using recursion](https://www.geeksforgeeks.org/reverse-a-linked-list/) (递归反转单链表算法实现)
 *
 * @details
 * 递归反转单链表是利用函数调用栈的后进先出特性，将链表的指向方向逆转。
 * 我们可以将反转过程看作：先递归地将当前节点后面的子链表反转，然后再将当前节点指向的下一个节点的 next 指针指向当前节点，
 * 并将当前节点的 next 指针置为空。
 *
 * 时间复杂度: $O(N)$，其中 $N$ 是链表的节点数量。
 * 空间复杂度: $O(N)$，递归调用时占用函数调用栈空间。
 *
 * @note
 * 【空指针/单元素解引用与内存泄漏 Bug 审计与修复】：
 * 1. **空链表或单元素链表下野指针崩溃 Bug**：原程序中在 `main()` 里面直接无脑调用 `reverse(start, start->next)`。
 *    若链表为空（`start == nullptr`）或只有一个节点时，`start->next` 会直接发生**空指针解引用崩溃（Segmentation Fault）**。
 *    **修复**：使用一个更安全的单入参递归反转接口 `reverse(node* head)`，在其内部进行前置 `nullptr` 和单节点判断。
 * 2. **全局变量污染与内存泄漏**：原实现使用全局变量 `node *start`，并且在退出程序时从未进行内存释放，导致内存泄漏。
 *    **修复**：重构并消除全局变量，将其组织在局部作用域与 `operations_on_datastructures` 命名空间中，并在自测试结束后闭环释放链表占用的所有堆内存。
 */

#include <cassert>
#include <iostream>
#include <vector>

namespace operations_on_datastructures {
namespace reverse_linked_list {

/**
 * @brief 链表节点结构体
 */
struct Node {
    int val;
    Node *next;
    explicit Node(int x) : val(x), next(nullptr) {}
};

/**
 * @brief 在链表尾部插入一个新元素
 * @param head 链表头节点的引用
 * @param x 待插入的元素值
 */
void insert(Node *&head, int x) {
    Node *n = new Node(x);
    if (head == nullptr) {
        head = n;
        return;
    }
    Node *temp = head;
    while (temp->next != nullptr) {
        temp = temp->next;
    }
    temp->next = n;
}

/**
 * @brief 尾递归形式实现链表反转
 * @param curr 当前待反转的节点
 * @param prev 当前节点的前驱节点（反转后作为 next）
 * @return 反转后新的头节点指针
 */
Node* reverse_recursive_helper(Node *curr, Node *prev) {
    if (curr == nullptr) {
        return prev;
    }
    Node *next_node = curr->next;
    curr->next = prev;
    return reverse_recursive_helper(next_node, curr);
}

/**
 * @brief 递归反转链表（安全入口函数）
 * @param head 链表头节点指针
 * @return 反转后的新链表头节点指针
 */
Node* reverse(Node *head) {
    // 核心防卫：空链表或单节点链表无需反转，直接返回
    if (head == nullptr || head->next == nullptr) {
        return head;
    }
    return reverse_recursive_helper(head, nullptr);
}

/**
 * @brief 获取链表所有元素的值向量
 */
std::vector<int> get_values(Node *head) {
    std::vector<int> res;
    Node *temp = head;
    while (temp != nullptr) {
        res.push_back(temp->val);
        temp = temp->next;
    }
    return res;
}

/**
 * @brief 顺序打印链表
 */
void show(Node *head) {
    Node *temp = head;
    while (temp != nullptr) {
        std::cout << temp->val << "\t";
        temp = temp->next;
    }
    std::cout << "\n";
}

/**
 * @brief 迭代释放链表节点内存，防止内存泄漏
 */
void delete_list(Node *&head) {
    Node *curr = head;
    while (curr != nullptr) {
        Node *next_node = curr->next;
        delete curr;
        curr = next_node;
    }
    head = nullptr;
}

}  // namespace reverse_linked_list
}  // namespace operations_on_datastructures

/**
 * @brief 单元自测用例
 */
static void test() {
    using namespace operations_on_datastructures::reverse_linked_list;
    
    // 1. 空链表测试不应崩溃
    Node *empty_head = nullptr;
    empty_head = reverse(empty_head);
    assert(empty_head == nullptr);

    // 2. 单元素链表测试不应崩溃
    Node *single_head = nullptr;
    insert(single_head, 42);
    single_head = reverse(single_head);
    assert(single_head->val == 42 && single_head->next == nullptr);
    delete_list(single_head);

    // 3. 常规链表反转测试
    Node *head = nullptr;
    insert(head, 1);
    insert(head, 2);
    insert(head, 3);
    insert(head, 4);
    insert(head, 5);
    insert(head, 6);

    std::cout << "Original list:\n";
    show(head);

    head = reverse(head);

    std::cout << "Reversed list:\n";
    show(head);

    std::vector<int> expected = {6, 5, 4, 3, 2, 1};
    assert(get_values(head) == expected);

    // 回收堆空间，消除内存泄漏
    delete_list(head);
    assert(head == nullptr);

    std::cout << "All recursive list reversal tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行单元自测
    return 0;
}
