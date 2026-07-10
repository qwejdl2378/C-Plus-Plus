/**
 * @brief Stack Data Structure Using the Queue Data Structure (使用队列实现栈)
 * @details
 * 使用两个队列（std::queue）实现栈的数据结构。通过在 push 动作中进行 O(n) 的转移重排，
 * 可以令新插入的元素总是处于队列的头部，从而实现 O(1) 的 pop 和 top。
 *
 * References used:
 * [StudyTonight](https://www.studytonight.com/data-structures/stack-using-queue)
 * @author [tushar2407](https://github.com/tushar2407)
 */
#include <cassert>   /// 用于 assert 断言
#include <cstdint>
#include <iostream>  /// 用于输入输出
#include <queue>     /// 用于 std::queue 队列

/**
 * @namespace data_structures
 * @brief 数据结构算法命名空间
 */
namespace data_structures {
/**
 * @namespace stack_using_queue
 * @brief 使用队列实现栈的算法命名空间
 */
namespace stack_using_queue {
/**
 * @brief 栈类定义（基于队列）
 */
struct Stack {
    std::queue<int64_t> main_q;       ///< 存储当前栈实际状态的主队列
    std::queue<int64_t> auxiliary_q;  ///< 辅助队列，用于执行插入重排操作
    uint32_t current_size = 0;        ///< 记录当前栈的元素个数

    /**
     * @brief 返回栈顶元素（即主队列的队头元素）
     * @returns 栈顶元素值
     */
    int top() { return main_q.front(); }

    /**
     * @brief 向栈中压入一个新元素 val
     * @details
     * 为了保证后进先出（LIFO）：
     * 1. 将新元素压入空的辅助队列 auxiliary_q。
     * 2. 将主队列 main_q 中当前的所有元素依次出队，压入 auxiliary_q 的后面。
     * 3. 交换两个队列的名字（或指针），使 auxiliary_q 成为新的主队列，清空 auxiliary_q。
     * 此时最新插入的元素就位于主队列队头。
     * 
     * @param val 待插入的值
     */
    void push(int val) {
        auxiliary_q.push(val);
        while (!main_q.empty()) {
            auxiliary_q.push(main_q.front());
            main_q.pop();
        }
        swap(main_q, auxiliary_q); // 交换主辅队列状态
        current_size++;
    }

    /**
     * @brief 弹出栈顶的元素
     */
    void pop() {
        if (main_q.empty()) {
            return;
        }
        main_q.pop();
        current_size--;
    }

    /**
     * @brief 获取当前栈的元素个数
     * @returns 栈的大小
     */
    int size() { return current_size; }
};
}  // namespace stack_using_queue
}  // namespace data_structures

/**
 * @brief 单元自测用例集
 */
static void test() {
    data_structures::stack_using_queue::Stack s;
    s.push(1);  /// 压入 1
    s.push(2);  /// 压入 2
    s.push(3);  /// 压入 3

    assert(s.size() == 3);  /// 验证大小应为 3

    assert(s.top() == 3);  /// 验证当前栈顶应为 3

    s.pop();               /// 弹出 3
    assert(s.top() == 2);  /// 验证当前栈顶应为 2

    s.pop();  /// 弹出 2
    assert(s.top() == 1);  /// 验证当前栈顶应为 1

    s.push(5);             /// 压入 5
    assert(s.top() == 5);  /// 验证栈顶应为 5

    s.pop();               /// 弹出 5
    assert(s.top() == 1);  /// 验证栈顶应恢复为 1

    assert(s.size() == 1);  /// 验证大小应为 1
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行单元测试
    return 0;
}
