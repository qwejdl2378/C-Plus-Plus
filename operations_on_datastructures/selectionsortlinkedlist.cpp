/**
 * @file
 * @brief Selection sort implementation on a Linked List (单链表选择排序算法实现)
 *
 * @details
 * 选择排序在单链表上的实现通常通过直接修改指针关系（而不是仅交换节点中的数据域）来完成。
 * 算法维护已排序部分和未排序部分：
 * 1. 每次从未排序部分挑出一个最小值节点。
 * 2. 调整指针，将该节点插入到已排序部分的末尾（也就是未排序部分的前面）。
 * 3. 循环往复，直到未排序部分只剩下一个节点。
 *
 * 时间复杂度: $O(N^2)$，其中 $N$ 是链表的长度。
 * 空间复杂度: $O(1)$，原地对指针进行修改。
 *
 * @note
 * 【空指针崩溃与交互挂起 Bug 审计与修复】：
 * 1. **空链表指针解引用崩溃 Bug**：在 `my_selection_sort_linked_list` 中，
 *    首行直接执行 `node *current = min->link;`。若传入空链表（`head == nullptr`），
 *    会导致 `nullptr->link` 的空指针解引用崩溃（Segmentation Fault）。
 *    **修复**：在入口处添加 `if (head == nullptr || head->link == nullptr) return;` 保护。
 * 2. **自动测试交互挂起 Bug**：原程序中 `main()` 完全依赖交互式 `cin` 输入，这会在自动化测试环境中导致无限等待挂起。
 *    **修复**：去除交互式依赖，设计基于 vector 直接初始化的 `assert` 自测机制，并彻底释放链表堆空间防范泄漏。
 */

#include <cassert>
#include <iostream>
#include <vector>

namespace operations_on_datastructures {
namespace selection_sort_list {

/**
 * @brief 链表节点类
 */
class Node {
 public:
    int data;
    Node *link;
    explicit Node(int d) : data(d), link(nullptr) {}
};

/**
 * @brief 顺序打印链表
 */
void print(Node *head) {
    Node *current = head;
    while (current != nullptr) {
        std::cout << current->data << " ";
        current = current->link;
    }
    std::cout << std::endl;
}

/**
 * @brief 根据数组向量快速构建一个单链表
 */
Node *createlist(const std::vector<int>& arr) {
    Node *head = nullptr;
    Node *t = nullptr;
    for (int num : arr) {
        Node *temp = new Node(num);
        if (head == nullptr) {
            head = temp;
            t = temp;
            continue;
        }
        t->link = temp;
        t = temp;
    }
    return head;
}

/**
 * @brief 释放链表的所有节点内存，消除泄漏
 */
void delete_list(Node *&head) {
    Node *current = head;
    while (current != nullptr) {
        Node *next_node = current->link;
        delete current;
        current = next_node;
    }
    head = nullptr;
}

/**
 * @brief 将链表元素转换为 vector 以方便测试验证
 */
std::vector<int> get_values(Node *head) {
    std::vector<int> res;
    Node *current = head;
    while (current != nullptr) {
        res.push_back(current->data);
        current = current->link;
    }
    return res;
}

/**
 * @brief 对单链表进行原地选择排序（仅修改指针）
 * @param head 链表头节点的引用
 */
void my_selection_sort_linked_list(Node *&head) {
    // 核心修复：添加边界安全判定，防止空链表解引用崩溃
    if (head == nullptr || head->link == nullptr) {
        return;
    }

    Node *min = head;  
    Node *current = min->link;  
    Node *previous = min;  
    Node *temp = nullptr;  

    while (min->link != nullptr) {
        while (current != nullptr) {
            if (current->data < min->data) {
                if (temp == nullptr) {
                    if (previous == min) {
                        head = current;  
                        min->link = current->link;
                        current->link = previous;
                        min = current;
                        current = previous->link;
                    } else {
                        head = current;  
                        previous->link = current->link;
                        current->link = min;
                        min = current;
                        current = previous->link;
                    }
                } else {
                    temp->link = current;
                    previous->link = current->link;
                    current->link = min;
                    min = current;
                    current = previous->link;
                }
            } else {
                previous = previous->link;
                current = current->link;
            }
        }

        temp = min;
        min = min->link;
        previous = min;
        current = min->link;
    }
}

}  // namespace selection_sort_list
}  // namespace operations_on_datastructures

/**
 * @brief 单元自测用例
 */
static void test() {
    using namespace operations_on_datastructures::selection_sort_list;

    // 1. 空链表测试
    Node *empty_head = nullptr;
    my_selection_sort_linked_list(empty_head);
    assert(empty_head == nullptr);

    // 2. 单个节点测试
    Node *single_head = new Node(5);
    my_selection_sort_linked_list(single_head);
    assert(single_head->data == 5 && single_head->link == nullptr);
    delete_list(single_head);

    // 3. 常规逆序测试
    Node *head = createlist({8, 9, 3, 1, 4});
    my_selection_sort_linked_list(head);
    std::vector<int> expected1 = {1, 3, 4, 8, 9};
    assert(get_values(head) == expected1);
    delete_list(head);

    // 4. 包含负数测试
    head = createlist({5, 3, 4, 1, -2, -4});
    my_selection_sort_linked_list(head);
    std::vector<int> expected2 = {-4, -2, 1, 3, 4, 5};
    assert(get_values(head) == expected2);
    delete_list(head);

    std::cout << "All selection sort linked list tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测

    std::cout << "Would you like to run interactive mode? (1 for Yes, 0 for No): ";
    int run_interactive = 0;
    if (!(std::cin >> run_interactive) || run_interactive == 0) {
        return 0;
    }

    int n;
    std::cout << "enter the no. of nodes : ";  
    if (!(std::cin >> n) || n <= 0)
        return 0;

    std::vector<int> arr(n);
    std::cout << "Enter " << n << " values:\n";
    for (int i = 0; i < n; i++) {
        std::cin >> arr[i];
    }

    operations_on_datastructures::selection_sort_list::Node *head = operations_on_datastructures::selection_sort_list::createlist(arr);
    std::cout << "original list is : ";
    operations_on_datastructures::selection_sort_list::print(head);                          

    operations_on_datastructures::selection_sort_list::my_selection_sort_linked_list(head);  
    std::cout << "sorted list is : ";
    operations_on_datastructures::selection_sort_list::print(head);  

    operations_on_datastructures::selection_sort_list::delete_list(head);
    return 0;
}