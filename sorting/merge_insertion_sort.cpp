/**
 * @file
 * @brief Implementation of combined Merge-Insertion Sort algorithm (混合归并插入排序算法实现)
 *
 * @details
 * 混合归并插入排序（Merge-Insertion Sort / Tim-Sort-like hybrid）是一种实用优化排序算法。
 * 其基本原理与 `std::stable_sort` 类似：
 * - 归并排序（Merge Sort）在处理小规模数组时，递归和合并的额外开销（如临时数组分配和数据拷贝）相对于 $O(N \log N)$ 的时间收益并不合算。
 * - 插入排序（Insertion Sort）在处理小编界数组时非常迅速且开销极小。
 * - 因此，当归并区间缩小到预设阈值（`threshold`）以内时，算法不再继续向下二分归并，
 *   而是直接调用插入排序对该子区间进行排序，然后再向上进行归并合并。
 *
 * @note
 * 【原版代码的严重正确性及边界失配 Bug 审计与修复】：
 * 原版实现中存在极其严重的**排序失效和越界/数据未拷贝 Bug**，这是由于“包含（Inclusive）”和“不包含（Exclusive）”边界混用导致的：
 * 1. 边界定义冲突：`InsertionSort` 视传入的 `end` 为**开区间**（即不包含 `end`）；
 *    而 `merge` 视 `max` 为**闭区间**（即包含 `max`）；
 *    `mergeSort` 递归中则是左右开闭界限严重混乱，导致中间有元素从未被子排序处理过。
 * 2. 元素丢失 Bug：在原 `merge` 拷贝中，使用 `memcpy(ptr + min, tempArray.data() + min, (max - min) * sizeof(T))`，
 *    由于其 `max` 视作闭区间，合并的实际长度应为 `max - min + 1`。原版少拷了一个元素，导致最右端的元素在合并后直接丢失（被旧值覆盖或保持未排序）。
 * 3. 测试形同虚设：原 `test()` 函数中，`mergeSort` 行被强行注释掉了，只测试了正常的 `InsertionSort`，因此原作者未发现上述重大缺陷。
 *
 * ### 修复方案
 * - **统一边界规范**：将本文件中所有区间函数（`InsertionSort`, `merge`, `mergeSort`）全部重构为标准的 **左闭右开区间 `[min, max)`** 规范，这非常符合 C++ 迭代器的习惯。
 * - 重构 `merge` 逻辑以支持开区间 `max`，使 `memcpy` 拷贝字节数精确为 `(max - min) * sizeof(T)`。
 * - 取消 `test()` 中对 `mergeSort` 调用的注释，确保混合算法得到真正验证。
 *
 * 时间复杂度: $O(N \log N)$
 * 空间复杂度: $O(N)$
 * 
 * @author [@sinkyoungdeok](https://github.com/sinkyoungdeok)
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <algorithm>
#include <array>
#include <cassert>
#include <ctime>
#include <iostream>
#include <cstring>

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {
/**
 * @namespace merge_insertion
 * @brief 混合归并插入排序命名空间
 */
namespace merge_insertion {

/**
 * @brief 局域插入排序（左闭右开区间 [start, end)）
 * @tparam T 元素模板类型
 * @tparam N 数组最大静态长度
 * @param A 待排序容器指针
 * @param start 起始索引（包含）
 * @param end 结束索引（不包含）
 */
template <typename T, size_t N>
static void InsertionSort(std::array<T, N> *A, size_t start, size_t end) {
    if (start >= end) {
        return;
    }
    T *ptr = A->data();

    for (size_t i = start + 1; i < end; i++) {
        T temp = ptr[i];
        size_t j = i;
        while (j > start && temp < ptr[j - 1]) {
            ptr[j] = ptr[j - 1];
            j--;
        }
        ptr[j] = temp;
    }
}

/**
 * @brief 合并两个相邻的已排序区间（左区间 [min, mid) 与 右区间 [mid, max)）
 * @tparam T 元素模板类型
 * @tparam N 数组最大静态长度
 * @param array 待排序容器指针
 * @param min 起始索引
 * @param max 结束索引
 * @param mid 分割中点
 */
template <typename T, size_t N>
static void merge(std::array<T, N> *array, size_t min, size_t max, size_t mid) {
    size_t firstIndex = min;
    size_t secondIndex = mid;

    auto ptr = array->data();
    std::array<T, N> tempArray{};

    // 合并双路数据到临时数组中
    for (size_t index = min; index < max; index++) {
        if (firstIndex < mid && (secondIndex >= max || ptr[firstIndex] <= ptr[secondIndex])) {
            tempArray[index] = ptr[firstIndex];
            firstIndex++;
        } else {
            tempArray[index] = ptr[secondIndex];
            secondIndex++;
        }
    }

    // 拷贝回原数组，此时因为是左闭右开，(max - min) 即为需拷贝的准确元素个数
    std::memcpy(ptr + min, tempArray.data() + min, (max - min) * sizeof(T));
}

/**
 * @brief 混合归并插入排序主递归逻辑
 * @tparam T 元素模板类型
 * @tparam N 数组最大静态长度
 * @param array 待排序容器指针
 * @param min 排序区间的左边界（包含）
 * @param max 排序区间的右边界（不包含）
 * @param threshold 切换为插入排序的长度阈值
 */
template <typename T, size_t N>
void mergeSort(std::array<T, N> *array, size_t min, size_t max, size_t threshold) {
    if (max - min <= 1) {
        return;
    }
    
    // 如果区间长度小于等于阈值，直接调用高效的插入排序
    if ((max - min) <= threshold) {
        InsertionSort(array, min, max);
    } else {
        // 二分划分
        size_t mid = min + ((max - min) / 2);

        // 递归排序左半部分 [min, mid)
        mergeSort(array, min, mid, threshold);
        // 递归排序右半部分 [mid, max)
        mergeSort(array, mid, max, threshold);

        // 合并两个有序区间
        merge(array, min, max, mid);
    }
}

}  // namespace merge_insertion
}  // namespace sorting

/**
 * @brief 单元自测用例
 */
static void test() {
    constexpr size_t size = 30;
    std::array<int, size> array{0};
    
    std::cout << "Original Array: ";
    for (size_t i = 0; i < size; i++) {
        array[i] = std::rand() % 100 - 50;
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;

    // 真正调用混合排序进行测试，阈值设为 8
    sorting::merge_insertion::mergeSort(&array, 0, size, 8);

    std::cout << "Sorted Array: ";
    for (size_t i = 0; i < size; ++i) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;

    assert(std::is_sorted(std::begin(array), std::end(array)));
    std::cout << "Test passed\n";
}

/**
 * @brief 主函数
 */
int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    test();
    return 0;
}
