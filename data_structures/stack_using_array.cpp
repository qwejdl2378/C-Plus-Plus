#include <cassert>    /// 用于 std::assert
#include <iostream>   /// 用于 std::cout
#include <memory>     /// 用于 std::unique_ptr 智能指针
#include <stdexcept>  /// 用于 std::out_of_range 异常

/**
 * @namespace data_structures
 * @brief 数据结构算法命名空间
 */
namespace data_structures {
/**
 * @brief 栈模板类实现
 * @tparam T 栈中元素的数据类型
 */
template <typename T>
class Stack {
 private:
    std::unique_ptr<T[]> stack;  ///< 指向栈动态数组的唯一智能指针（自动管理生命周期释放内存）
    int stackSize;               ///< 栈的最大上限容量大小
    int stackIndex;              ///< 指向栈顶有效元素的索引指针（初始值为 -1）

 public:
    /**
     * @brief 构造一个新的栈对象
     * @param size 栈的最大大小
     */
    Stack(int size) : stack(new T[size]), stackSize(size), stackIndex(-1) {}

    /**
     * @brief 检查栈是否已满
     * @return `true` 代表已满；`false` 否则
     */
    bool full() const { return stackIndex == stackSize - 1; }

    /**
     * @brief 检查栈是否为空
     * @return `true` 代表为空；`false` 否则
     */
    bool empty() const { return stackIndex == -1; }

    /**
     * @brief 向栈顶压入一个新元素
     * @param element 待压入的元素
     * @throws std::out_of_range 如果栈已满发生上溢
     */
    void push(T element) {
        if (full()) {
            throw std::out_of_range("Stack overflow");
        } else {
            stack[++stackIndex] = element;
        }
    }

    /**
     * @brief 弹出栈顶的元素并返回
     * @return 被弹出的栈顶元素
     * @throws std::out_of_range 如果栈为空发生下溢
     */
    T pop() {
        if (empty()) {
            throw std::out_of_range("Stack underflow");
        }
        return stack[stackIndex--];
    }

    /**
     * @brief 打印栈中的所有元素（从底到顶）
     */
    void show() const {
        for (int i = 0; i <= stackIndex; i++) {
            std::cout << stack[i] << "\n";
        }
    }

    /**
     * @brief 返回当前栈顶的元素值，不执行弹出操作
     * @return 栈顶元素值
     * @throws std::out_of_range 如果栈为空
     */
    T topmost() const {
        if (empty()) {
            throw std::out_of_range("Stack underflow");
        }
        return stack[stackIndex];
    }

    /**
     * @brief 返回当前栈底的元素值
     * @return 栈底元素值（即索引为 0 处的元素）
     * @throws std::out_of_range 如果栈为空
     */
    T bottom() const {
        if (empty()) {
            throw std::out_of_range("Stack underflow");
        }
        return stack[0];
    }
};
}  // namespace data_structures

/**
 * @brief 单元自测用例集
 */
static void test() {
    data_structures::Stack<int> stack(5);

    // 测试空、满判断
    assert(stack.empty());
    assert(!stack.full());

    // 压入元素并测试栈顶值
    stack.push(10);
    assert(stack.topmost() == 10);

    stack.push(20);
    assert(stack.topmost() == 20);

    stack.push(30);
    stack.push(40);
    stack.push(50);
    assert(stack.full());

    // 溢出异常处理测试
    try {
        stack.push(60);
    } catch (const std::out_of_range& e) {
        assert(std::string(e.what()) == "Stack overflow");
    }

    // 弹出元素测试
    assert(stack.pop() == 50);
    assert(stack.pop() == 40);
    assert(stack.pop() == 30);

    // 栈顶和栈底检测
    assert(stack.topmost() == 20);
    assert(stack.bottom() == 10);

    assert(stack.pop() == 20);
    assert(stack.pop() == 10);

    assert(stack.empty());
    assert(!stack.full());

    // 下溢异常处理测试
    try {
        stack.pop();
    } catch (const std::out_of_range& e) {
        assert(std::string(e.what()) == "Stack underflow");
    }

    try {
        stack.topmost();
    } catch (const std::out_of_range& e) {
        assert(std::string(e.what()) == "Stack underflow");
    }

    try {
        stack.bottom();
    } catch (const std::out_of_range& e) {
        assert(std::string(e.what()) == "Stack underflow");
    }
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行单元测试
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
