/**
 * @file
 * @brief Implementation of 3-Way Partition Quick Sort (三路划分快速排序算法实现)
 *
 * @details
 * 三路划分快速排序（Quick Sort 3 / 3-Way Quick Sort）是经典快速排序的一项重要改良。
 * 它结合了**荷兰国旗问题（Dutch National Flag Algorithm）**的分流思想：
 * 
 * 在划分（Partition）阶段，传统快排只划分出“小于基准值”和“大于基准值”的两个区间；
 * 而三路快排将数组划分为**三个区间**：
 * 1. 小于基准值（Pivot）的区间：`[low, i]`
 * 2. 等于基准值的区间：`[i+1, j-1]`
 * 3. 大于基准值的区间：`[j, high]`
 *
 * ### 优势场景
 * - 当待排序数组中存在**大量的重复元素（Duplicate Keys）**时，传统快排会面临巨大的多余划分和比较开销；
 * - 三路快排在单趟划分后，可以直接将所有与 Pivot 相等的重复元素一步归位，且在随后的递归中直接跳过这一等值区间。
 * - 对于纯重复元素的区间，三路快排的时间复杂度直接降至 $O(N)$。
 *
 * 时间复杂度: 
 *   - 最好情况: $O(N)$ (当元素全部相同或全部由极少数重复值组成时)
 *   - 最坏情况: $O(N^2)$ (如果每次选择的 Pivot 都是最值)
 *   - 平均情况: $O(N \log N)$
 * 空间复杂度: $O(\log N)$ (递归深度)
 * 
 * @author immortal-j
 * @author [Krishna Vedala](https://github/kvedala)
 */

#include <algorithm>
#include <cassert>
#include <ctime>
#include <iostream>
#include <vector>

namespace {
/**
 * @brief 辅助打印 vector 容器内容重载
 */
template <typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &arr) {
    for (size_t i = 0; i < arr.size(); ++i) {
        out << arr[i];
        if (i < arr.size() - 1) {
            out << ", ";
        }
    }
    return out;
}
}  // namespace

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {
namespace {  // 匿名命名空间，使 partition 辅助函数仅局限在此文件内部

/**
 * @brief 荷兰国旗三路划分核心逻辑
 * @tparam T 元素模板类型
 * @param arr 待排序数组的指针
 * @param low 划分区间的左边界
 * @param high 划分区间的右边界（基准值选为 (*arr)[high]）
 * @param i [out] 输出：小于基准值的最后一个元素的下标
 * @param j [out] 输出：大于基准值的第一个元素的下标
 */
template <typename T>
void partition3(std::vector<T> *arr, int32_t low, int32_t high, int32_t *i,
                int32_t *j) {
    // 基础防线：如果区间仅包含 2 个元素
    if (high - low <= 1) {
        if ((*arr)[high] < (*arr)[low]) {
            std::swap((*arr)[high], (*arr)[low]);
        }
        *i = low;
        *j = high;
        return;
    }

    int32_t mid = low;
    T pivot = (*arr)[high]; // 选择最后一个元素作为基准值

    // 采用三向指针进行荷兰国旗颜色分类
    while (mid <= high) {
        if ((*arr)[mid] < pivot) {
            std::swap((*arr)[low++], (*arr)[mid++]);
        } else if ((*arr)[mid] == pivot) {
            mid++;
        } else if ((*arr)[mid] > pivot) {
            std::swap((*arr)[mid], (*arr)[high--]);
        }
    }

    // 更新输出索引：小于 Pivot 的右边界在 low-1，大于 Pivot 的左边界在 mid
    *i = low - 1;
    *j = mid;
}
}  // namespace

/**
 * @brief 三路划分快速排序（就地排序指针版本）
 * @tparam T 元素模板类型
 * @param arr 待排序数组的指针
 * @param low 排序区间下界
 * @param high 排序区间上界
 */
template <typename T>
void quicksort(std::vector<T> *arr, int32_t low, int32_t high) {
    if (low >= high) {  // 区间仅剩 0 或 1 个元素，无需排序
        return;
    }

    int32_t i = 0, j = 0;

    // 对 arr[low...high] 执行三向划分
    partition3(arr, low, high, &i, &j);

    // 递归对两端非等值区域进行排序，避开已经排好序的中间等值区域 [i+1, j-1]
    quicksort(arr, low, i);
    quicksort(arr, j, high);
}

/**
 * @brief 三路划分快速排序（传值返回副本版本）
 * @tparam T 元素模板类型
 * @param arr 传入的数组容器副本
 * @param low 排序区间下界
 * @param high 排序区间上界
 * @returns 排序完毕的新 vector 数组
 */
template <typename T>
std::vector<T> quicksort(std::vector<T> arr, int32_t low, int32_t high) {
    if (low >= high) {
        return arr;
    }

    int32_t i = 0, j = 0;

    // 直接在局部副本 arr 上调用指针版本，提高递归效率
    partition3(&arr, low, high, &i, &j);

    quicksort(&arr, low, i);
    quicksort(&arr, j, high);

    return arr;
}
}  // namespace sorting

/**
 * @brief 对整型随机数组执行自测
 */
static void test_int() {
    std::cout << "\nTesting integer type arrays\n";

    for (int num_tests = 1; num_tests < 21; num_tests++) {
        size_t size = std::rand() % 500;
        std::vector<int> arr(size);
        for (auto &a : arr) {
            a = std::rand() % 500 - 250;  // 填充 -250 到 249 的随机数
        }

        std::cout << "Test " << num_tests << "\t Array size:" << size << "\t ";
        std::vector<int> sorted = sorting::quicksort(arr, 0, int32_t(size) - 1);
        if (size < 20) {
            std::cout << "\t Sorted Array is:\n\t";
            std::cout << sorted << "\n";
        }
        assert(std::is_sorted(std::begin(sorted), std::end(sorted)));
        std::cout << "\t Passed\n";
    }
}

/**
 * @brief 对浮点型随机数组执行自测
 */
static void test_double() {
    std::cout << "\nTesting Double type arrays\n";
    for (int num_tests = 1; num_tests < 21; num_tests++) {
        size_t size = std::rand() % 500;
        std::vector<double> arr(size);
        for (auto &a : arr) {
            a = double(std::rand() % 500) - 250.f;
            a /= 100.f;  // 转换为 -2.5 到 2.49 的浮点数
        }

        std::cout << "Test " << num_tests << "\t Array size:" << size << "\t ";
        std::vector<double> sorted =
            sorting::quicksort(arr, 0, int32_t(size) - 1);
        if (size < 20) {
            std::cout << "\t Sorted Array is:\n\t";
            std::cout << sorted << "\n";
        }
        assert(std::is_sorted(std::begin(sorted), std::end(sorted)));
        std::cout << "\t Passed\n";
    }
}

/**
 * @brief 主函数
 */
int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    test_int();
    test_double();
    return 0;
}
