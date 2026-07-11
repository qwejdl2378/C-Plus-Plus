/**
 * @file
 * @brief Implementation of iterative [Selection Sort](https://en.wikipedia.org/wiki/Selection_sort) algorithm (迭代版选择排序算法实现)
 *
 * @details
 * 选择排序（Selection Sort）是一种简单直观的排序算法。
 * 它的工作原理如下：
 * 1. 数组被分为“已排序”和“未排序”两部分。初始时已排序部分为空。
 * 2. 在未排序部分中找到最小（或最大）的元素，将其与未排序部分的第一个元素进行交换。
 * 3. 交换后，已排序部分增加一个元素，未排序部分减少一个元素。
 * 4. 重复步骤 2 和 3，直到所有元素都已归入已排序部分。
 *
 * ### 性能与稳定性
 * - 选择排序是**不稳定排序（Unstable Sort）**。因为交换操作可能会改变等值元素的初始相对顺序（例如 `[5, 5, 2]`，把 2 和第一个 5 交换后，两个 5 的相对位置就变了）。
 * - 它的优点是：**数据移动（Swap）次数非常少**，最多为 $N-1$ 次。
 *
 * @note
 * 【C++ 复制粘贴隐患与越界风险审计】：
 * 1. **测试用例变量拷贝错误**：在第 112 行测试 4 中，代码写为 `uint64_t vector4size = vector2.size();`。
 *    这里由于拷贝粘贴失误，错误地使用了 `vector2` 的大小来代表 `vector4` 的大小。
 *    虽然本例中两者大小碰巧相同，但这属于严重的编码安全隐患。已修正为 `vector4.size()`。
 * 2. **大小参数越界风险**：函数签名中接受了显式参数 `len`。若调用方传入的 `len` 大于 `arr.size()`，
 *    循环 `it < len` 会直接越界访问 `array[it]`，导致**内存溢出或崩溃**。
 *    已在函数内添加了安全保护 `len = std::min(len, static_cast<uint64_t>(array.size()));`。
 *
 * 时间复杂度: $O(N^2)$ (最好、最坏、平均)
 * 空间复杂度: $O(N)$ (由于函数返回了一个新的 vector 副本)
 * 
 * @author [Lajat Manekar](https://github.com/Lazeeez)
 */

#include <algorithm>  /// 用于 std::is_sorted, std::min
#include <cassert>    /// 用于 assert 断言
#include <cstdint>   /// 用于 uint64_t
#include <iostream>   /// 用于输入输出
#include <vector>     /// 用于 std::vector

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {

/**
 * @brief 选择排序核心算法（迭代版）
 * @param arr 待排序数组只读引用
 * @param len 排序范围大小
 * @returns 排序完毕的新数组
 */
std::vector<uint64_t> selectionSort(const std::vector<uint64_t> &arr,
                                    uint64_t len) {
    std::vector<uint64_t> array(arr.begin(), arr.end()); // 构造副本

    // 安全防范：防止 len 参数超出实际数组范围导致越界崩溃
    len = std::min(len, static_cast<uint64_t>(array.size()));

    for (uint64_t it = 0; it < len; ++it) {
        uint64_t min = it; // 假定当前未排序区间的首元素为最小值
        
        // 在未排序区间中寻找更小值的索引
        for (uint64_t it2 = it + 1; it2 < len; ++it2) {
            if (array[it2] < array[min]) {
                min = it2; // 记录新的最小值索引
            }
        }

        // 如果最小值不是当前未排序区间的首位，则执行交换
        if (min != it) {
            uint64_t tmp = array[min];
            array[min] = array[it];
            array[it] = tmp;
        }
    }

    return array;
}
}  // namespace sorting

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1
    std::vector<uint64_t> vector1 = {1, 0, 0, 1, 1, 0, 2, 1};
    uint64_t vector1size = vector1.size();
    std::cout << "1st test... ";
    std::vector<uint64_t> result_test1;
    result_test1 = sorting::selectionSort(vector1, vector1size);
    assert(std::is_sorted(result_test1.begin(), result_test1.end()));
    std::cout << "Passed" << std::endl;

    // 测试 2
    std::vector<uint64_t> vector2 = {19, 22, 540, 241, 156, 140, 12, 1};
    uint64_t vector2size = vector2.size();
    std::cout << "2nd test... ";
    std::vector<uint64_t> result_test2;
    result_test2 = sorting::selectionSort(vector2, vector2size);
    assert(std::is_sorted(result_test2.begin(), result_test2.end()));
    std::cout << "Passed" << std::endl;

    // 测试 3
    std::vector<uint64_t> vector3 = {11, 20, 30, 41, 15, 60, 82, 15};
    uint64_t vector3size = vector3.size();
    std::cout << "3rd test... ";
    std::vector<uint64_t> result_test3;
    result_test3 = sorting::selectionSort(vector3, vector3size);
    assert(std::is_sorted(result_test3.begin(), result_test3.end()));
    std::cout << "Passed" << std::endl;

    // 测试 4
    std::vector<uint64_t> vector4 = {1, 9, 11, 546, 26, 65, 212, 14};
    uint64_t vector4size = vector4.size(); // 核心修复：更正为使用 vector4.size()
    std::cout << "4th test... ";
    std::vector<uint64_t> result_test4;
    result_test4 = sorting::selectionSort(vector4, vector4size);
    assert(std::is_sorted(result_test4.begin(), result_test4.end()));
    std::cout << "Passed" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
