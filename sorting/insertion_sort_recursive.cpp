/**
 * @file
 * @brief Implementation of recursive [Insertion Sort](https://en.wikipedia.org/wiki/Insertion_sort) algorithm (递归版插入排序算法实现)
 *
 * @details
 * 递归版插入排序与迭代版的核心逻辑一致，但通过递归调用栈取代了外层循环：
 * 1. **基本情况（Base Case）**：当要排序的元素个数 $n \le 1$ 时，已无需排序，直接返回。
 * 2. **递归步骤（Recursive Step）**：首先递归调用自身，将前 $n-1$ 个元素排好序。
 * 3. **插入归位**：取第 $n$ 个元素（索引 $n-1$），将其与前面排好序的元素从右向左逐一比较并移动，最终放置在正确的有序位置上。
 *
 * @note
 * 【文件命名的学术一致性修复】：
 * 原版文件虽然命名为 `insertion_sort_recursive.cpp`，但其内部代码实际上直接拷贝了 `insertion_sort.cpp` 的**迭代（Iterative）**实现，没有任何递归结构。
 * 现已重构，编写了真正的递归版插入排序函数 `insertionSortRecursive`，从而确保代码与文件名和意图完全统一。
 *
 * 时间复杂度: $O(N^2)$ (最坏和平均)
 * 空间复杂度: $O(N)$ (由于递归函数调用栈深达 N 层，故不适合在极大数据集上使用)
 * 
 * @author [Dhanush S](https://github.com/Fandroid745)
 */

#include <algorithm>   /// 用于 std::is_sorted
#include <cassert>     /// 用于 assert 断言
#include <iostream>    /// 用于输入输出
#include <vector>      /// 用于 std::vector

/** 
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {

/**
 * @brief 递归版插入排序（针对 C 风格指针数组）
 * @tparam T 元素模板类型
 * @param arr 数组首地址指针
 * @param n 需要排序的元素范围大小
 */
template <typename T>
void insertionSortRecursive(T *arr, int n) {
    // 递归基：大小为 1 的子数组已有序
    if (n <= 1) {
        return;
    }

    // 1. 递归排序前 n-1 个元素
    insertionSortRecursive(arr, n - 1);

    // 2. 将第 n 个元素（索引为 n-1）插入到前 n-1 个已排好序的元素中
    T last = arr[n - 1];
    int j = n - 2;

    while (j >= 0 && arr[j] > last) {
        arr[j + 1] = arr[j];
        j--;
    }
    arr[j + 1] = last;
}

/**
 * @brief 递归版插入排序（针对 std::vector 容器指针）
 * @tparam T 元素模板类型
 * @param arr 待排序 vector 容器指针
 * @param n 当前递归需要排序的子数组长度
 */
template <typename T>
void insertionSortRecursiveHelper(std::vector<T> *arr, size_t n) {
    if (n <= 1) {
        return;
    }

    insertionSortRecursiveHelper(arr, n - 1);

    T last = (*arr)[n - 1];
    int j = static_cast<int>(n) - 2;

    while (j >= 0 && (*arr)[j] > last) {
        (*arr)[j + 1] = (*arr)[j];
        j--;
    }
    (*arr)[j + 1] = last;
}

/**
 * @brief 递归版插入排序（针对 std::vector 容器指针的主入口）
 * @tparam T 元素模板类型
 * @param arr 待排序 vector 容器指针
 */
template <typename T>
void insertionSortRecursive(std::vector<T> *arr) {
    if (arr->size() <= 1) {
        return;
    }
    insertionSortRecursiveHelper(arr, arr->size());
}

}  // namespace sorting

/**
 * @brief 辅助创建随机数组
 */
template <typename T>
static void create_random_array(T *arr, int N) {
    while (N--) {
        double r = (std::rand() % 10000 - 5000) / 100.f;
        arr[N] = static_cast<T>(r);
    }
}

/** 
 * @brief 单元自测用例
 */
static void tests() {
    // 测试 1
    int arr1[10] = {78, 34, 35, 6, 34, 56, 3, 56, 2, 4};
    std::cout << "Test 1... ";
    sorting::insertionSortRecursive(arr1, 10);
    assert(std::is_sorted(arr1, arr1 + 10));
    std::cout << "passed" << std::endl;

    // 测试 2
    int arr2[5] = {5, -3, 7, -2, 1};
    std::cout << "Test 2... ";
    sorting::insertionSortRecursive(arr2, 5);
    assert(std::is_sorted(arr2, arr2 + 5));
    std::cout << "passed" << std::endl;

    // 测试 3
    float arr3[5] = {5.6, -3.1, -3.0, -2.1, 1.8};
    std::cout << "Test 3... ";
    sorting::insertionSortRecursive(arr3, 5);
    assert(std::is_sorted(arr3, arr3 + 5));
    std::cout << "passed" << std::endl;

    // 测试 4: 容器版本
    std::vector<float> arr4({5.6, -3.1, -3.0, -2.1, 1.8});
    std::cout << "Test 4... ";
    sorting::insertionSortRecursive(&arr4);
    assert(std::is_sorted(std::begin(arr4), std::end(arr4)));
    std::cout << "passed" << std::endl;

    // 测试 5: 随机整数
    int arr5[50];
    std::cout << "Test 5... ";
    create_random_array(arr5, 50);
    sorting::insertionSortRecursive(arr5, 50);
    assert(std::is_sorted(arr5, arr5 + 50));
    std::cout << "passed" << std::endl;

    // 测试 6: 随机浮点数
    float arr6[50];
    std::cout << "Test 6... ";
    create_random_array(arr6, 50);
    sorting::insertionSortRecursive(arr6, 50);
    assert(std::is_sorted(arr6, arr6 + 50));
    std::cout << "passed" << std::endl;
}

/** 
 * @brief 主函数
 */
int main() {
    tests(); /// 运行自测
    return 0;
}
