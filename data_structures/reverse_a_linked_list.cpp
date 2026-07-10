/**
 * @file
 * @brief Implementation of [Reversing a single linked list](https://simple.wikipedia.org/wiki/Linked_list) (单链表反转算法实现)
 * @details
 * 链表是一种用于存储有序元素序列的数据结构。
 * 本程序实现了一个完整的单链表，包括元素的插入、清空、深拷贝、以及核心的反转（reverseList）功能。
 * 
 * ### 反转算法原理
 * 使用三个指针来完成原地反转：`curr` (当前节点)，`prev` (前驱节点) 和 `next_node` (后继临时节点)。
 * 1. 遍历过程中，先保存下一个节点 `next_node = curr->next` 防止链表断开。
 * 2. 核心反转：让当前节点指向前驱节点 `curr->next = prev`。
 * 3. 移动指针：前驱指针移到当前 `prev = curr`，当前指针移到下一个 `curr = next_node`。
 * 4. 循环结束时，`curr` 变为 nullptr，整个链表新的头节点就是 `prev`。
 */

#include <cassert>   /// 用于 assert 断言
#include <iostream>  /// 用于输入输出
#include <new>       /// 用于动态内存管理 (bad_alloc 异常捕获)

/**
 * @namespace data_structures
 * @brief 数据结构算法命名空间
 */
namespace data_structures {
/**
 * @namespace linked_list
 * @brief 单链表算法相关命名空间
 */
namespace linked_list {
/**
 * @brief 链表节点类 (Node)
 */
class Node {
 public:
    int32_t val;  ///< 节点存储的值
    Node* next;   ///< 指向下一个节点的指针
};

/**
 * @brief 递归深度拷贝所有节点
 * @param node 待拷贝链表的头节点指针
 * @return 拷贝生成的新链表头节点指针；若为空则返回 nullptr
 */
Node* copy_all_nodes(const Node* const node) {
    if (node) {
        Node* res = new Node();
        res->val = node->val;
        res->next = copy_all_nodes(node->next); // 递归拷贝后继节点
        return res;
    }
    return nullptr;
}

/**
 * @brief 链表管理类 (list)
 */
class list {
 private:
    Node* head = nullptr;  ///< 实际首个节点的指针，若为空代表链表为空
    void delete_all_nodes();
    void copy_all_nodes_from_list(const list& other);

 public:
    bool isEmpty() const;
    void insert(int32_t new_elem);
    void reverseList();
    void display() const;
    int32_t top() const;
    int32_t last() const;
    int32_t traverse(int32_t index) const;
    ~list();
    list() = default;
    list(const list& other);
    list& operator=(const list& other);
};

/**
 * @brief 检查链表是否为空
 * @return `true` 代表为空；`false` 否则
 */
bool list::isEmpty() const { return head == nullptr; }

/**
 * @brief 在链表尾部插入一个新元素
 * @param n 待插入的元素值
 */
void list::insert(int32_t n) {
    try {
        Node* new_node = new Node();
        Node* temp = nullptr;
        new_node->val = n;
        new_node->next = nullptr;
        if (isEmpty()) {
            head = new_node;
        } else {
            temp = head;
            // 遍历找到当前的尾部节点
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = new_node;
        }
    } catch (std::bad_alloc& exception) {
        std::cerr << "bad_alloc detected: " << exception.what() << "\n";
    }
}

/**
 * @brief 原地反转链表（核心函数）
 * @details 采用三指针法，逐个节点翻转其 next 指向。
 */
void list::reverseList() {
    Node* curr = head;        ///< 当前正在处理的节点
    Node* prev = nullptr;     ///< 当前节点的前驱节点
    Node* next_node = nullptr;///< 暂存当前节点的下一个节点，以防断链

    while (curr != nullptr) {
        next_node = curr->next;  // 1. 暂存下一个节点
        curr->next = prev;       // 2. 将当前节点指向前驱节点（反转）
        prev = curr;             // 3. prev 指针向后移动一位
        curr = next_node;        // 4. curr 指针向后移动一位
    }
    head = prev;                 // 遍历结束，更新头节点为最后一个非空节点 prev
}

/**
 * @brief 获取链表的第一个元素值
 * @return 头节点的值
 * @throws std::logic_error 如果链表为空
 */
int32_t list::top() const {
    if (!isEmpty()) {
        return head->val;
    } else {
        throw std::logic_error("List is empty");
    }
}

/**
 * @brief 获取链表的最后一个元素值
 * @return 尾节点的值
 * @throws std::logic_error 如果链表为空
 */
int32_t list::last() const {
    if (!isEmpty()) {
        Node* t = head;
        while (t->next != nullptr) {
            t = t->next;
        }
        return t->val;
    } else {
        throw std::logic_error("List is empty");
    }
}

/**
 * @brief 获取链表中指定索引位置处的节点值
 * @param index 0 起始的索引
 * @return 目标索引的值
 */
int32_t list::traverse(int32_t index) const {
    Node* current = head;
    int count = 0;
    while (current != nullptr) {
        if (count == index) {
            return (current->val);
        }
        count++;
        current = current->next;
    }
    // 越界请求，异常退出
    exit(1);
}

/**
 * @brief 清空释放链表所有节点的内存空间
 */
void list::delete_all_nodes() {
    while (head != nullptr) {
        const auto tmp_node = head->next;
        delete head;
        head = tmp_node;
    }
}

list::~list() { delete_all_nodes(); }

/**
 * @brief 辅助深拷贝函数
 */
void list::copy_all_nodes_from_list(const list& other) {
    assert(isEmpty());
    head = copy_all_nodes(other.head);
}

/**
 * @brief 拷贝构造函数（深拷贝）
 */
list::list(const list& other) { copy_all_nodes_from_list(other); }

/**
 * @brief 赋值运算符重载（防自赋值深拷贝）
 */
list& list::operator=(const list& other) {
    if (this == &other) {
        return *this;
    }
    delete_all_nodes(); // 释放原有的所有元素

    copy_all_nodes_from_list(other);
    return *this;
}

}  // namespace linked_list
}  // namespace data_structures

/**
 * @brief 单元自测用例集 1
 */
static void test() {
    data_structures::linked_list::list L;
    L.insert(11);
    L.insert(12);
    L.insert(15);
    L.insert(10);
    L.insert(-12);
    L.insert(-20);
    L.insert(18);
    assert(L.top() == 11);
    assert(L.last() == 18);
    
    L.reverseList(); // 反转链表
    
    // 验证反转后元素位置
    assert(L.top() == 18);
    assert(L.traverse(1) == -20);
    assert(L.traverse(2) == -12);
    assert(L.traverse(3) == 10);
    assert(L.traverse(4) == 15);
    assert(L.traverse(5) == 12);
    assert(L.last() == 11);
    std::cout << "All tests have successfully passed!" << std::endl;
}

/**
 * @brief 测试拷贝构造函数是否能够实现完全深拷贝
 */
void test_copy_constructor() {
    data_structures::linked_list::list L;
    L.insert(10);
    L.insert(20);
    L.insert(30);
    data_structures::linked_list::list otherList(L);
    otherList.insert(40);

    L.insert(400);

    assert(L.top() == 10);
    assert(otherList.top() == 10);
    assert(L.traverse(1) == 20);
    assert(otherList.traverse(1) == 20);

    assert(L.traverse(2) == 30);
    assert(otherList.traverse(2) == 30);

    assert(L.last() == 400);
    assert(otherList.last() == 40);
}

/**
 * @brief 测试赋值运算符重载深拷贝
 */
void test_assignment_operator() {
    data_structures::linked_list::list L;
    data_structures::linked_list::list otherList;
    L.insert(10);
    L.insert(20);
    L.insert(30);
    otherList = L;

    otherList.insert(40);
    L.insert(400);

    assert(L.top() == 10);
    assert(otherList.top() == 10);
    assert(L.traverse(1) == 20);
    assert(otherList.traverse(1) == 20);

    assert(L.traverse(2) == 30);
    assert(otherList.traverse(2) == 30);

    assert(L.last() == 400);
    assert(otherList.last() == 40);
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行单元测试集 1
    test_copy_constructor();
    test_assignment_operator();
    return 0;
}
