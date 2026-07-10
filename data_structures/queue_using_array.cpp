/**
 * @file
 * @brief Implementation of Linear [Queue using array](https://www.geeksforgeeks.org/array-implementation-of-queue-simple/). (使用数组实现线性顺序队列)
 * @details
 * 线性顺序队列是保存值序列的数据结构。值可以加到队尾 (enqueue)，或者从队头移除 (dequeue)。
 * 插入操作：增加 `rear` 变量并放入元素。
 * 删除操作：增加 `front` 标示，从而隔离原有元素实现移除。
 *
 * @author [Pooja](https://github.com/pooja-git11)
 * @author [Farbod Ahmadian](https://github.com/farbodahm)
 */
#include <array>     /// 用于 std::array
#include <cstdint>
#include <iostream>  /// 用于输入输出

constexpr uint16_t max_size{10};  ///< 队列的最大上限容量

/**
 * @namespace data_structures
 * @brief 数据结构算法命名空间
 */
namespace data_structures {

/**
 * @namespace queue_using_array
 * @brief 数组队列的实现函数命名空间
 */
namespace queue_using_array {

/**
 * @brief 基于数组实现的队列类
 */
class Queue_Array {
 public:
    void enqueue(const int16_t&);  ///< 向队列尾部插入一个元素
    int dequeue();                 ///< 从队列首部弹出一个元素
    void display() const;          ///< 打印队列中所有有效的元素
 private:
    int8_t front{-1};                     ///< 指向队首的索引指针
    int8_t rear{-1};                      ///< 指向队尾的索引指针
    std::array<int16_t, max_size> arr{};  ///< 存储数据的静态数组
};

/**
 * @brief 向队尾插入一个新元素
 * @param ele 待插入的元素引用
 */
void Queue_Array::enqueue(const int16_t& ele) {
    if (rear == arr.size() - 1) {
        std::cout << "\nQueue is full"; // 队满溢出
    } else if (front == -1 && rear == -1) {
        // 队列为空，初始化 front 和 rear 指针指向 0 并插入
        front = 0;
        rear = 0;
        arr[rear] = ele;
    } else if (rear < arr.size()) {
        // rear 后移一位，插入新元素
        ++rear;
        arr[rear] = ele;
    }
}

/**
 * @brief 从队首弹出一个元素
 * @returns 被弹出的元素值（如果队列为空则返回 0）
 */
int Queue_Array::dequeue() {
    int8_t d{0};
    if (front == -1) {
        std::cout << "\nQueue is empty "; // 队空下溢
        return 0;
    } else if (front == rear) {
        // 队列中只剩最后一个元素，取值后重置为空状态 (-1)
        d = arr.at(front);
        front = rear = -1;
    } else {
        // 取值后 front 队首指针向后移动一位
        d = arr.at(front++);
    }

    return d;
}

/**
 * @brief 打印队列当前全部元素
 */
void Queue_Array::display() const {
    if (front == -1) {
        std::cout << "\nQueue is empty";
    } else {
        for (int16_t i{front}; i <= rear; ++i) std::cout << arr.at(i) << " ";
    }
}

}  // namespace queue_using_array
}  // namespace data_structures

/**
 * @brief 主函数，提供交互式的队列插入、删除和展示菜单
 * @returns 0
 */
int main() {
    int op{0}, data{0};
    data_structures::queue_using_array::Queue_Array ob;

    std::cout << "\n1. enqueue(Insertion) ";
    std::cout << "\n2. dequeue(Deletion)";
    std::cout << "\n3. Display";
    std::cout << "\n4. Exit";
    while (true) {
        std::cout << "\nEnter your choice ";
        std::cin >> op;
        if (op == 1) {
            std::cout << "Enter data  ";
            std::cin >> data;
            ob.enqueue(data);
        } else if (op == 2) {
            data = ob.dequeue();
            std::cout << "\ndequeue element is:\t" << data;
        } else if (op == 3) {
            ob.display();
        } else if (op == 4) {
            exit(0);
        } else {
            std::cout << "\nWrong choice ";
        }
    }

    return 0;
}
