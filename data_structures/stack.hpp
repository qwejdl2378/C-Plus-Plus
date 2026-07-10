/**
 * @file
 * @author danghai
 * @author [Piotr Idzik](https://github.com/vil02)
 * @brief This class specifies the basic operation on a stack as a linked list (基于单链表实现的泛型栈)
 **/
#ifndef DATA_STRUCTURES_STACK_HPP_
#define DATA_STRUCTURES_STACK_HPP_

#include <stdexcept>  /// 用于 std::invalid_argument

#include "node.hpp"  /// 用于 Node 节点结构体和辅助工具

/**
 * @brief 基于单链表实现的泛型栈类模板
 * @tparam ValueType 存储的元素数据类型
 */
template <class ValueType>
class stack {
 public:
    using value_type = ValueType;

    /**
     * @brief 打印当前栈中的全部元素（从栈顶到栈底）
     */
    void display() const {
        std::cout << "Top --> ";
        display_all(this->stackTop.get());
        std::cout << '\n';
        std::cout << "Size of stack: " << size << std::endl;
    }

    /**
     * @brief 将栈中的元素导出到 std::vector 中
     * @return 包含栈所有元素且自顶向下的 std::vector
     */
    std::vector<value_type> toVector() const {
        return push_all_to_vector(this->stackTop.get(), this->size);
    }

 private:
    /**
     * @brief 辅助函数：如果栈为空则抛出异常
     * @exception std::invalid_argument 如果栈为空
     */
    void ensureNotEmpty() const {
        if (isEmptyStack()) {
            throw std::invalid_argument("Stack is empty.");
        }
    }

 public:
    /**
     * @brief 检查栈是否为空
     * @return `true` 代表为空；`false` 否则
     */
    bool isEmptyStack() const { return (stackTop == nullptr); }

    /**
     * @brief 向栈顶压入一个新元素 (Push)
     * @param item 新元素的值
     */
    void push(const value_type& item) {
        auto newNode = std::make_shared<Node<value_type>>();
        newNode->data = item;
        newNode->next = stackTop; ///< 新节点指向旧栈顶
        stackTop = newNode;       ///< 更新栈顶指针
        size++;
    }

    /**
     * @brief 获取栈顶的元素值，不弹出
     * @return 栈顶元素值
     * @exception std::invalid_argument 如果栈为空
     */
    value_type top() const {
        ensureNotEmpty();
        return stackTop->data;
    }

    /**
     * @brief 弹出栈顶的第一个元素 (Pop)
     * @exception std::invalid_argument 如果栈为空
     */
    void pop() {
        ensureNotEmpty();
        stackTop = stackTop->next; ///< 栈顶指针移向下一个元素
        size--;
    }

    /**
     * @brief 清空当前栈
     */
    void clear() {
        stackTop = nullptr;
        size = 0;
    }

 private:
    std::shared_ptr<Node<value_type>> stackTop =
        {};                /**< 指向栈顶的共享智能指针 */
    std::size_t size = 0;  ///< 栈的大小计数器
};

#endif  // DATA_STRUCTURES_STACK_HPP_
