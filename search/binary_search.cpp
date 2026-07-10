/******************************************************************************
 * @file
 * @brief [Binary search algorithm](https://en.wikipedia.org/wiki/Binary_search_algorithm) (二分查找算法实现)
 * @details
 * 二分查找是一种在有序数组中查找特定元素的搜索算法。
 * 类似于在字典中查词：我们首先将目标值与数组的中间元素进行比较。
 * 如果相等，则直接返回其索引；如果不相等，由于数组是有序的，我们可以将不可能包含目标值的那一半区间直接排除，
 * 并在剩余的半个区间内继续重复该过程，直到找到目标值。
 * 如果搜索区间收缩为空，说明目标值不存在于数组中。
 *
 * ### 实现要求
 *
 * 二分查找的前提是原数组必须是有序的。
 *
 * ### 复杂度分析
 *
 * 令 n 为数组中元素的个数。
 *
 * 最坏时间复杂度：O(log n)
 * 最好时间复杂度：O(1)
 * 平均时间复杂度：O(log n)
 * 空间复杂度：O(1) (只需要常数级别的辅助变量)
 *
 * @author [Lajat Manekar](https://github.com/Lazeeez)
 * @author Unknown author
 *******************************************************************************/

#include <algorithm>  /// 用于 std::sort 排序函数
#include <cassert>    /// 用于 std::assert
#include <cstdint>
#include <iostream>   /// 用于输入输出操作
#include <vector>     /// 用于 std::vector

/******************************************************************************
 * @namespace search
 * @brief 搜索算法命名空间
 *******************************************************************************/
namespace search {

/******************************************************************************
 * @namespace binary_search
 * @brief 二分查找实现命名空间
 *******************************************************************************/
namespace binary_search {

/******************************************************************************
 * @brief 二分查找核心实现函数
 * @param arr 已经排好序的待查找向量
 * @param val 需要检索的目标数值
 * @returns 目标数值在向量中的索引位置，若未找到则返回 uint64_t 类型的最大值 (-1)
 *******************************************************************************/
uint64_t binarySearch(std::vector<uint64_t> arr, uint64_t val) {
    uint64_t low = 0;                // 检索区间的左端点
    uint64_t high = arr.size() - 1;  // 检索区间的右端点

    while (low <= high) {
        // 计算中点位置，采用 (low + (high - low) / 2) 可以有效防止 (low + high) 发生整型溢出
        uint64_t m = low + (high - low) / 2;  

        // 找到目标值，直接返回其索引
        if (val == arr[m]) {
            return m;
        } 
        // 如果目标值小于中点值，说明目标只可能存在于左半边，收缩右端点
        else if (val < arr[m]) {
            high = m - 1;
        } 
        // 如果目标值大于中点值，说明目标只可能存在于右半边，收缩左端点
        else {
            low = m + 1;
        }
    }
    return -1;  // 若未找到，返回 -1 (在无符号类型下表现为 18446744073709551615)
}

}  // namespace binary_search

}  // namespace search

/*******************************************************************************
 * @brief 自测用例 1
 *******************************************************************************/
static void test1() {
    // 测试用例 1
    // 输入数组 arr = [1,3,5,7,9,8,6,4,2]，查找 4
    // 排序后 arr = [1,2,3,4,5,6,7,8,9]，值 4 对应的排序后索引应为 3

    std::vector<uint64_t> arr = {{1, 3, 5, 7, 9, 8, 6, 4, 2}};
    std::sort(arr.begin(), arr.end()); // 排序
    uint64_t expected_ans = 3;
    uint64_t derived_ans = search::binary_search::binarySearch(arr, 4);
    std::cout << "Test #1: ";
    assert(derived_ans == expected_ans);
    std::cout << "Passed!" << std::endl;
}

/*******************************************************************************
 * @brief 自测用例 2
 *******************************************************************************/
void test2() {
    // 测试用例 2
    // 输入数组 arr = [1,23,25,4,2]，查找 25
    // 排序后为 [1,2,4,23,25]，值 25 对应的排序后索引应为 4
    std::vector<uint64_t> arr = {{1, 23, 25, 4, 2}};
    std::sort(arr.begin(), arr.end());
    uint64_t expected_ans = 4;
    uint64_t derived_ans = search::binary_search::binarySearch(arr, 25);
    std::cout << "Test #2: ";
    assert(derived_ans == expected_ans);
    std::cout << "Passed!" << std::endl;
}

/*******************************************************************************
 * @brief 自测用例 3
 *******************************************************************************/
void test3() {
    // 测试用例 3
    // 输入数组 arr = [1,31,231,12,2,5,51,21,23,12,3]，查找 31
    // 排序后为 [1,2,3,5,12,12,21,23,31,51,231]，值 31 对应的排序后索引应为 8
    std::vector<uint64_t> arr = {{1, 31, 231, 12, 2, 5, 51, 21, 23, 12, 3}};
    std::sort(arr.begin(), arr.end());
    uint64_t expected_ans = 8;
    uint64_t derived_ans = search::binary_search::binarySearch(arr, 31);
    std::cout << "Test #3: ";
    assert(derived_ans == expected_ans);
    std::cout << "Passed!" << std::endl;
}

/*******************************************************************************
 * @brief 主函数
 * @returns 0
 *******************************************************************************/
int main() {
    test1();  // 运行自测用例 1
    test2();  // 运行自测用例 2
    test3();  // 运行自测用例 3

    return 0;
}
