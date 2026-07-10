#include <cassert>   /// 用于 assert 断言
#include <iostream>  /// 用于 std::cout

#include "./queue.hpp"

/**
 * @brief 测试新构造的队列是否默认为空
 * @tparam T 队列保存的类型
 */
template <typename T>
void testConstructedQueueIsEmpty() {
    const queue<T> curQueue;
    assert(curQueue.isEmptyQueue());
}

/**
 * @brief 测试 enQueue 入队功能
 */
void testEnQueue() {
    queue<int> curQueue;
    curQueue.enQueue(10);
    assert(curQueue.toVector() == std::vector<int>({10}));
    curQueue.enQueue(20);
    assert(curQueue.toVector() == std::vector<int>({10, 20}));
    curQueue.enQueue(30);
    curQueue.enQueue(40);
    assert(curQueue.toVector() == std::vector<int>({10, 20, 30, 40}));
}

/**
 * @brief 测试 deQueue 出队功能
 */
void testDeQueue() {
    queue<int> curQueue;
    curQueue.enQueue(10);
    curQueue.enQueue(20);
    curQueue.enQueue(30);

    curQueue.deQueue(); // 弹出 10
    assert(curQueue.toVector() == std::vector<int>({20, 30}));

    curQueue.deQueue(); // 弹出 20
    assert(curQueue.toVector() == std::vector<int>({30}));

    curQueue.deQueue(); // 弹出 30
    assert(curQueue.isEmptyQueue());
}

/**
 * @brief 测试获取队首元素的功能
 */
void testFront() {
    queue<int> curQueue;
    curQueue.enQueue(10);
    assert(curQueue.front() == 10);
    curQueue.enQueue(20);
    assert(curQueue.front() == 10);
}

/**
 * @brief 测试 clear 队列清空功能
 */
void testQueueAfterClearIsEmpty() {
    queue<int> curQueue;
    curQueue.enQueue(10);
    curQueue.enQueue(20);
    curQueue.enQueue(30);
    curQueue.clear();
    assert(curQueue.isEmptyQueue());
}

/**
 * @brief 验证空队列调用 front() 时是否会抛出 invalid_argument 异常
 */
void testFrontThrowsAnInvalidArgumentWhenQueueEmpty() {
    const queue<int> curQueue;
    bool wasException = false;
    try {
        curQueue.front();
    } catch (const std::invalid_argument&) {
        wasException = true;
    }
    assert(wasException);
}

/**
 * @brief 验证空队列调用 deQueue() 时是否会抛出 invalid_argument 异常
 */
void testDeQueueThrowsAnInvalidArgumentWhenQueueEmpty() {
    queue<int> curQueue;
    bool wasException = false;
    try {
        curQueue.deQueue();
    } catch (const std::invalid_argument&) {
        wasException = true;
    }
    assert(wasException);
}

/**
 * @brief 主函数，依次运行所有队列测试用例
 * @returns 0
 */
int main() {
    testConstructedQueueIsEmpty<int>();
    testConstructedQueueIsEmpty<double>();
    testConstructedQueueIsEmpty<std::vector<long double>>();

    testEnQueue();
    testDeQueue();

    testQueueAfterClearIsEmpty();

    testFrontThrowsAnInvalidArgumentWhenQueueEmpty();
    testDeQueueThrowsAnInvalidArgumentWhenQueueEmpty();

    std::cout << "All tests pass!\n";
    return 0;
}
