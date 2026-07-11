/**
 * @file
 * @brief Implementation of optimized [Shell Sort](https://en.wikipedia.org/wiki/Shellsort) algorithm using Ciura's gap sequence (基于 Ciura 步长序列的优化版希尔排序算法实现)
 *
 * @details
 * 本实现（shell_sort2）使用了著名的 Ciura 步长序列 `{701, 301, 132, 57, 23, 10, 4, 1}`。
 *  Marcin Ciura 在实验中发现这个增量序列比传统的折半（Shell 原始序列）效率更高，
 * 其平均时间复杂度被实验证明能达到更低的增长阶数。
 *
 * @note
 * 【C++ 模板类型减法溢出的重大安全 Bug 审计与修复】：
 * 原代码在第 55 行的比较条件中写为：
 *   `(arr[j - gap] - tmp) > 0`
 * 1. **无符号类型下溢 Bug**：如果模板类型 `T` 被实例化为**无符号整型**（如 `uint32_t`, `uint64_t`），
 *    当比较 `3` 和 `5` 时，`3 - 5` 会发生无符号整型下溢，得到一个非常大的正数，从而使表达式错误地评估为 `true`。
 *    这会导致排序逻辑完全错乱，产生错误的升降序或内存越界。
 * 2. **自定义类型不支持减法**：如果 `T` 是自定义类（如 `std::string` 或其他没有重载 `operator-` 的自定义结构体），
 *    该减法操作会直接导致**编译失败**，破坏了模板泛型设计的通用性。
 * 
 * ### 修复方案
 * - 将不安全的减法比较重构为最直接、最通用的关系比较：
 *   `arr[j - gap] > tmp`
 * - 该修复保证了对所有无符号整型、浮点数以及只支持常规比较的自定义类型的绝对安全与兼容。
 *
 * 时间复杂度: 约 $O(N^{1.3})$ (对于 Ciura 序列)
 * 空间复杂度: $O(1)$
 * 
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <utility>  // 用于 std::swap
#include <vector>

/**
 * @brief 辅助打印数组内容
 */
template <class T>
void show_data(T *arr, size_t LEN) {
    for (size_t i = 0; i < LEN; i++) {
        std::cout << arr[i] << ", ";
    }
    std::cout << std::endl;
}

/**
 * @brief 辅助打印数组重载
 */
template <typename T, size_t N>
void show_data(T (&arr)[N]) {
    show_data(arr, N);
}

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {

/**
 * @brief 优化版希尔排序核心算法
 * @tparam T 元素模板类型
 * @param arr 待排序数组的指针
 * @param LEN 数组长度
 */
template <typename T>
void shell_sort(T *arr, size_t LEN) {
    // Ciura 最佳实验增量序列
    const unsigned int gaps[] = {701, 301, 132, 57, 23, 10, 4, 1};
    const unsigned int gap_len = 8;
    size_t i = 0, j = 0, g = 0;

    for (g = 0; g < gap_len; g++) {
        unsigned int gap = gaps[g];
        for (i = gap; i < LEN; i++) {
            T tmp = arr[i];

            // 核心修复：使用 arr[j - gap] > tmp 代替减法以防范无符号下溢，并支持无减法操作符的对象排序
            for (j = i; j >= gap && arr[j - gap] > tmp; j -= gap) {
                arr[j] = arr[j - gap];
            }

            arr[j] = tmp;
        }
    }
}

/**
 * @brief 希尔排序重载（原生数组引用版本）
 */
template <typename T, size_t N>
void shell_sort(T (&arr)[N]) {
    shell_sort(arr, N);
}

/**
 * @brief 希尔排序重载（std::vector 容器版本）
 */
template <typename T>
void shell_sort(std::vector<T> *arr) {
    shell_sort(arr->data(), arr->size());
}

}  // namespace sorting

using sorting::shell_sort;

/**
 * @brief 比较器，用于 qsort 校验
 */
template <typename T>
int compare(const void *a, const void *b) {
    T arg1 = *static_cast<const T *>(a);
    T arg2 = *static_cast<const T *>(b);

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

/**
 * @brief 自测用例（整数类型）
 */
void test_int(const int NUM_DATA) {
    int *data = new int[NUM_DATA];
    int *data2 = new int[NUM_DATA];
    int range = 1800;

    for (int i = 0; i < NUM_DATA; i++) {
        data[i] = data2[i] = (std::rand() % range) - (range >> 1);
    }

    std::clock_t start = std::clock();
    shell_sort(data, NUM_DATA);
    std::clock_t end = std::clock();
    double elapsed_time = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::cout << "Time spent sorting using shell_sort2 (int): " << elapsed_time << "s\n";

    std::qsort(data2, NUM_DATA, sizeof(data2[0]), compare<int>);

    for (int i = 0; i < NUM_DATA; i++) {
        assert(data[i] == data2[i]);
    }

    delete[] data;
    delete[] data2;
}

/**
 * @brief 自测用例（浮点类型）
 */
void test_f(const int NUM_DATA) {
    float *data = new float[NUM_DATA];
    float *data2 = new float[NUM_DATA];
    int range = 1000;

    for (int i = 0; i < NUM_DATA; i++) {
        data[i] = data2[i] = static_cast<float>((std::rand() % range) - (range >> 1)) / 100.f;
    }

    std::clock_t start = std::clock();
    shell_sort(data, NUM_DATA);
    std::clock_t end = std::clock();
    double elapsed_time = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::cout << "Time spent sorting using shell_sort2 (float): " << elapsed_time << "s\n";

    std::qsort(data2, NUM_DATA, sizeof(data2[0]), compare<float>);

    for (int i = 0; i < NUM_DATA; i++) {
        assert(data[i] == data2[i]);
    }

    delete[] data;
    delete[] data2;
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[]) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    test_int(100);
    std::cout << "Test 1 - 100 int values - passed. \n";
    test_int(1000);
    std::cout << "Test 2 - 1000 int values - passed.\n";
    test_int(10000);
    std::cout << "Test 3 - 10000 int values - passed.\n";

    test_f(100);
    std::cout << "Test 1 - 100 float values - passed. \n";
    test_f(1000);
    std::cout << "Test 2 - 1000 float values - passed.\n";
    test_f(10000);
    std::cout << "Test 3 - 10000 float values - passed.\n";

    int NUM_DATA = 0;
    if (argc == 2) {
        NUM_DATA = std::atoi(argv[1]);
    } else {
        NUM_DATA = 200;
    }

    int *data = new int[NUM_DATA];
    int range = 1800;

    for (int i = 0; i < NUM_DATA; i++) {
        data[i] = (std::rand() % range) - (range >> 1);
    }

    std::cout << "Unsorted original data: " << std::endl;
    show_data(data, NUM_DATA);
    
    std::clock_t start = std::clock();
    shell_sort(data, NUM_DATA);
    std::clock_t end = std::clock();

    std::cout << "Data Sorted using custom implementation: " << std::endl;
    show_data(data, NUM_DATA);

    double elapsed_time = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::cout << "Time spent sorting: " << elapsed_time << "s\n" << std::endl;

    delete[] data;
    return 0;
}
