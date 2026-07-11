/**
 * @file
 * @brief Implementation for the [Union of two sorted Arrays](https://en.wikipedia.org/wiki/Union_(set_theory)) algorithm (两个已排序数组的并集算法实现)
 *
 * @details
 * 两个已排序数组的并集是指包含这两个数组中所有唯一元素的集合，并且结果数组按升序排列。
 * 本实现要求输入的两个数组必须是已升序排序的。
 * 通过双指针线性扫描法，比较指针所指节点值：
 * - 将较小的值存入临时变量并向前移动对应指针。
 * - 遇到相同元素时则任选其一，同时移动两个指针。
 * - 当一个数组扫描完毕，将另一个数组的剩余元素追加到末尾。
 * - 每次向结果数组中插入新元素前，均进行去重校验。
 *
 * 时间复杂度: $O(N + M)$，其中 $N$ 和 $M$ 分别为两个输入数组的长度。
 * 空间复杂度: $O(N + M)$，用于存储结果并集。
 *
 * @note
 * 【已排序前置断言防卫】：
 * 1. **已排序强校验**：本双指针归并算法极度依赖输入的有序性。如果输入数组未排序，并集结果会缺失、错序或包含未去重元素。
 *    **修复**：在 `get_union` 中加入 `std::is_sorted` 断言防御性检查，以提早暴露输入错误。
 * 2. **去重保证**：通过在插入前检查 `res.empty() || next != res.back()` 确保并集里的每个元素均为唯一。
 *
 * @see intersection_of_two_arrays.cpp
 * @author [Alvin](https://github.com/polarvoid)
 */

#include <algorithm>  
#include <cassert>    
#include <iostream>   
#include <vector>     

namespace operations_on_datastructures {

/**
 * @brief 顺序打印向量中的所有元素
 * @param array 待打印的数组引用
 */
void print(const std::vector<int32_t> &array) {
    for (int32_t i : array) {
        std::cout << i << " ";  
    }
    std::cout << "\n";  
}

/**
 * @brief 获取两个已排序数组的并集并按升序返回
 * @param first 第一个已排序数组
 * @param second 第二个已排序数组
 * @return 包含无重复并集元素的升序向量
 */
std::vector<int32_t> get_union(const std::vector<int32_t> &first,
                               const std::vector<int32_t> &second) {
    // 核心修复：防卫性校验，确保输入数组必须为已排序状态
    assert(std::is_sorted(first.begin(), first.end()) && "First array must be sorted!");
    assert(std::is_sorted(second.begin(), second.end()) && "Second array must be sorted!");

    std::vector<int32_t> res;         
    size_t f_index = 0;               
    size_t s_index = 0;               
    size_t f_length = first.size();   
    size_t s_length = second.size();  
    int32_t next = 0;  

    while (f_index < f_length && s_index < s_length) {
        if (first[f_index] < second[s_index]) {
            next = first[f_index];  
            f_index++;              
        } else if (first[f_index] > second[s_index]) {
            next = second[s_index];  
            s_index++;               
        } else {
            next = first[f_index];  
            f_index++;              
            s_index++;              
        }
        if ((res.size() == 0) || (next != res.back())) {
            res.push_back(next);  
        }
    }
    
    // 追加第一个数组的剩余元素
    while (f_index < f_length) {
        next = first[f_index];  
        if ((res.size() == 0) || (next != res.back())) {
            res.push_back(next);  
        }
        f_index++;
    }
    
    // 追加第二个数组的剩余元素
    while (s_index < s_length) {
        next = second[s_index];  
        if ((res.size() == 0) || (next != res.back())) {
            res.push_back(next);  
        }
        s_index++;
    }
    return res;
}

}  // namespace operations_on_datastructures

namespace tests {
using operations_on_datastructures::get_union;
using operations_on_datastructures::print;

/**
 * @brief 单元自测用例 1：两个空数组的并集
 */
void test1() {
    std::cout << "TEST CASE 1\n";
    std::cout << "Intialized a = {} b = {}\n";
    std::cout << "Expected result: {}\n";
    std::vector<int32_t> a = {};
    std::vector<int32_t> b = {};
    std::vector<int32_t> result = get_union(a, b);
    assert(result == a);  
    print(result);        
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 单元自测用例 2：一个空数组的并集
 */
void test2() {
    std::cout << "TEST CASE 2\n";
    std::cout << "Intialized a = {} b = {2, 3}\n";
    std::cout << "Expected result: {2, 3}\n";
    std::vector<int32_t> a = {};
    std::vector<int32_t> b = {2, 3};
    std::vector<int32_t> result = get_union(a, b);
    assert(result == b);  
    print(result);        
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 单元自测用例 3：常规并集测试
 */
void test3() {
    std::cout << "TEST CASE 3\n";
    std::cout << "Intialized a = {4, 6} b = {2, 3}\n";
    std::cout << "Expected result: {2, 3, 4, 6}\n";
    std::vector<int32_t> a = {4, 6};
    std::vector<int32_t> b = {2, 3};
    std::vector<int32_t> result = get_union(a, b);
    std::vector<int32_t> expected = {2, 3, 4, 6};
    assert(result == expected);  
    print(result);               
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 单元自测用例 4：包含重复值自动去重的并集测试
 */
void test4() {
    std::cout << "TEST CASE 4\n";
    std::cout << "Intialized a = {4, 6, 6, 7} b = {2, 3, 4}\n";
    std::cout << "Expected result: {2, 3, 4, 6, 7}\n";
    std::vector<int32_t> a = {4, 6, 6, 7};
    std::vector<int32_t> b = {2, 3, 4};
    std::vector<int32_t> result = get_union(a, b);
    std::vector<int32_t> expected = {2, 3, 4, 6, 7};
    assert(result == expected);  
    print(result);               
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 单元自测用例 5：一般复杂并集测试
 */
void test5() {
    std::cout << "TEST CASE 5\n";
    std::cout << "Intialized a = {1, 4, 6, 7, 9} b = {2, 3, 5}\n";
    std::cout << "Expected result: {1, 2, 3, 4, 5, 6, 7, 9}\n";
    std::vector<int32_t> a = {1, 4, 6, 7, 9};
    std::vector<int32_t> b = {2, 3, 5};
    std::vector<int32_t> result = get_union(a, b);
    std::vector<int32_t> expected = {1, 2, 3, 4, 5, 6, 7, 9};
    assert(result == expected);  
    print(result);               
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 单元自测用例 6：使用 std::sort 排序后的数据并集测试
 */
void test6() {
    std::cout << "TEST CASE 6\n";
    std::cout << "Intialized a = {1, 3, 3, 2, 5, 9, 4, 3, 2} ";
    std::cout << "b = {11, 3, 7, 8, 6}\n";
    std::cout << "Expected result: {1, 2, 3, 4, 5, 6, 7, 8, 9, 11}\n";
    std::vector<int32_t> a = {1, 3, 3, 2, 5, 9, 4, 3, 2};
    std::vector<int32_t> b = {11, 3, 7, 8, 6};
    std::sort(a.begin(), a.end());  
    std::sort(b.begin(), b.end());  
    std::vector<int32_t> result = get_union(a, b);
    std::vector<int32_t> expected = {1, 2, 3, 4, 5, 6, 7, 8, 9, 11};
    assert(result == expected);  
    print(result);               
    std::cout << "TEST PASSED!\n\n";
}
}  // namespace tests

static void test() {
    tests::test1();
    tests::test2();
    tests::test3();
    tests::test4();
    tests::test5();
    tests::test6();
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试用例确认正确性
    return 0;
}
