/**
 * @file
 * @brief [Quick sort implementation](https://en.wikipedia.org/wiki/Quicksort) in C++ (快速排序算法实现)
 * @details
 *      快速排序是一种采用“分治法”(Divide and Conquer) 的高效排序算法。
 *      该算法选取一个元素作为基准值 (Pivot)，并通过分区 (Partition) 将数组分为两部分：
 *      所有小于基准值的元素移到基准值左边，所有大于基准值的元素移到右边。
 *      
 *      常见的基准值选取策略包括：
 *      1. 始终选择第一个元素为基准值
 *      2. 始终选择最后一个元素为基准值（本程序所实现的策略）
 *      3. 随机选择一个元素为基准值
 *      4. 选择三数中值 (Median of three) 为基准值
 *
 *      快速排序的核心步骤是分区操作 `partition()`。分区的目标是：
 *      在 $O(n)$ 时间内，将基准值放置在其在排序完成后的最终正确位置上，
 *      使得其左边的所有元素都小于或等于它，右边的所有元素都大于它。
 *
 * @author [David Leal](https://github.com/Panquesito7)
 * @author [popoapp](https://github.com/popoapp)
 */

#include <algorithm>  /// 用于 std::is_sorted
#include <cassert>    /// 用于 std::assert
#include <cstdint>
#include <ctime>      /// 用于 std::time
#include <iostream>   /// 用于输入输出操作
#include <vector>     /// 用于 std::vector

/**
 * @brief 排序算法命名空间
 * @namespace sorting
 */
namespace sorting {
/**
 * @namespace quick_sort
 * @brief 快速排序算法实现的相关函数
 */
namespace quick_sort {
/**
 * @brief 使用最后一个元素作为基准值进行分区
 * @details
 * 该函数选取子数组的最后一个元素为基准值 (pivot)，
 * 然后将所有小于或等于基准值的元素交换到左侧，
 * 最后将基准值与大于它的第一个元素交换，使得基准值处于正确的位置。
 * 
 * @tparam T 数组元素类型
 * @param arr 传入的数组指针
 * @param low 当前子数组的起始索引
 * @param high 当前子数组的结束索引
 * @returns 基准值在分区完成后的最终正确索引位置
 *  
 *  ### 时间复杂度
 *  最好与平均情况：$O(n \log n)$
 *  最坏情况：$O(n^2)$ （当分区持续极度不均衡时，例如数组已完全有序或逆序）
 *  
 *  ### 空间复杂度
 *  平均情况：$O(\log n)$ (由于递归调用栈的深度)
 *  最坏情况：$O(n)$ (单支退化递归栈深度)
 */ 
template <typename T>
int partition(std::vector<T> *arr, const int &low, const int &high) {
    T pivot = (*arr)[high];  // 选取最后一个元素作为基准值
    int i = (low - 1);       // i 指向小于或等于 pivot 的元素边界位置

    for (int j = low; j < high; j++) {
        // 如果当前元素小于或等于基准值
        if ((*arr)[j] <= pivot) {
            i++;  // 扩大较小值区域的右边界
            std::swap((*arr)[i], (*arr)[j]); // 将较小值交换到左侧边界内
        }
    }

    // 最后，将基准值与大于它的第一个元素交换，放回基准值正确的位置
    std::swap((*arr)[i + 1], (*arr)[high]);
    return (i + 1); // 返回基准值的正确索引
}

/**
 * @brief 快速排序的主要递归函数 (原地修改原数组)
 * @tparam T 数组元素类型
 * @param arr 待排序数组的指针
 * @param low 起始索引
 * @param high 结束索引
 */
template <typename T>
void quick_sort(std::vector<T> *arr, const int &low, const int &high) {
    if (low < high) {
        // 1. 进行分区操作，获取基准值位置 p
        int p = partition(arr, low, high);

        // 2. 递归地对基准值左半区间和右半区间进行快速排序
        quick_sort(arr, low, p - 1);
        quick_sort(arr, p + 1, high);
    }
}

/**
 * @brief 快速排序的非破坏性封装函数 (拷贝传值并返回新数组)
 * @tparam T 数组元素类型
 * @param arr 待排序数组的拷贝
 * @param low 起始索引
 * @param high 结束索引
 * @returns 排序完成后的新数组
 */
template <typename T>
std::vector<T> quick_sort(std::vector<T> arr, const int &low, const int &high) {
    if (low < high) {
        int p = partition(&arr, low, high);

        quick_sort(&arr, low, p - 1);
        quick_sort(&arr, p + 1, high);
    }
    return arr;
}

/**
 * @brief 打印数组元素的辅助工具函数
 * @param arr 待打印的数组
 * @param size 数组长度
 */
template <typename T>
void show(const std::vector<T> &arr, const int &size) {
    for (int i = 0; i < size; i++) std::cout << arr[i] << " ";
    std::cout << "\n";
}

}  // namespace quick_sort
}  // namespace sorting

/**
 * @brief 自测用例集
 */
static void tests() {
    // 测试用例 1 (常规正数)
    std::vector<uint64_t> arr = {5, 3, 8, 12, 14, 16, 28, 96, 2, 5977};
    std::vector<uint64_t> arr_sorted = sorting::quick_sort::quick_sort(
        arr, 0, int(std::end(arr) - std::begin(arr)) - 1);

    assert(std::is_sorted(std::begin(arr_sorted), std::end(arr_sorted)));
    std::cout << "\n1st test: passed!\n";

    // 测试用例 2 (包含负数与正数)
    std::vector<int64_t> arr2 = {9,    15,   28,   96,  500, -4, -58,
                                 -977, -238, -800, -21, -53, -55};
    std::vector<int64_t> arr_sorted2 = sorting::quick_sort::quick_sort(
        arr2, 0, (std::end(arr2) - std::begin(arr2)) - 1);

    assert(std::is_sorted(std::begin(arr_sorted2), std::end(arr_sorted2)));
    std::cout << "2nd test: passed!\n";

    // 测试用例 3 (包含浮点数和整数)
    std::vector<double> arr3 = {29,  36,   1100, 0,      77,     1,
                                6.7, 8.97, 1.74, 950.10, -329.65};
    std::vector<double> arr_sorted3 = sorting::quick_sort::quick_sort(
        arr3, 0, int(std::end(arr3) - std::begin(arr3)) - 1);

    assert(std::is_sorted(std::begin(arr_sorted3), std::end(arr_sorted3)));
    std::cout << "3rd test: passed!\n";

    // 测试用例 4 (大批量随机浮点数和负数)
    size_t size = std::rand() % 750 + 100;

    std::vector<float> arr4(size);
    for (uint64_t i = 0; i < size; i++) {
        arr4[i] = static_cast<float>(std::rand()) /
                      static_cast<float>(RAND_MAX / 999.99 - 0.99) -
                  250;
    }

    std::vector<float> arr4_sorted = sorting::quick_sort::quick_sort(
        arr4, 0, int(std::end(arr4) - std::begin(arr4)) - 1);
    assert(std::is_sorted(std::begin(arr4_sorted), std::end(arr4_sorted)));

    std::cout << "4th test: passed!\n";

    // 打印所有的排序后数组结果
    std::cout << "\n\tPrinting all sorted arrays:\t\n";

    std::cout << "1st array:\n";
    sorting::quick_sort::show(arr_sorted, std::end(arr) - std::begin(arr));
    std::cout << std::endl;
    std::cout << "2nd array:\n";
    sorting::quick_sort::show(arr_sorted2, std::end(arr2) - std::begin(arr2));
    std::cout << std::endl;
    std::cout << "3rd array:\n";
    sorting::quick_sort::show(arr_sorted3, int(std::end(arr3) - std::begin(arr3)));
    std::cout << std::endl;
    std::cout << "Start: 4th array:\n\n";
    sorting::quick_sort::show(
        arr4_sorted, int(std::end(arr4_sorted) - std::begin(arr4_sorted)));
    std::cout << "\nEnd: 4th array.\n";
}

/**
 * @brief 主函数，提供模式选择：1. 自测模式，2. 交互式用户输入模式
 * @returns 0
 */
int main() {
    int choice = 0;

    std::cout << "\tAvailable modes\t\n\n";
    std::cout << "1. Self-tests mode\n2. Interactive mode";

    std::cout << "\nChoose a mode: ";
    std::cin >> choice;
    std::cout << "\n";

    while ((choice != 1) && (choice != 2)) {
        std::cout << "Invalid option. Choose between the valid modes: ";
        std::cin >> choice;
    }

    if (choice == 1) {
        std::srand(std::time(nullptr));
        tests();  // 运行自测集
    } else if (choice == 2) {
        int size = 0;
        std::cout << "\nEnter the number of elements: ";

        std::cin >> size;
        std::vector<float> arr(size);

        std::cout
            << "\nEnter the unsorted elements (can be negative/decimal): ";

        for (int i = 0; i < size; ++i) {
            std::cout << "\n";
            std::cin >> arr[i];
        }
        sorting::quick_sort::quick_sort(&arr, 0, size - 1);
        std::cout << "\nSorted array: \n";
        sorting::quick_sort::show(arr, size);
    }
    return 0;
}
