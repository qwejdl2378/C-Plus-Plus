/**
 * @file
 * @brief Given a linked list L[0,....,n] of n numbers, find the middle node (寻找链表的中位数/中间节点)
 *
 * @details 
 * 该实现使用经典的“快慢指针 (Floyd's tortoise and hare)”双指针策略。
 * 慢指针 (slowptr) 每次走一步，快指针 (fastptr) 每次走两步。
 * 当快指针到达链表末尾（即无法再前进两步）时，慢指针所指向的位置恰好就是链表的中间节点。
 * 
 * 空间复杂度 O(1)，时间复杂度 O(N)。
 *
 * @author [Benjamin Weiss](https://github.com/weiss-ben)
 * @see median_search.cpp
 */

#include <cassert>   /// 用于 assert 断言
#include <iostream>  /// 用于输入输出

/**
 * @brief 单链表节点结构体 (ListNode)
 */
struct ListNode {
    int val{0};               ///< 节点存储的值
    ListNode* next{nullptr};  ///< 指向下一个节点的指针
    ListNode() = default;     ///< 默认构造函数
    explicit ListNode(int x) : val(x) {}  ///< 构造函数
    ListNode(int x, ListNode* next) : val(x), next(next) {} ///< 带 next 指针的构造函数
};

/**
 * @namespace search
 * @brief 搜索算法命名空间
 */
namespace search {
/**
 * @namespace median_search2
 * @brief 链表中间节点查询算法相关命名空间
 */
namespace median_search2 {
/**
 * @brief 寻找并返回单链表的中间节点指针
 * @param head 链表头节点指针
 * @returns 中间节点的指针；若链表为空则返回 nullptr
 */
ListNode* middleNode(ListNode* head) {
    if (!head) {
        return nullptr;
    }

    // 初始化快慢指针都指向头节点
    ListNode* fastptr = nullptr;
    ListNode* slowptr = fastptr = head;

    // 快指针每次向前跳两步，慢指针每次跳一步
    while (fastptr->next && fastptr->next->next) {
        slowptr = slowptr->next;
        fastptr = fastptr->next->next;
    }

    // 根据链表的总长度是奇数还是偶数返回对应的中间节点：
    // 如果是偶数长度 (fastptr->next 不为空，例如 6 个节点)，返回 slowptr->next（即第 4 个节点）
    // 如果是奇数长度 (fastptr->next 为空，例如 5 个节点)，直接返回 slowptr（即第 3 个节点）
    return (fastptr->next) ? slowptr->next : slowptr;
}

/**
 * @brief 递归清理释放整个链表的内存，防止内存泄漏
 * @param head 链表头指针
 */
void deleteAll(const ListNode* const head) {
    if (head) {
        deleteAll(head->next); // 先释放后面的节点
        delete head;           // 再释放当前节点
    }
}
}  // namespace median_search2
}  // namespace search

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1：奇数长度链表 {1, 2, 3, 4, 5}
    auto* head1 = new ListNode;
    head1->val = 1;

    ListNode* temp = head1;
    for (int i = 2; i < 6; ++i) {
        auto* temp1 = new ListNode;
        temp1->val = i;
        temp->next = temp1;
        temp = temp1;
    }
    temp->next = nullptr;

    const ListNode* const median = search::median_search2::middleNode(head1);
    assert(3 == median->val);  // 中间节点的值应为 3
    search::median_search2::deleteAll(head1);
    std::cout << "test case:1 passed\n";

    // 测试 2：偶数长度链表 {1, 2, 3, 4, 5, 6}
    auto* head2 = new ListNode;
    head2->val = 1;

    ListNode* temp2 = head2;
    for (int i = 2; i < 7; ++i) {
        auto* temp3 = new ListNode;
        temp3->val = i;
        temp2->next = temp3;
        temp2 = temp3;
    }
    temp2->next = nullptr;

    const ListNode* const median1 = search::median_search2::middleNode(head2);
    assert(4 == median1->val);  // 偶数长度时，中间偏右节点值应为 4
    search::median_search2::deleteAll(head2);
    std::cout << "test case:2 passed\n";

    std::cout << "--All tests passed--\n";
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行自测
    return 0;
}
