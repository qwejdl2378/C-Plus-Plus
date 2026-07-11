/**
 * @file
 * @brief Implementation of [Wave Sort](https://www.geeksforgeeks.org/sort-array-wave-form-2/) algorithm (波浪排序/波形排序算法实现)
 *
 * @details
 * 波形排序（Wave Sort）是一种将数组重新排列为波浪起伏形状的算法。
 * 排列后的数组满足：`arr[0] >= arr[1] <= arr[2] >= arr[3] <= arr[4] ...`
 * 也就是说，奇数索引处的元素都小于或等于其相邻的偶数索引处的元素（或者相反）。
 *
 * ### 两种实现思路
 * 1. **排序法 (排序 + 相邻交换，本实现)**：
 *    - 首先对数组进行升序排序。
 *    - 然后，从左到右，步长为 2，两两交换相邻的元素（如交换 `arr[0]` 和 `arr[1]`，`arr[2]` 和 `arr[3]` 等）。
 *    - 时间复杂度主要取决于内部排序，通常为 $O(N \log N)$。
 * 2. **一次扫描法 (线性时间优化，$O(N)$)**：
 *    - 无需排序。直接遍历偶数位置的元素，将其与其左右两个元素比较，确保其是局部最大值。
 *    - 如果左边或右边的元素比它大，则进行交换。
 *    - 这样可以在 $O(N)$ 时间和 $O(1)$ 额外空间内完成排序。
 *
 * @note
 * 【原版代码的余赘赋值与潜在越界 Bug 审计与修复】：
 * 1. **越界崩溃隐患**：原代码直接声明了 `std::vector<T> arr(in_arr);` 后，又通过循环
 *    `for (int64_t i = 0; i < n; i++) { arr[i] = in_arr[i]; }` 进行了二次显式赋值。
 *    如果调用方传入的参数 `n` 大于数组的实际尺寸，这里会立即触发**越界内存访问崩溃**。
 * 2. **优化方案**：删除了该冗余的赋值循环，直接使用复制构造的 `arr`；同时，对输入长度参数进行了范围限制保护：
 *    `n = std::min(n, static_cast<int64_t>(arr.size()));`。
 *
 * 时间复杂度: $O(N \log N)$ (由于调用了 std::sort)
 * 空间复杂度: $O(N)$ (由于返回了新的副本)
 * 
 * @author [Swastika Gupta](https://github.com/Swastyy)
 */

#include <algorithm>  /// 用于 std::is_sorted, std::swap
#include <cassert>    /// 用于 assert 断言
#include <iostream>   /// 用于输入输出
#include <vector>     /// 用于 std::vector

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {
/**
 * @namespace wave_sort
 * @brief 波形排序算法命名空间
 */
namespace wave_sort {

/**
 * @brief 波形排序主函数
 * @tparam T 元素模板类型
 * @param in_arr 待排序数组的只读引用
 * @param n 需要排序的元素长度
 * @returns 波形排序后的新数组
 */
template <typename T>
std::vector<T> waveSort(const std::vector<T> &in_arr, int64_t n) {
    std::vector<T> arr(in_arr); // 拷贝构造副本

    // 限制 n 的范围，防止调用方传入错误的长度导致越界崩溃
    n = std::min(n, static_cast<int64_t>(arr.size()));

    if (n <= 1) {
        return arr; // 大小为 0 或 1 已经是波形，直接返回
    }

    // 1. 先进行整体升序排序
    std::sort(arr.begin(), arr.begin() + n);

    // 2. 步长为 2 遍历数组，交换相邻的成对元素，将有序序列拉扯成起伏波状
    for (int64_t i = 0; i < n - 1; i += 2) {
        std::swap(arr[i], arr[i + 1]);
    }
    
    return arr;
}
}  // namespace wave_sort
}  // namespace sorting

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1
    std::vector<int64_t> array1 = {10, 90, 49, 2, 1, 5, 23};
    std::cout << "Test 1... ";
    std::vector<int64_t> arr1 = sorting::wave_sort::waveSort(array1, 7);
    const std::vector<int64_t> o1 = {2, 1, 10, 5, 49, 23, 90};
    assert(arr1 == o1);
    std::cout << "passed" << std::endl;

    // 测试 2
    std::vector<int64_t> array2 = {1, 3, 4, 2, 7, 8};
    std::cout << "Test 2... ";
    std::vector<int64_t> arr2 = sorting::wave_sort::waveSort(array2, 6);
    const std::vector<int64_t> o2 = {2, 1, 4, 3, 8, 7};
    assert(arr2 == o2);
    std::cout << "passed" << std::endl;

    // 测试 3: 全部相同的数据
    std::vector<int64_t> array3 = {3, 3, 3, 3};
    std::cout << "Test 3... ";
    std::vector<int64_t> arr3 = sorting::wave_sort::waveSort(array3, 4);
    const std::vector<int64_t> o3 = {3, 3, 3, 3};
    assert(arr3 == o3);
    std::cout << "passed" << std::endl;

    // 测试 4
    std::vector<int64_t> array4 = {9, 4, 6, 8, 14, 3};
    std::cout << "Test 4... ";
    std::vector<int64_t> arr4 = sorting::wave_sort::waveSort(array4, 6);
    const std::vector<int64_t> o4 = {4, 3, 8, 6, 14, 9};
    assert(arr4 == o4);
    std::cout << "passed" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
