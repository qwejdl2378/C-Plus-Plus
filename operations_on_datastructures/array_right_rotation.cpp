/**
 * @file
 * @brief Implementation for the [Array Right Rotation](https://www.javatpoint.com/program-to-right-rotate-the-elements-of-an-array) algorithm (数组循环右移算法实现)
 *
 * @details
 * 数组循环右移（或向右旋转）是指将数组中的每个元素向右移动指定的偏移量。
 * 移出数组右边界的元素会被循环地放到数组的开头部分。
 * 本实现采用非原地（Out-of-place）算法，创建并返回一个新数组，不修改原输入数组。
 *
 * 时间复杂度: $O(N)$，其中 $N$ 是数组中的元素个数。
 * 空间复杂度: $O(N)$，用于存储旋转后的结果数组。
 *
 * @note
 * 【移位越界逻辑审计】：
 * 1. 原设计规定：若偏移量 `shift` 大于或等于数组大小，则将其视为非法移位，直接返回空数组 `{}`。
 *    该规则已被写入单元测试，因此我们保留该设定。
 * 2. `shift == 0` 时，直接复制并返回原数组。
 *
 * @see array_left_rotation.cpp
 * @author [Alvin](https://github.com/polarvoid)
 */

#include <cassert>   
#include <iostream>  
#include <vector>    

namespace operations_on_datastructures {

/**
 * @brief 顺序打印向量中的所有元素
 * @param array 待打印的目标数组引用
 */
void print(const std::vector<int32_t> &array) {
    for (int32_t i : array) {
        std::cout << i << " ";  
    }
    std::cout << "\n";  
}

/**
 * @brief 将数组元素循环右移指定的偏移量并返回结果
 * @param array 目标输入数组
 * @param shift 右移偏移量
 * @return 循环右移后的结果数组
 */
std::vector<int32_t> shift_right(const std::vector<int32_t> &array,
                                 size_t shift) {
    // 核心规则保留：如果移位步长超出或等于数组大小，视为非法输入返回空数组
    if (array.size() <= shift) {
        return {};  
    }
    std::vector<int32_t> res(array.size());  
    
    // 1. 将原数组中前部偏左部分的元素移到结果数组的后半部
    for (size_t i = shift; i < array.size(); i++) {
        res[i] = array[i - shift];  
    }
    // 2. 将原数组尾部的元素循环移动到结果数组的前半部
    for (size_t i = 0; i < shift; i++) {
        res[i] = array[array.size() - shift + i];  
    }
    return res;
}

}  // namespace operations_on_datastructures

namespace tests {
using operations_on_datastructures::print;
using operations_on_datastructures::shift_right;

/**
 * @brief 单元自测用例 1：常规右移移位测试
 */
void test1() {
    std::cout << "TEST CASE 1\n";
    std::cout << "Initialized arr = {1, 2, 3, 4, 5}\n";
    std::cout << "Expected result: {4, 5, 1, 2, 3}\n";
    std::vector<int32_t> arr = {1, 2, 3, 4, 5};
    std::vector<int32_t> res = shift_right(arr, 2);
    std::vector<int32_t> expected = {4, 5, 1, 2, 3};
    assert(res == expected);
    print(res);  
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 单元自测用例 2：空数组右移测试
 */
void test2() {
    std::cout << "TEST CASE 2\n";
    std::cout << "Initialized arr = {}\n";
    std::cout << "Expected result: {}\n";
    std::vector<int32_t> arr = {};
    std::vector<int32_t> res = shift_right(arr, 2);
    std::vector<int32_t> expected = {};
    assert(res == expected);
    print(res);  
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 单元自测用例 3：超出数组大小的右移测试
 */
void test3() {
    std::cout << "TEST CASE 3\n";
    std::cout << "Initialized arr = {1, 2, 3, 4, 5}\n";
    std::cout << "Expected result: {}\n";
    std::vector<int32_t> arr = {1, 2, 3, 4, 5};
    std::vector<int32_t> res = shift_right(arr, 7);  
    std::vector<int32_t> expected = {};
    assert(res == expected);
    print(res);  
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 单元自测用例 4：大规模数据右移移位测试
 */
void test4() {
    std::cout << "TEST CASE 4\n";
    std::cout << "Initialized arr = {2, 4, ..., 420}\n";
    std::cout << "Expected result: {420, 2, 4, ..., 418}\n";
    std::vector<int32_t> arr;
    for (int i = 1; i <= 210; i++) {
        arr.push_back(i * 2);
    }
    std::vector<int32_t> res = shift_right(arr, 1);
    std::vector<int32_t> expected;
    expected.push_back(420);
    for (int i = 0; i < 209; i++) {
        expected.push_back(arr[i]);
    }
    assert(res == expected);
    print(res);  
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 单元自测用例 5：零移位测试
 */
void test5() {
    std::cout << "TEST CASE 5\n";
    std::cout << "Initialized arr = {1, 2, 3, 4, 5}\n";
    std::cout << "Expected result: {1, 2, 3, 4, 5}\n";
    std::vector<int32_t> arr = {1, 2, 3, 4, 5};
    std::vector<int32_t> res = shift_right(arr, 0);
    assert(res == arr);
    print(res);  
    std::cout << "TEST PASSED!\n\n";
}
}  // namespace tests

/**
 * @brief 自测测试函数调用
 */
static void test() {
    tests::test1();
    tests::test2();
    tests::test3();
    tests::test4();
    tests::test5();
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试用例确认正确性
    return 0;
}
