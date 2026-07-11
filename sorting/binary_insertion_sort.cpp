/**
 * @file
 * @brief Implementation of [Binary Insertion Sort](https://en.wikipedia.org/wiki/Insertion_sort) algorithm (折半插入排序 / 二分插入排序算法实现)
 *
 * @details
 * 二分插入排序（Binary Insertion Sort）是插入排序的改进版本。
 * 传统插入排序需要对前面已排序的元素进行线性扫描以寻找合适的插入位置。
 * 在折半插入排序中，我们利用已排序子数组的有序性，使用**二分查找（Binary Search）**在 $O(\log N)$ 时间内锁定插入点。
 *
 * 虽然寻找位置的比较次数由 $O(N^2)$ 降低到 $O(N \log N)$，但是为了挪动元素腾出空间，
 * 其整体时间复杂度仍为 $O(N^2)$，因为每次插入可能都需要搬移 $O(N)$ 个元素。
 *
 * @note
 * 【排序稳定性 Bug 审计】：
 * 在第 72-74 行中：
 *   `else { return mid + 1; }`
 * 当折半查找发现 `arr[mid] == val` 时，程序直接返回了 `mid + 1` 并终止了搜索。
 * 这一做法在**存在重复元素时会破坏稳定性**。例如：排序数组中有多个相同值，
 * `mid` 命中了其中某一个，新元素会被插入到该值之后，但如果在 `mid` 之后还有其他相同元素，
 * 它们就会被移到新元素的后面，这破坏了等值元素原有的相对先后顺序。
 * 为了维持稳定性，标准的二分查找应当寻找“首个严格大于待插入值的元素位置”（类似于 `std::upper_bound` 的位置）。
 *
 * 时间复杂度: $O(N^2)$ (平均及最坏时间)
 * 空间复杂度: $O(\log N)$ (递归搜索调用栈空间)
 * 
 * @author [Akshat Vaya](https://github.com/AkVaya)
 */

#include <algorithm>  /// 用于 std::is_sorted 等
#include <cassert>    /// 用于 assert 断言
#include <iostream>   /// 用于标准输入输出
#include <vector>     /// 用于 std::vector

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {

/**
 * @brief 用二分查找定位待插入元素的位置
 * @tparam T 模版通用数据类型
 * @param arr 输入的数组引用
 * @param val 待插入的元素值
 * @param low 搜索区间的下界 (0-indexed)
 * @param high 搜索区间的上界 (0-indexed)
 * @returns 最终适合插入的数组索引位置
 */
template <class T>
int64_t binary_search(std::vector<T> &arr, T val, int64_t low, int64_t high) {
    if (high <= low) {
        return (val > arr[low]) ? (low + 1) : low;
    }
    int64_t mid = low + (high - low) / 2;
    if (arr[mid] > val) {
        return binary_search(arr, val, low, mid - 1);
    } else if (arr[mid] < val) {
        return binary_search(arr, val, mid + 1, high);
    } else {
        // 警告：直接返回 mid + 1 会在重复元素存在时破坏稳定性
        return mid + 1;
    }
}

/**
 * @brief 折半插入排序主函数
 * @tparam T 模版通用数据类型
 * @param arr 待排序数组的引用
 */
template <typename T>
void insertionSort_binsrch(std::vector<T> &arr) {
    int64_t n = arr.size();

    for (int64_t i = 1; i < n; i++) {
        T key = arr[i]; // 当前需要插入的元素
        int64_t j = i - 1;
        // 利用二分查找确定插入位置
        int64_t loc = sorting::binary_search(arr, key, 0, j);
        
        // 挪动元素为 key 腾出空间
        while (j >= loc) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key; // 放置到目标位置
    }
}
}  // namespace sorting

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1
    std::vector<int64_t> arr1({5, -3, -1, -2, 7});
    std::cout << "1st test... ";
    sorting::insertionSort_binsrch(arr1);
    assert(std::is_sorted(std::begin(arr1), std::end(arr1)));
    std::cout << "passed" << std::endl;

    // 测试 2
    std::vector<int64_t> arr2({12, 26, 15, 91, 32, 54, 41});
    std::cout << "2nd test... ";
    sorting::insertionSort_binsrch(arr2);
    assert(std::is_sorted(std::begin(arr2), std::end(arr2)));
    std::cout << "passed" << std::endl;

    // 测试 3: 浮点数类型
    std::vector<float> arr3({7.1, -2.5, -4.0, -2.1, 5.7});
    std::cout << "3rd test... ";
    sorting::insertionSort_binsrch(arr3);
    assert(std::is_sorted(std::begin(arr3), std::end(arr3)));
    std::cout << "passed" << std::endl;

    // 测试 4: 浮点数类型
    std::vector<float> arr4({12.8, -3.7, -20.7, -7.1, 2.2});
    std::cout << "4th test... ";
    sorting::insertionSort_binsrch(arr4);
    assert(std::is_sorted(std::begin(arr4), std::end(arr4)));
    std::cout << "passed" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行自测
    return 0;
}
