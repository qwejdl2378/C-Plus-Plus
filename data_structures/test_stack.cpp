#include <cassert>    /// 用于 assert 断言
#include <iostream>   /// 用于 std::cout
#include <stdexcept>  /// 用于 std::invalid_argument 异常
#include <vector>     /// 用于 std::vector

#include "./stack.hpp"

/**
 * @brief 测试新构造的栈对象是否默认为空
 * @tparam T 栈保存的元素类型
 */
template <typename T>
void testConstructedStackIsEmpty() {
    const stack<T> curStack;
    assert(curStack.isEmptyStack());
}

/**
 * @brief 测试 push 进栈操作并校验其元素顺序（应当后进先出，倒序排列）
 */
void testPush() {
    using valueType = int;
    stack<valueType> curStack;
    curStack.push(10);
    curStack.push(20);
    curStack.push(30);
    curStack.push(40);
    const auto expectedData = std::vector<valueType>({40, 30, 20, 10});
    assert(curStack.toVector() == expectedData);
}

/**
 * @brief 测试 top() 获取栈顶元素功能
 */
void testTop() {
    using valueType = unsigned;
    stack<valueType> curStack;
    curStack.push(1);
    curStack.push(2);
    curStack.push(3);
    curStack.push(4);
    assert(curStack.top() == static_cast<valueType>(4));
}

/**
 * @brief 测试 pop 出栈功能
 */
void testPop() {
    using valueType = int;
    stack<valueType> curStack;
    curStack.push(100);
    curStack.push(200);
    curStack.push(300);

    assert(curStack.top() == static_cast<valueType>(300));
    curStack.pop();
    assert(curStack.top() == static_cast<valueType>(200));
    curStack.pop();
    assert(curStack.top() == static_cast<valueType>(100));
    curStack.pop();
    assert(curStack.isEmptyStack());
}

/**
 * @brief 测试 clear 清空栈功能
 */
void testClear() {
    stack<int> curStack;
    curStack.push(1000);
    curStack.push(2000);
    curStack.clear();
    assert(curStack.isEmptyStack());
}

/**
 * @brief 测试拷贝构造函数能够生成内容一致的深拷贝栈
 */
void testCopyOfStackHasSameData() {
    stack<int> stackA;
    stackA.push(10);
    stackA.push(200);
    stackA.push(3000);
    const auto stackB(stackA);
    assert(stackA.toVector() == stackB.toVector());
}

/**
 * @brief 验证修改拷贝出的新栈 stackB，原栈 stackA 不受影响
 */
void testPushingToCopyDoesNotChangeOriginal() {
    using valueType = int;
    stack<valueType> stackA;
    stackA.push(10);
    stackA.push(20);
    stackA.push(30);
    auto stackB(stackA);
    stackB.push(40);

    const auto expectedDataA = std::vector<valueType>({30, 20, 10});
    const auto expectedDataB = std::vector<valueType>({40, 30, 20, 10});

    assert(stackA.toVector() == expectedDataA);
    assert(stackB.toVector() == expectedDataB);
}

/**
 * @brief 验证从新栈 stackB 中弹出元素，原栈 stackA 不受影响
 */
void testPoppingFromCopyDoesNotChangeOriginal() {
    using valueType = int;
    stack<valueType> stackA;
    stackA.push(10);
    stackA.push(20);
    stackA.push(30);
    auto stackB(stackA);
    stackB.pop();

    const auto expectedDataA = std::vector<valueType>({30, 20, 10});
    const auto expectedDataB = std::vector<valueType>({20, 10});

    assert(stackA.toVector() == expectedDataA);
    assert(stackB.toVector() == expectedDataB);
}

/**
 * @brief 验证向原栈 stackA 中压入新元素，拷贝栈 stackB 不受影响
 */
void testPushingToOrginalDoesNotChangeCopy() {
    using valueType = int;
    stack<valueType> stackA;
    stackA.push(10);
    stackA.push(20);
    stackA.push(30);
    const auto stackB(stackA);
    stackA.push(40);

    const auto expectedDataA = std::vector<valueType>({40, 30, 20, 10});
    const auto expectedDataB = std::vector<valueType>({30, 20, 10});

    assert(stackA.toVector() == expectedDataA);
    assert(stackB.toVector() == expectedDataB);
}

/**
 * @brief 验证从原栈 stackA 中弹出元素，拷贝栈 stackB 不受影响
 */
void testPoppingFromOrginalDoesNotChangeCopy() {
    using valueType = int;
    stack<valueType> stackA;
    stackA.push(10);
    stackA.push(20);
    stackA.push(30);
    const auto stackB(stackA);
    stackA.pop();

    const auto expectedDataA = std::vector<valueType>({20, 10});
    const auto expectedDataB = std::vector<valueType>({30, 20, 10});

    assert(stackA.toVector() == expectedDataA);
    assert(stackB.toVector() == expectedDataB);
}

/**
 * @brief 测试赋值运算符重载 (=) 的深拷贝和互不干扰性质
 */
void testAssign() {
    using valueType = int;
    stack<valueType> stackA;
    stackA.push(10);
    stackA.push(20);
    stackA.push(30);
    stack<valueType> stackB = stackA;
    stackA.pop();
    stackB.push(40);

    const auto expectedDataA = std::vector<valueType>({20, 10});
    const auto expectedDataB = std::vector<valueType>({40, 30, 20, 10});

    assert(stackA.toVector() == expectedDataA);
    assert(stackB.toVector() == expectedDataB);

    stackB = stackA;
    stackA.pop();
    stackB.push(5);
    stackB.push(6);

    const auto otherExpectedDataA = std::vector<valueType>({10});
    const auto otherExpectedDataB = std::vector<valueType>({6, 5, 20, 10});

    assert(stackA.toVector() == otherExpectedDataA);
    assert(stackB.toVector() == otherExpectedDataB);
}

/**
 * @brief 验证在空栈上调用 top() 是否会引发 std::invalid_argument 异常
 */
void testTopThrowsAnInvalidArgumentWhenStackEmpty() {
    const stack<long double> curStack;
    bool wasException = false;
    try {
        curStack.top();
    } catch (const std::invalid_argument&) {
        wasException = true;
    }
    assert(wasException);
}

/**
 * @brief 验证在空栈上调用 pop() 是否会引发 std::invalid_argument 异常
 */
void testPopThrowsAnInvalidArgumentWhenStackEmpty() {
    stack<bool> curStack;
    bool wasException = false;
    try {
        curStack.pop();
    } catch (const std::invalid_argument&) {
        wasException = true;
    }
    assert(wasException);
}

/**
 * @brief 主函数，运行所有栈相关单元测试用例
 * @returns 0
 */
int main() {
    testConstructedStackIsEmpty<int>();
    testConstructedStackIsEmpty<char>();

    testPush();
    testPop();
    testClear();

    testCopyOfStackHasSameData();
    testPushingToCopyDoesNotChangeOriginal();
    testPoppingFromCopyDoesNotChangeOriginal();
    testPushingToOrginalDoesNotChangeCopy();
    testPoppingFromOrginalDoesNotChangeCopy();

    testAssign();

    testTopThrowsAnInvalidArgumentWhenStackEmpty();
    testPopThrowsAnInvalidArgumentWhenStackEmpty();

    std::cout << "All tests pass!\n";
    return 0;
}
