/**
 * @file
 * @brief Implementation for a [Circular Linked List](https://www.geeksforgeeks.org/circular-linked-list/) (循环单链表实现)
 *
 * @details
 * 循环链表是单链表的一种变体，其中最后一个节点（尾节点）的指针指向第一个节点（头节点），从而形成一个闭环。
 * 这种结构允许从链表中的任意节点出发遍历整个链表。
 *
 * 时间复杂度:
 * - 尾部插入 (Insert): $O(1)$ （因为维护了 `end` 尾节点指针）
 * - 打印 (Print) / 获取元素 (Values): $O(N)$
 * - 销毁 (Erase): $O(N)$
 * 空间复杂度: $O(N)$
 *
 * @note
 * 【未初始化状态与深拷贝死循环 Bug 审计与修复】：
 * 1. **拷贝构造与拷贝赋值中的无限循环 Bug**：原实现在复制链表时使用 `while (node != nullptr)` 进行遍历。
 *    但在循环链表中，节点指针永远成环，`node` 绝不会为 `nullptr`。一旦对非空链表执行拷贝构造或赋值，
 *    程序会进入**死循环，耗尽内存直至崩溃**。
 *    **修复**：改用标准的 `do-while` 结构，通过 `node != copy.root` 作为退出判定条件。
 * 2. **拷贝构造函数中未初始化成员触发未定义悬空指针 Bug**：原拷贝构造函数首行直接调用 `erase()`。
 *    由于是构造阶段，`root` 和 `end` 尚未被初始化（内含内存随机脏数据），直接在 `erase()` 中判定 `root == nullptr` 
 *    会大概率误判为真，导致遍历垃圾地址产生**段错误崩溃（Segmentation Fault）**。
 *    **修复**：在拷贝构造首行显式将 `root` 和 `end` 初始化为 `nullptr`，移除无意义的 `erase()` 调用。
 * 
 * @author [Alvin](https://github.com/polarvoid)
 */

#include <cassert>   
#include <iostream>  
#include <vector>    

namespace operations_on_datastructures {
namespace circular_linked_list {

/**
 * @brief 循环单链表节点结构体
 */
struct Node {
    int64_t data;  ///< 节点存储的数值
    Node* next;    ///< 指向下一个节点的指针

    explicit Node(int64_t _data) : data(_data), next(nullptr) {}

    explicit Node(int64_t _data, Node* _next) : data(_data), next(_next) {}
};

/**
 * @brief 循环单链表类
 */
class CircularLinkedList {
 private:
    Node* root;   ///< 指向链表头节点的指针
    Node* end;    ///< 指向链表尾节点的指针

 public:
    /**
     * @brief 默认构造函数
     */
    CircularLinkedList() : root(nullptr), end(nullptr) {}

    /**
     * @brief 拷贝构造函数
     * @note 核心修复：1. 显式初始化成员为 nullptr，防范 erase() 野指针。2. 引入成环终止条件，规避死循环。
     */
    CircularLinkedList(const CircularLinkedList& copy) : root(nullptr), end(nullptr) {
        if (copy.root == nullptr) {
            return;
        }
        Node* node = copy.root;
        do {
            insert(node->data);
            node = node->next;
        } while (node != copy.root);
    }

    /**
     * @brief 移动构造函数
     */
    CircularLinkedList(CircularLinkedList&& source) noexcept {
        root = source.root;
        end = source.end;
        source.root = nullptr;
        source.end = nullptr;
    }

    /**
     * @brief 拷贝赋值运算符
     * @note 核心修复：防止自赋值，使用成环终止条件防止死循环。
     */
    CircularLinkedList& operator=(const CircularLinkedList& other) {
        if (this != &other) {
            erase();
            if (other.root != nullptr) {
                Node* node = other.root;
                do {
                    insert(node->data);
                    node = node->next;
                } while (node != other.root);
            }
        }
        return *this;
    }

    /**
     * @brief 移动赋值运算符
     */
    CircularLinkedList& operator=(CircularLinkedList&& other) noexcept {
        if (this != &other) {
            erase();
            root = other.root;
            end = other.end;
            other.root = nullptr;
            other.end = nullptr;
        }
        return *this;
    }

    /**
     * @brief 析构函数，回收内存
     */
    ~CircularLinkedList() { erase(); }

    /**
     * @brief 迭代释放链表中的所有节点内存
     */
    void erase() {
        if (root == nullptr) {
            return;
        }
        Node* node = root;
        do {
            Node* temp = node;
            node = node->next;
            delete temp;
        } while (node != root);
        root = nullptr;
        end = nullptr;
    }

    /**
     * @brief 批量插入向量中的所有元素
     */
    void insert(const std::vector<int64_t>& values) {
        for (int64_t value : values) {
            insert(value);
        }
    }

    /**
     * @brief 插入单个数据值
     */
    void insert(int64_t data) {
        Node* node = new Node(data, root);
        insert(node);
    }

    /**
     * @brief 在链表尾部插入一个已有的节点对象
     */
    void insert(Node* node) {
        if (root == nullptr) {
            root = node;        
            node->next = root;  
            end = root;         
        } else {
            end->next = node;   
            node->next = root;  
            end = node;         
        }
    }

    /**
     * @brief 打印当前链表，默认自头节点开始
     */
    void print() { print(root); }

    /**
     * @brief 从指定节点开始，顺时针打印一圈链表数值
     */
    void print(Node* start_node) {
        if (start_node == nullptr) {
            std::cout << "Empty List!\n";
            return;
        }
        Node* temp = start_node;
        do {
            std::cout << temp->data << " ";
            temp = temp->next;
        } while (temp != start_node);
        std::cout << "\n";
    }

    /**
     * @brief 获取包含链表所有元素的向量
     */
    std::vector<int64_t> values() const { return values(root); }

    /**
     * @brief 从指定节点出发，获取一圈的元素向量值
     */
    std::vector<int64_t> values(Node* start_node) const {
        std::vector<int64_t> res;
        if (start_node == nullptr) {
            return res;  
        }
        Node* temp = start_node;
        do {
            res.push_back(temp->data);
            temp = temp->next;
        } while (temp != start_node);
        return res;
    }
};

}  // namespace circular_linked_list
}  // namespace operations_on_datastructures

namespace tests {
using operations_on_datastructures::circular_linked_list::CircularLinkedList;
using operations_on_datastructures::circular_linked_list::Node;

void test1() {
    std::cout << "TEST CASE 1\n";
    std::cout << "Intialized a = {2}\n";
    std::cout << "Expected result: {2}\n";
    CircularLinkedList a;
    std::vector<int64_t> res = {2};
    a.insert(2);
    assert(a.values() == res);
    a.print();
    std::cout << "TEST PASSED!\n\n";
}

void test2() {
    std::cout << "TEST CASE 2\n";
    std::cout << "Intialized a = {2, 5, 6}\n";
    std::cout << "Expected result: {2, 5, 6}\n";
    CircularLinkedList a;
    std::vector<int64_t> res = {2, 5, 6};
    a.insert(2);
    a.insert(5);
    a.insert(6);
    assert(a.values() == res);
    a.print();
    std::cout << "TEST PASSED!\n\n";
}

void test3() {
    std::cout << "TEST CASE 3\n";
    std::cout << "Intialized a = {2, 7, 8, 3, 2, 6}\n";
    std::cout << "Expected result: {2, 7, 8, 3, 2, 6}\n";
    CircularLinkedList a;
    std::vector<int64_t> res = {2, 7, 8, 3, 2, 6};
    a.insert({2, 7, 8, 3, 2, 6});
    a.print();
    assert(a.values() == res);
    std::cout << "TEST PASSED!\n\n";
}

void test4() {
    std::cout << "TEST CASE 4\n";
    std::cout << "Intialized a = {2, 5}\n";
    std::cout << "Expected result: {5, 2}\n";
    CircularLinkedList a;
    std::vector<int64_t> res = {5, 2};
    a.insert(2);
    Node* start = new Node(5);  
    a.insert(start);
    assert(a.values(start) == res);
    a.print(start);
    std::cout << "TEST PASSED!\n\n";
}

void test5() {
    std::cout << "TEST CASE 5\n";
    std::cout << "Intialized a = {}\n";
    std::cout << "Expected result: Empty List!\n";
    CircularLinkedList a;
    std::vector<int64_t> res = {};
    assert(a.values() == res);
    a.print();
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 测试深拷贝正确性，验证修复成果
 */
void test_copy() {
    std::cout << "TEST COPY & ASSIGNMENT\n";
    CircularLinkedList a;
    a.insert({1, 2, 3});
    
    // 测试拷贝构造
    CircularLinkedList b(a);
    assert(b.values() == a.values());
    
    // 测试拷贝赋值
    CircularLinkedList c;
    c = a;
    assert(c.values() == a.values());
    std::cout << "TEST COPY PASSED!\n\n";
}
}  // namespace tests

static void test() {
    tests::test1();
    tests::test2();
    tests::test3();
    tests::test4();
    tests::test5();
    tests::test_copy();
}

int main() {
    test();  
    return 0;
}
