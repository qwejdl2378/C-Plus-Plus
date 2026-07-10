/**
 * @author [shoniavika](https://github.com/shoniavika)
 * @file
 *
 * Implementation of a Queue using two Stacks. (使用两个栈实现队列)
 */

#include <cassert>
#include <iostream>
#include <stack>

namespace {
/**
 * @brief 队列数据结构模板类。以 FIFO (先进先出) 的方式存储元素。
 * @tparam T 存储在队列中的元素数据类型
 */
template <typename T>
class MyQueue {
 private:
    std::stack<T> s1, s2; ///< 辅助栈 s1，主存储栈 s2

 public:
    /**
     * @brief 默认构造函数
     */
    MyQueue() = default;

    /**
     * @brief 向队列尾部插入一个元素 x
     */
    void push(T x);

    /**
     * @brief 移除并返回队列首部的元素
     */
    const T& pop();

    /**
     * @brief 返回队列首部元素值，不移除它
     */
    const T& peek() const;

    /**
     * @brief 检查队列是否为空
     */
    bool empty() const;
};

/**
 * @brief 向队列尾部追加元素
 * @details 为了保持先进先出特性，我们需要将元素颠倒顺序存储：
 * 1. 将 s2 中所有元素依次出栈并压入 s1。
 * 2. 将新元素 x 压入空的 s2。
 * 3. 再将 s1 中的全部元素依次出栈压回 s2。
 * 这样做的结果是，最早进入队列的元素总是位于 s2 的栈顶，从而实现 O(1) 的 pop 和 peek 操作。
 */
template <typename T>
void MyQueue<T>::push(T x) {
    while (!s2.empty()) {
        s1.push(s2.top());
        s2.pop();
    }
    s2.push(x);
    while (!s1.empty()) {
        s2.push(s1.top());
        s1.pop();
    }
}

/**
 * @brief 弹出并返回队首的元素
 */
template <typename T>
const T& MyQueue<T>::pop() {
    const T& temp = MyQueue::peek();
    s2.pop();
    return temp;
}

/**
 * @brief 返回队首位置的元素，不执行弹出操作
 */
template <typename T>
const T& MyQueue<T>::peek() const {
    if (!empty()) {
        return s2.top();
    }
    std::cerr << "Queue is empty" << std::endl;
    exit(0);
}

/**
 * @brief 判断队列是否为空
 */
template <typename T>
bool MyQueue<T>::empty() const {
    return s2.empty() && s1.empty();
}
}  // namespace

/**
 * @brief 单元自测用例集
 */
void queue_test() {
    MyQueue<int> que;
    std::cout << "Test #1\n";
    que.push(2);
    que.push(5);
    que.push(0);
    assert(que.peek() == 2);
    assert(que.pop() == 2);
    assert(que.peek() == 5);
    assert(que.pop() == 5);
    assert(que.peek() == 0);
    assert(que.pop() == 0);
    assert(que.empty() == true);
    std::cout << "PASSED\n";

    std::cout << "Test #2\n";
    que.push(-1);
    assert(que.empty() == false);
    assert(que.peek() == -1);
    assert(que.pop() == -1);
    std::cout << "PASSED\n";

    MyQueue<double> que2;
    std::cout << "Test #3\n";
    que2.push(2.31223);
    que2.push(3.1415926);
    que2.push(2.92);

    assert(que2.peek() == 2.31223);
    assert(que2.pop() == 2.31223);
    assert(que2.peek() == 3.1415926);
    assert(que2.pop() == 3.1415926);
    assert(que2.peek() == 2.92);
    assert(que2.pop() == 2.92);
    std::cout << "PASSED\n";
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    queue_test();
    return 0;
}
