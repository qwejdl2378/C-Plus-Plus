/**
 * @file
 * @brief Implementation of standard [Insertion Sort](https://en.wikipedia.org/wiki/Insertion_sort) algorithm (标准插入排序算法实现)
 *
 * @details
 * 插入排序（Insertion Sort）的工作方式非常像人们手动整理扑克牌：
 * 1. 数组被分为“已排序”和“未排序”两部分。
 * 2. 初始时，已排序部分只包含第一个元素（索引 0）。
 * 3. 接着，依次取未排序部分的每一个元素 `temp`，与已排序部分从右向左依次比较：
 *    - 如果已排序部分中的元素 `arr[j]` 大于 `temp`，则将 `arr[j]` 向右移动一位（腾出空位）。
 *    - 重复该过程，直到找到第一个小于或等于 `temp` 的元素，将 `temp` 插入到其右侧。
 *
 * ### 算法优缺点
 * - **优点**：简单易写，原位排序（额外空间 $O(1)$），且是**稳定排序（Stable Sort）**。
 *   在数据量较小，或者数据本身基本有序时，插入排序的实际运行效率非常高。
 * - **缺点**：在大数据量下效率极低（时间复杂度为平方级 $O(N^2)$）。
 *
 * @note
 * 【C++ 语法书写规范建议】：
 * 在针对 `std::vector<T>* arr` 指针的版本中，代码使用了 `arr[0][i]` 来访问元素。
 * 虽然在 C++ 语法中，由于指针退化，`arr[0]` 相当于解引用 `*arr`，写法编译正确，
 * 但这种“看起来像二维数组”的书写格式极易引起误解。推荐改写为直观的 `(*arr)[i]`，提升代码的可读性。
 *
 * 时间复杂度: 
 *   - 最好情况: $O(N)$ (当输入数组已完全有序时，内层循环不发生移动)
 *   - 最坏情况: $O(N^2)$ (逆序数组)
 *   - 平均情况: $O(N^2)$
 * 空间复杂度: $O(1)$
 *
 * @author Unknown author
 */

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {
/**
 * @brief 针对 C-style 指针数组的插入排序
 * @tparam T 元素模板类型
 * @param arr 数组首地址指针
 * @param n 数组长度
 */
template <typename T>
void insertionSort(T *arr, int n) {
    for (int i = 1; i < n; i++) {
        T temp = arr[i];
        int j = i - 1;
        // 将大于 temp 的元素向右挪动
        while (j >= 0 && temp < arr[j]) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp; // 插入到合适的位置
    }
}

/**
 * @brief 针对 std::vector 容器指针的插入排序
 * @tparam T 元素模板类型
 * @param arr 存放数据的 vector 指针
 */
template <typename T>
void insertionSort(std::vector<T> *arr) {
    size_t n = arr->size();

    for (size_t i = 1; i < n; i++) {
        T temp = (*arr)[i]; // 规范化书写：使用 (*arr)[i] 替代混淆的 arr[0][i]
        int32_t j = i - 1;
        while (j >= 0 && temp < (*arr)[j]) {
            (*arr)[j + 1] = (*arr)[j];
            j--;
        }
        (*arr)[j + 1] = temp;
    }
}

}  // namespace sorting

/**
 * @brief 随机数生成填充辅助函数
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
void tests() {
    // 测试 1
    int arr1[10] = {78, 34, 35, 6, 34, 56, 3, 56, 2, 4};
    std::cout << "Test 1... ";
    sorting::insertionSort(arr1, 10);
    assert(std::is_sorted(arr1, arr1 + 10));
    std::cout << "passed" << std::endl;

    // 测试 2: 负数
    int arr2[5] = {5, -3, 7, -2, 1};
    std::cout << "Test 2... ";
    sorting::insertionSort(arr2, 5);
    assert(std::is_sorted(arr2, arr2 + 5));
    std::cout << "passed" << std::endl;

    // 测试 3: 浮点数
    float arr3[5] = {5.6, -3.1, -3.0, -2.1, 1.8};
    std::cout << "Test 3... ";
    sorting::insertionSort(arr3, 5);
    assert(std::is_sorted(arr3, arr3 + 5));
    std::cout << "passed" << std::endl;

    // 测试 4: 容器 std::vector 版本
    std::vector<float> arr4({5.6, -3.1, -3.0, -2.1, 1.8});
    std::cout << "Test 4... ";
    sorting::insertionSort(&arr4);
    assert(std::is_sorted(std::begin(arr4), std::end(arr4)));
    std::cout << "passed" << std::endl;

    // 测试 5: 50 个随机整数
    int arr5[50];
    std::cout << "Test 5... ";
    create_random_array(arr5, 50);
    sorting::insertionSort(arr5, 50);
    assert(std::is_sorted(arr5, arr5 + 50));
    std::cout << "passed" << std::endl;

    // 测试 6: 50 个随机浮点数
    float arr6[50];
    std::cout << "Test 6... ";
    create_random_array(arr6, 50);
    sorting::insertionSort(arr6, 50);
    assert(std::is_sorted(arr6, arr6 + 50));
    std::cout << "passed" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    tests(); // 运行单元自测

    size_t n = 0;
    std::cout << "Enter the length of your array (0 to exit): ";
    std::cin >> n;
    if (n == 0) {
        return 0;
    }

    int *arr = new int[n];
    std::cout << "Enter any " << n << " numbers for unsorted Array: ";
    for (size_t i = 0; i < n; i++) {
        std::cin >> arr[i];
    }

    sorting::insertionSort(arr, n);

    std::cout << "Sorted Array: ";
    for (size_t i = 0; i < n; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
    
    delete[] arr;
    return 0;
}
