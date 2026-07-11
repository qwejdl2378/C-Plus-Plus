/**
 * @file
 * @brief Implementation for the [Intersection of two sorted Arrays](https://en.wikipedia.org/wiki/Intersection_(set_theory)) algorithm (两个已排序数组的交集算法实现)
 *
 * @details
 * 两个数组的交集定义为同时属于这两个数组的元素集合。
 * 本实现要求输入的两个数组必须是已升序排序的。算法通过双指针法以线性时间复杂度扫描两数组并提取公共元素。
 * 在提取过程中，通过判断当前元素是否与结果集末尾元素相同来自动去重，从而保证输出的交集元素唯一。
 *
 * 时间复杂度: $O(N + M)$，其中 $N$ 和 $M$ 分别为两个输入数组的长度。
 * 空间复杂度: $O(\min(N, M))$，用于存储结果交集。
 *
 * @note
 * 【排序先决条件与去重审计】：
 * 1. **双指针排序前提条件**：双指针法依赖于输入数组的单调性，若传入未排序数组会导致计算交集遗漏或错误。
 *    我们在实现中加入 `std::is_sorted` 防护性断言以保证输入数据的正确性。
 * 2. **去重策略**：当检测到两个指针指向的元素相等时，只有在结果集为空，或当前元素不等于结果集末尾元素时才将其加入，从而完美解决重复元素的去重。
 *
 * @see union_of_two_arrays.cpp
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
 * @brief 计算两个已排序数组的唯一交集
 * @param first 第一个已升序排序的数组
 * @param second 第二个已升序排序的数组
 * @return 包含唯一交集元素的升序数组
 */
std::vector<int32_t> get_intersection(const std::vector<int32_t> &first,
                                      const std::vector<int32_t> &second) {
    // 核心修复：防卫性校验，确保输入数组必须为已排序状态
    assert(std::is_sorted(first.begin(), first.end()) && "First array must be sorted!");
    assert(std::is_sorted(second.begin(), second.end()) && "Second array must be sorted!");

    std::vector<int32_t> res;         
    size_t f_index = 0;               
    size_t s_index = 0;               
    size_t f_length = first.size();   
    size_t s_length = second.size();  

    while (f_index < f_length && s_index < s_length) {
        if (first[f_index] < second[s_index]) {
            f_index++;  
        } else if (first[f_index] > second[s_index]) {
            s_index++;  
        } else {
            // 当元素相等时，判断是否已存在于结果集中以达到去重效果
            if ((res.size() == 0) || (first[f_index] != res.back())) {
                res.push_back(first[f_index]);  
            }
            f_index++;  
            s_index++;  
        }
    }
    return res;
}

}  // namespace operations_on_datastructures

namespace tests {
using operations_on_datastructures::get_intersection;
using operations_on_datastructures::print;

/**
 * @brief 单元自测用例 1：两个空数组交集测试
 */
void test1() {
    std::cout << "TEST CASE 1\n";
    std::cout << "Intialized a = {} b = {}\n";
    std::cout << "Expected result: {}\n";
    std::vector<int32_t> a = {};
    std::vector<int32_t> b = {};
    std::vector<int32_t> result = get_intersection(a, b);
    assert(result == a);  
    print(result);        
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 单元自测用例 2：一个空数组交集测试
 */
void test2() {
    std::cout << "TEST CASE 2\n";
    std::cout << "Intialized a = {} b = {2, 3}\n";
    std::cout << "Expected result: {}\n";
    std::vector<int32_t> a = {};
    std::vector<int32_t> b = {2, 3};
    std::vector<int32_t> result = get_intersection(a, b);
    assert(result == a);  
    print(result);        
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 单元自测用例 3：常规无重复元素交集测试
 */
void test3() {
    std::cout << "TEST CASE 3\n";
    std::cout << "Intialized a = {4, 6} b = {3, 6}\n";
    std::cout << "Expected result: {6}\n";
    std::vector<int32_t> a = {4, 6};
    std::vector<int32_t> b = {3, 6};
    std::vector<int32_t> result = get_intersection(a, b);
    std::vector<int32_t> expected = {6};
    assert(result == expected);  
    print(result);               
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 单元自测用例 4：包含重复值交集自动去重测试
 */
void test4() {
    std::cout << "TEST CASE 4\n";
    std::cout << "Intialized a = {4, 6, 6, 6} b = {2, 4, 4, 6}\n";
    std::cout << "Expected result: {4, 6}\n";
    std::vector<int32_t> a = {4, 6, 6, 6};
    std::vector<int32_t> b = {2, 4, 4, 6};
    std::vector<int32_t> result = get_intersection(a, b);
    std::vector<int32_t> expected = {4, 6};
    assert(result == expected);  
    print(result);               
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 单元自测用例 5：一般交集测试
 */
void test5() {
    std::cout << "TEST CASE 5\n";
    std::cout << "Intialized a = {1, 2, 3, 4, 6, 7, 9} b = {2, 3, 4, 5}\n";
    std::cout << "Expected result: {2, 3, 4}\n";
    std::vector<int32_t> a = {1, 2, 3, 4, 6, 7, 9};
    std::vector<int32_t> b = {2, 3, 4, 5};
    std::vector<int32_t> result = get_intersection(a, b);
    std::vector<int32_t> expected = {2, 3, 4};
    assert(result == expected);  
    print(result);               
    std::cout << "TEST PASSED!\n\n";
}

/**
 * @brief 单元自测用例 6：使用 std::sort 预先排序的数据测试
 */
void test6() {
    std::cout << "TEST CASE 6\n";
    std::cout << "Intialized a = {1, 3, 3, 2, 5, 9, 4, 7, 3, 2} ";
    std::cout << "b = {11, 3, 7, 8, 6}\n";
    std::cout << "Expected result: {3, 7}\n";
    std::vector<int32_t> a = {1, 3, 3, 2, 5, 9, 4, 7, 3, 2};
    std::vector<int32_t> b = {11, 3, 7, 8, 6};
    std::sort(a.begin(), a.end());  
    std::sort(b.begin(), b.end());  
    std::vector<int32_t> result = get_intersection(a, b);
    std::vector<int32_t> expected = {3, 7};
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
