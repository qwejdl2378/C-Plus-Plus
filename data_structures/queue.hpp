/* This class specifies the basic operation on a queue as a linked list */
#ifndef DATA_STRUCTURES_QUEUE_HPP_
#define DATA_STRUCTURES_QUEUE_HPP_

#include "node.hpp"

/**
 * @brief 泛型链表队列类模板
 * @tparam ValueType 存储的元素数据类型
 */
template <class ValueType>
class queue {
    using node_type = Node<ValueType>;

 public:
    using value_type = ValueType;
    
    /**
     * @brief 在标准输出流中可视化打印队列所有元素
     */
    void display() const {
        std::cout << "Front --> ";
        display_all(this->queueFront.get());
        std::cout << '\n';
        std::cout << "Size of queue: " << size << '\n';
    }

    /**
     * @brief 将队列元素导出到 std::vector 中
     * @return 包含队列所有元素且保持顺序一致的 std::vector
     */
    std::vector<value_type> toVector() const {
        return push_all_to_vector(this->queueFront.get(), this->size);
    }

 private:
    /**
     * @brief 辅助函数：如果队列为空则抛出 invalid_argument 异常
     * @exception std::invalid_argument 如果队列为空
     */
    void ensureNotEmpty() const {
        if (isEmptyQueue()) {
            throw std::invalid_argument("Queue is empty.");
        }
    }

 public:
    /**
     * @brief 检查队列是否为空
     * @return `true` 代表为空；`false` 否则
     */
    bool isEmptyQueue() const { return (queueFront == nullptr); }

    /**
     * @brief 向队列尾部插入一个新元素（入队）
     * @param item 新元素的值
     */
    void enQueue(const value_type& item) {
        auto newNode = std::make_shared<node_type>();
        newNode->data = item;
        newNode->next = nullptr;
        if (isEmptyQueue()) {
            queueFront = newNode;
            queueRear = newNode;
        } else {
            queueRear->next = newNode;
            queueRear = queueRear->next;
        }
        ++size;
    }

    /**
     * @brief 获取当前队首的元素
     * @return 队首元素的值
     * @exception std::invalid_argument 如果队列为空
     */
    value_type front() const {
        ensureNotEmpty();
        return queueFront->data;
    }

    /**
     * @brief 弹出队首元素（出队）
     * @exception std::invalid_argument 如果队列为空
     */
    void deQueue() {
        ensureNotEmpty();
        queueFront = queueFront->next;
        --size;
    }

    /**
     * @brief 清空队列中的所有元素
     */
    void clear() {
        queueFront = nullptr;
        queueRear = nullptr;
        size = 0;
    }

 private:
    std::shared_ptr<node_type> queueFront =
        {}; /**< 指向队首的共享智能指针 */
    std::shared_ptr<node_type> queueRear =
        {}; /**< 指向队尾的共享智能指针 */
    std::size_t size = 0; ///< 队列当前的有效元素计数器
};

#endif  // DATA_STRUCTURES_QUEUE_HPP_
