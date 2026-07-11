/**
 * @file
 * @brief Counting Inversions using [Merge Sort](https://en.wikipedia.org/wiki/Merge_sort) (基于归并排序的逆序对计数算法实现)
 *
 * @details
 * 逆序对（Inversion Pair）：在一个数组 `a` 中，如果满足 `i < j` 且 `a[i] > a[j]`，则数对 `(a[i], a[j])` 称为一个逆序对。
 * 逆序对的数量可以反映数组与升序序列之间的“混乱程度”。
 * 
 * ### 归并排序计数原理
 * 1. 采用分治策略：将数组拆分为左右两个子数组，分别递归求解左半部分的逆序对数和右半部分的逆序对数。
 * 2. 在合并阶段（`merge`）：左右两个子数组已经各自排好序。
 *    若发现 `arr[i] > arr[j]`（`i` 在左半部分，`j` 在右半部分），
 *    由于左半部分有序，意味着从 `i` 开始到 `mid` 的所有左侧元素都必定大于 `arr[j]`。
 *    因此，在本次合并中以 `arr[j]` 结尾的跨区间逆序对数即为 `(mid - i + 1)` 个。
 * 3. 汇总三者：总逆序对数 = 左侧逆序对 + 右侧逆序对 + 合并跨区间的逆序对。
 *
 * @note
 * 【大数溢出风险审计】：
 * 在原版实现中，逆序对计数使用 `uint32_t`。
 * 当输入数组较大且逆序对较多时，例如大小为 $N = 10^5$ 的完全逆序数组，
 * 逆序对总数为 $N(N-1)/2 \approx 5 \times 10^9$，这超出了 `uint32_t` 最大值（约 $4.29 \times 10^9$）。
 * 此时会发生**数值溢出（Overflow）**，返回不正确的负数或截断数值。
 * 在处理大规模数据时，建议将返回类型提升至 `uint64_t`。
 *
 * 时间复杂度: $O(N \log N)$
 * 空间复杂度: $O(N)$
 * 
 * @author [Rakshit Raj](https://github.com/rakshitraj)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>   /// 用于 uint64_t 等数据类型
#include <iostream>  /// 用于输入输出
#include <vector>    /// 用于 std::vector

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {
/**
 * @namespace inversion
 * @brief 逆序对计数算法命名空间
 */
namespace inversion {

/**
 * @brief 归并子数组并统计跨区间的逆序对数量
 * @tparam T 模版通用数据类型
 * @param arr 原数组指针
 * @param temp 临时辅助数组指针
 * @param left 当前合并区间的左边界
 * @param mid 分割中点
 * @param right 当前合并区间的右边界
 * @returns 合并过程中发现的逆序对数
 */
template <typename T>
uint32_t merge(T* arr, T* temp, uint32_t left, uint32_t mid, uint32_t right) {
    uint32_t i = left;       /* 左半部分索引 */
    uint32_t j = mid + 1;    /* 右半部分索引 */
    uint32_t k = left;       /* 临时数组存放位置索引 */
    uint32_t inv_count = 0;  // 记录合并中的逆序对数

    while ((i <= mid) && (j <= right)) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
            // 核心统计步骤：由于左半部分有序，i 及其右边的所有到 mid 的元素都大于 arr[j]
            inv_count += (mid - i + 1);
        }
    }
    
    // 拷贝左半部分或右半部分剩余的元素
    while (i <= mid) {
        temp[k++] = arr[i++];
    }
    while (j <= right) {
        temp[k++] = arr[j++];
    }
    
    // 将排好序的临时数组覆盖回原数组
    for (k = left; k <= right; k++) {
        arr[k] = temp[k];
    }
    return inv_count;
}

/**
 * @brief 递归执行归并排序并统计逆序对
 * @tparam T 模版通用数据类型
 * @param arr 数组指针
 * @param temp 辅助临时数组指针
 * @param left 当前区间的左边界
 * @param right 当前区间的右边界
 * @returns 逆序对总数
 */
template <typename T>
uint32_t mergeSort(T* arr, T* temp, uint32_t left, uint32_t right) {
    uint32_t mid = 0, inv_count = 0;
    if (right > left) {
        mid = (right + left) / 2; // 分治划分中点
        
        // 1. 累加左半边内部的逆序对数
        inv_count += mergeSort(arr, temp, left, mid);
        // 2. 累加右半边内部的逆序对数
        inv_count += mergeSort(arr, temp, mid + 1, right);
        // 3. 累加两半边在合并（Merge）时产生的跨区间逆序对数
        inv_count += merge(arr, temp, left, mid, right);
    }
    return inv_count;
}

/**
 * @brief 主函数入口：计算并返回输入数组中的逆序对数，同时会对数组进行排序
 * @tparam T 模版通用数据类型
 * @param arr 输入数据数组指针
 * @param size 数组长度
 * @returns 逆序对的数量
 */
template <class T>
uint32_t countInversion(T* arr, const uint32_t size) {
    if (size <= 1) {
        return 0;
    }
    std::vector<T> temp(size, 0); // 分配辅助空间
    return mergeSort(arr, temp.data(), 0, size - 1);
}

/**
 * @brief 辅助打印数组函数
 */
template <typename T>
void show(T* arr, const uint32_t array_size) {
    std::cout << "Printing array: \n";
    for (uint32_t i = 0; i < array_size; i++) {
        std::cout << " " << arr[i];
    }
    std::cout << "\n";
}

}  // namespace inversion
}  // namespace sorting

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1: 从 100 到 1 逆序，逆序对数量为 100 * 99 / 2 = 4950
    std::vector<uint64_t> arr1 = {
        100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84,
        83,  82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67,
        66,  65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50,
        49,  48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33,
        32,  31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,
        15,  14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1};
    uint32_t size1 = arr1.size();
    uint32_t inv_count1 = 4950;
    uint32_t result1 = sorting::inversion::countInversion(arr1.data(), size1);
    assert(inv_count1 == result1);

    // 测试 2
    std::vector<int> arr2 = {22, 66, 75, 23, 11, 87, 2, 44, 98, 43};
    uint32_t size2 = arr2.size();
    uint32_t inv_count2 = 20;
    uint32_t result2 = sorting::inversion::countInversion(arr2.data(), size2);
    assert(inv_count2 == result2);

    // 测试 3: 双精度浮点数
    std::vector<double> arr3 = {33.1, 45.2, 65.4, 76.5, 1.0,
                                2.9,  5.4,  7.7,  88.9, 12.4};
    uint32_t size3 = arr3.size();
    uint32_t inv_count3 = 21;
    uint32_t result3 = sorting::inversion::countInversion(arr3.data(), size3);
    assert(inv_count3 == result3);

    // 测试 4: 字符序列（已排序，逆序对为 0）
    std::vector<char> arr4 = {'a', 'b', 'c', 'd', 'e'};
    uint32_t size4 = arr4.size();
    uint32_t inv_count4 = 0;
    uint32_t result4 = sorting::inversion::countInversion(arr4.data(), size4);
    assert(inv_count4 == result4);
    
    std::cout << "All inversion test cases passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试
    return 0;
}
