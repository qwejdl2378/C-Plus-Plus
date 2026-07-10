/**
 * @file
 * @brief Implementation of the [Sublist Search Algorithm](https://www.geeksforgeeks.org/sublist-search-search-a-linked-list-in-another-list) (子链表检索算法)
 * @details
 * 子链表检索用于检测一个单链表（子链表 sublist）是否作为连续的一段出现在另一个单链表（主链表 mainList）中。
 * 类似字符串匹配算法（如 KMP），但操作的对象是链表节点而非字符数组。
 *
 * ### 算法原理
 * 1. 从主链表的当前位置开始，与子链表进行逐节点比对。
 * 2. 如果当前序列完全吻合，则返回 true。
 * 3. 一旦中途出现值不匹配，主链表回溯到开始比对位置的下一个节点，子链表回到头节点，重新开始下一轮比对。
 *
 * @author [Nitin Sharma](https://github.com/foo290)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>
#include <iostream>  /// 用于标准输入输出
#include <vector>    /// 用于 std::vector 容器

/**
 * @namespace search
 * @brief 搜索算法命名空间
 */
namespace search {
/**
 * @namespace sublist_search
 * @brief 子链表检索相关命名空间
 */
namespace sublist_search {
/**
 * @brief 单链表节点结构体 (Node)
 */
struct Node {
    uint32_t data = 0;  ///< 节点保存的值
    Node *next{};       ///< 指向下一个节点的指针
};

/**
 * @brief 打印输出单链表的内容
 * @param start 链表头节点指针
 */
void printLinkedList(Node *start) {
    while (start != nullptr) {
        std::cout << "->" << start->data;
        start = start->next;
    }
    std::cout << std::endl;
}

/**
 * @brief 根据 vector 数据序列快速构建单链表，并返回头指针
 * @param data 包含数值序列的 vector
 * @returns 构造完成的链表头节点指针
 */
Node *makeLinkedList(const std::vector<uint64_t> &data) {
    Node *head = nullptr;
    Node *tail = nullptr;
    for (int i : data) {
        Node *node = new Node;
        node->data = i;
        node->next = nullptr;
        if (head == nullptr) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            tail = tail->next;
        }
    }
    return head;
}

/**
 * @brief 递归清理释放整个链表的内存，防止内存泄漏
 * @param root 链表头指针
 */
void deleteList(Node *const root) {
    if (root != NULL) {
        deleteList(root->next);
        delete root;
    }
}

/**
 * @brief 子链表检索的核心实现函数
 * @param sublist 待查找的子链表头节点指针
 * @param mainList 主链表头节点指针
 * @return `true` 子链表存在于主链表中；`false` 否则
 */
bool sublistSearch(Node *sublist, Node *mainList) {
    if (sublist == nullptr || mainList == nullptr) {
        return false;
    }

    Node *target_ptr = sublist; // 跟踪子链表的当前对比指针

    // 外层循环：遍历主链表中的每一个节点作为潜在的匹配起点
    while (mainList != nullptr) {
        Node *main_ptr = mainList; // 跟踪主链表局部匹配的移动指针

        // 内层循环：从当前位置开始尝试与子链表进行完全比对
        while (target_ptr != nullptr) {
            if (main_ptr == nullptr) {
                // 如果主链表已经走到末尾而子链表还没匹配完，说明绝对匹配不成功了
                return false;
            } else if (main_ptr->data == target_ptr->data) {
                // 当前节点数值吻合，两个指针同时向后移动一步
                target_ptr = target_ptr->next;
                main_ptr = main_ptr->next;
            } else {
                // 出现数值不吻合，打破内层比对，主链表起点右移
                break;
            }
        }

        // 如果子链表指针成功走到了 NULL，说明子链表被完整地匹配到了
        if (target_ptr == nullptr) {
            return true;
        }

        // 未能完全匹配，重置子链表对比指针，并移动主链表的起点到下一个节点
        target_ptr = sublist;
        mainList = mainList->next;
    }

    return false; // 主链表遍历完毕，未找到
}

}  // namespace sublist_search
}  // namespace search

/**
 * @brief 封装了各种自测测试用例的测试类
 */
class TestCases {
 private:
    template <typename T>
    void log(T msg) {
        std::cout << "[TESTS] : ---> " << msg << std::endl;
    }

 public:
    /**
     * @brief 顺序运行所有测试用例
     */
    void runTests() {
        log("Running Tests...");

        testCase_1();
        testCase_2();
        testCase_3();

        log("Test Cases over!");
        std::cout << std::endl;
    }

    /**
     * @brief 测试用例 1：边缘边界测试，只匹配一个元素
     */
    void testCase_1() {
        const bool expectedOutput = true;

        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        log("This is test case 1 for sublist search Algorithm : ");
        log("Description:");
        log("   EDGE CASE : Only contains one element");

        std::vector<uint64_t> sublistData = {6};
        std::vector<uint64_t> mainlistData = {2, 5, 6, 7, 8};

        search::sublist_search::Node *sublistLL =
            search::sublist_search::makeLinkedList(sublistData);
        search::sublist_search::Node *mainlistLL =
            search::sublist_search::makeLinkedList(mainlistData);

        bool exists = search::sublist_search::sublistSearch(sublistLL, mainlistLL);

        log("Checking assert expression...");
        assert(exists == expectedOutput);
        log("Assertion check passed!");

        log("[PASS] : TEST CASE 1 PASS!");
        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

        deleteList(mainlistLL);
        deleteList(sublistLL);
    }

    /**
     * @brief 测试用例 2：常规匹配测试，主链表 100 元素，子链表 20 元素
     */
    void testCase_2() {
        const bool expectedOutput = true;

        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        log("This is test case 2 for sublist search Algorithm : ");
        log("Description:");
        log("   contains main list of 100 elements and sublist of 20");

        std::vector<uint64_t> sublistData(20);
        std::vector<uint64_t> mainlistData(100);

        for (int i = 0; i < 100; i++) {
            mainlistData[i] = i + 1;
        }

        int temp = 0;
        for (int i = 45; i < 65; i++) {
            sublistData[temp] = i + 1;
            temp++;
        }

        search::sublist_search::Node *sublistLL =
            search::sublist_search::makeLinkedList(sublistData);
        search::sublist_search::Node *mainlistLL =
            search::sublist_search::makeLinkedList(mainlistData);

        bool exists = search::sublist_search::sublistSearch(sublistLL, mainlistLL);

        log("Checking assert expression...");
        assert(exists == expectedOutput);
        log("Assertion check passed!");

        log("[PASS] : TEST CASE 2 PASS!");
        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

        deleteList(mainlistLL);
        deleteList(sublistLL);
    }

    /**
     * @brief 测试用例 3：不匹配测试，子序列值不吻合
     * @note 
     * 注意：这里在分配了容量为 20 和 50 的 vector 后又进行了 push_back，
     * 实际上构成了包含前导 0 的主链表 (100个节点) 和子链表 (40个节点)。但不影响测试不匹配功能。
     */
    void testCase_3() {
        const bool expectedOutput = false;

        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        log("This is test case 3 for sublist search Algorithm : ");
        log("Description:");
        log("   contains main list of 50 elements and sublist of 20");

        std::vector<uint64_t> sublistData(20);
        std::vector<uint64_t> mainlistData(50);

        for (int i = 0; i < 50; i++) {
            mainlistData.push_back(i + 1);
        }

        for (int i = 45; i < 65; i++) {
            sublistData.push_back(i + 1);
        }

        search::sublist_search::Node *sublistLL =
            search::sublist_search::makeLinkedList(sublistData);
        search::sublist_search::Node *mainlistLL =
            search::sublist_search::makeLinkedList(mainlistData);

        bool exists = search::sublist_search::sublistSearch(sublistLL, mainlistLL);

        log("Checking assert expression...");
        assert(exists == expectedOutput);
        log("Assertion check passed!");

        log("[PASS] : TEST CASE 3 PASS!");
        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

        deleteList(mainlistLL);
        deleteList(sublistLL);
    }
};

/**
 * @brief 自测用例封装入口
 */
static void test() {
    TestCases tc;
    tc.runTests();
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行封装的自测

    std::vector<uint64_t> mainlistData = {2, 5, 6, 7, 8};
    std::vector<uint64_t> sublistData = {6, 8}; // 非连续，匹配应失败

    search::sublist_search::Node *mainlistLL =
        search::sublist_search::makeLinkedList(mainlistData);
    search::sublist_search::Node *sublistLL =
        search::sublist_search::makeLinkedList(sublistData);

    bool exists = search::sublist_search::sublistSearch(sublistLL, mainlistLL);

    std::cout << "Sublist: " << std::endl;
    search::sublist_search::printLinkedList(sublistLL);

    std::cout << "Main list: " << std::endl;
    search::sublist_search::printLinkedList(mainlistLL);
    std::cout << std::endl;

    if (exists) {
        std::cout << "[TRUE] - sublist found in main list\n";
    } else {
        std::cout << "[FALSE] - sublist NOT found in main list\n";
    }

    deleteList(mainlistLL);
    deleteList(sublistLL);
    return 0;
}
