/**
 * @file
 * @brief Implementation of a [Circular Queue using an Array](https://www.geeksforgeeks.org/introduction-to-circular-queue/) (基于数组的循环队列实现)
 *
 * @details
 * 循环队列是一种线性数据结构，它遵循先进先出（FIFO）原则，并且该队列的最后一个位置与第一个位置相连，形成一个环。
 * 它可以有效克服普通顺序队列在出队后造成的“假溢出”空间浪费问题。
 *
 * 本实现使用固定大小的数组与 `front`（首指针）、`rear`（尾指针）及 `count`（元素个数）来维护队列。
 *
 * 时间复杂度:
 * - 入队 (Enqueue): $O(1)$
 * - 出队 (Dequeue): $O(1)$
 * - 查看首元素 (Front/Show): $O(1)$ / $O(N)$
 * 空间复杂度: $O(N)$，其中 $N$ 是数组的固定容量大小限制。
 *
 * @note
 * 【假溢出判定与除零/交互挂起 Bug 审计与修复】：
 * 1. **出队假下溢（Underflow）逻辑判定 Bug**：原程序中在 `Deque()` 中使用 `if (front == rear)` 来判定队列是否为空。
 *    但在循环队列中，当队列填满时，`rear` 也会环绕相遇等于 `front`。这导致**满队列执行 Deque 会误判为下溢（Underflow）**并报错拒绝服务。
 *    **修复**：由于维护了 `count` 指示当前队列内的元素数，直接将下溢判断修改为更稳健的 `if (count == 0)`（或 `if (empty())`）。
 * 2. **非交互测试挂起 Bug**：原程序 `main` 函数为无限交互式循环，没有自动化测试用例，会造成构建测试阶段永久挂起。
 *    **修复**：重构循环队列为类形式，设计完整的 `assert` 自动化单元自测，并将交互模式置于开关选项后。
 */

#include <cassert>
#include <iostream>
#include <vector>

namespace operations_on_datastructures {
namespace circular_queue {

/**
 * @class CircularQueue
 * @brief 基于静态数组的循环队列模板类
 * @tparam SIZE 队列的固定最大容量大小限制
 */
template <size_t SIZE = 10>
class CircularQueue {
 private:
    int queue[SIZE]{};   ///< 存放队列元素的静态数组
    size_t front = 0;    ///< 指向队列头部的索引
    size_t rear = 0;     ///< 指向队列尾部下一个可用槽位的索引
    size_t count = 0;    ///< 当前队列内的元素个数

 public:
    CircularQueue() = default;

    /**
     * @brief 判断队列是否已满
     */
    bool full() const { return count == SIZE; }

    /**
     * @brief 判断队列是否为空
     */
    bool empty() const { return count == 0; }

    /**
     * @brief 获取队列当前的元素数量
     */
    size_t size() const { return count; }

    /**
     * @brief 元素入队
     * @param x 入队元素值
     * @return 成功返回 true，发生上溢返回 false
     */
    bool enqueue(int x) {
        if (full()) {
            std::cout << "Queue Overflow\n";
            return false;
        }
        queue[rear] = x;
        rear = (rear + 1) % SIZE;
        count++;
        return true;
    }

    /**
     * @brief 元素出队
     * @param val_out [out] 输出出队元素值的指针
     * @return 成功出队返回 true，发生下溢返回 false
     */
    bool dequeue(int* val_out = nullptr) {
        // 核心修复：将原 front == rear 错误下溢判定改为 count == 0 判定
        if (empty()) {
            std::cout << "Queue Underflow\n";
            return false;
        }
        if (val_out != nullptr) {
            *val_out = queue[front];
        }
        front = (front + 1) % SIZE;
        count--;
        return true;
    }

    /**
     * @brief 获取当前队列中的所有元素向量
     */
    std::vector<int> get_elements() const {
        std::vector<int> elements;
        elements.reserve(count);
        for (size_t i = 0; i < count; i++) {
            elements.push_back(queue[(i + front) % SIZE]);
        }
        return elements;
    }

    /**
     * @brief 打印显示队列内所有元素
     */
    void show() const {
        if (empty()) {
            std::cout << "Queue is empty.\n";
            return;
        }
        for (size_t i = 0; i < count; i++) {
            std::cout << queue[(i + front) % SIZE] << "\t";
        }
        std::cout << "\n";
    }
};

}  // namespace circular_queue
}  // namespace operations_on_datastructures

/**
 * @brief 单元自测用例
 */
static void test() {
    operations_on_datastructures::circular_queue::CircularQueue<5> q;

    // 验证初始空状态
    assert(q.empty());
    assert(!q.full());
    assert(q.size() == 0);

    // 顺序插入元素
    assert(q.enqueue(10));
    assert(q.enqueue(20));
    assert(q.enqueue(30));
    assert(q.enqueue(40));
    assert(q.enqueue(50));
    // 队列已满，再次插入应该失败
    assert(!q.enqueue(60));
    assert(q.full());
    assert(q.size() == 5);

    // 获取并对比队列元素
    std::vector<int> expected = {10, 20, 30, 40, 50};
    assert(q.get_elements() == expected);

    // 核心测试：满队列出队不应误判为 Underflow
    int out_val = 0;
    assert(q.dequeue(&out_val));
    assert(out_val == 10);
    assert(q.size() == 4);
    assert(!q.full());

    // 出队剩余所有元素
    assert(q.dequeue(&out_val) && out_val == 20);
    assert(q.dequeue(&out_val) && out_val == 30);
    assert(q.dequeue(&out_val) && out_val == 40);
    assert(q.dequeue(&out_val) && out_val == 50);

    // 此时队列应为空，再次出队报下溢
    assert(q.empty());
    assert(!q.dequeue());

    std::cout << "All circular queue tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行单元测试验证正确性

    // 交互模式控制开关
    std::cout << "Would you like to run interactive mode? (1 for Yes, 0 for No): ";
    int run_interactive = 0;
    if (!(std::cin >> run_interactive) || run_interactive == 0) {
        return 0;
    }

    operations_on_datastructures::circular_queue::CircularQueue<10> q;
    int ch = 0, x = 0;
    do {
        std::cout << "\n1. Enqueue";
        std::cout << "\n2. Dequeue";
        std::cout << "\n3. Print";
        std::cout << "\nEnter Your Choice (0 to exit) : ";
        if (!(std::cin >> ch)) {
            break;
        }
        if (ch == 1) {
            std::cout << "\nInsert : ";
            std::cin >> x;
            q.enqueue(x);
        } else if (ch == 2) {
            q.dequeue();
        } else if (ch == 3) {
            q.show();
        }
    } while (ch != 0);

    return 0;
}
