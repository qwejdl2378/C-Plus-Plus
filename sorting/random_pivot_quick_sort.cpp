/**
 * @file
 * @brief Implementation of the Randomized Pivot [Quick Sort](https://en.wikipedia.org/wiki/Quicksort) algorithm (随机基准值快速排序算法实现)
 *
 * @details
 * 随机基准快速排序（Random Pivot Quick Sort）是经典快速排序的变体。
 * 经典快排在处理近乎有序或逆序的数组时，如果总是选择边界（第一个或最后一个）元素作为基准值（Pivot），
 * 其划分会极度失衡，导致时间复杂度退化至最坏情况的 $O(N^2)$。
 *
 * 通过在区间 `[start, end]` 内**随机选择一个索引**作为基准，并与边界元素交换后再进行常规划分，
 * 可以在概率上极大概率地打乱输入特征，将最坏情况发生的概率降为几乎为零，保证实际运行时间稳定在 $O(N \log N)$。
 *
 * @note
 * 【C++ 随机数种子滥用与性能隐患审计】：
 * 1. **`srand` 重复初始化 Bug**：原代码在 `getRandomIndex` 和 `generateUnsortedArray` 内部，每次调用时都会执行 `srand(time(nullptr));`。
 *    由于 `time(nullptr)` 的精度为秒级，在一秒钟内调用数千次排序，随机种子会被重置为**完全相同的值**。
 *    这导致“随机”生成的基准值全部变成固定的同一个数，完全失去了随机化的保护作用！
 *    **修复**：删除了子函数内部的 `srand` 初始化，统一交由 `main()` 函数在程序启动时初始化一次即可。
 * 2. **频繁数组值拷贝性能瓶颈**：原代码的 `quickSortRP` 接口直接通过传值的方式 `std::array<int64_t, size> arr` 传递容器，
 *    并且在每次递归调用时都复制了整阶 `std::array` 副本（空间开销为 $O(N)$）。
 *    对于大规模数组，这会引起海量无谓的内存移动，耗尽栈空间。实际工业实现中，应像经典快排一样，通过引用或指针在原数组上进行就地（In-place）修改。
 *
 * 时间复杂度: 平均 $O(N \log N)$，最坏（极小概率） $O(N^2)$
 * 空间复杂度: $O(N \log N)$ (由于递归时传递了 array 副本)
 * 
 * @author [Nitin Sharma](https://github.com/foo290)
 */

#include <algorithm>  /// 用于 std::is_sorted(), std::swap()
#include <array>      /// 用于 std::array
#include <cassert>    /// 用于 assert 断言
#include <ctime>      /// 用于初始化随机发生器
#include <iostream>   /// 用于输入输出
#include <tuple>      /// 用于 std::tuple 和 std::tie

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {
/**
 * @namespace random_pivot_quick_sort
 * @brief 随机基准快排算法命名空间
 */
namespace random_pivot_quick_sort {

/**
 * @brief 辅助打印数组
 */
template <size_t T>
void showArray(std::array<int64_t, T> arr) {
    for (size_t i = 0; i < arr.size(); i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

/**
 * @brief 在 [start, end] 区间内生成一个随机索引
 * @param start 区间起始索引
 * @param end 区间结束索引
 * @returns 产生的随机索引值
 */
int64_t getRandomIndex(int64_t start, int64_t end) {
    // 修复：移除内部重复调用 srand(time(nullptr))，以防止随机序列退化为常数
    int64_t randomPivotIndex = start + std::rand() % (end - start + 1);
    return randomPivotIndex;
}

/**
 * @brief Lomuto 划分方案，以 arr[end] 为基准值进行左右划分
 * @tparam size 数组长度
 * @param arr 传入的数组副本（按值传递）
 * @param start 划分起始索引
 * @param end 划分结束索引
 * @returns 划分完后的 pivot 索引以及修改后的数组元组
 */
template <size_t size>
std::tuple<int64_t, std::array<int64_t, size>> partition(
    std::array<int64_t, size> arr, int64_t start, int64_t end) {
    int64_t pivot = arr[end]; 
    int64_t pInd = start;

    for (int64_t i = start; i < end; i++) {
        if (arr[i] <= pivot) {
            std::swap(arr[i], arr[pInd]);
            pInd++;
        }
    }
    std::swap(arr[pInd], arr[end]);
    return std::make_tuple(pInd, arr);
}

/**
 * @brief 随机基准快速排序主递归函数（传值返回版本）
 * @tparam size 数组大小
 * @param arr 输入的数组副本
 * @param start 起始排序范围
 * @param end 结束排序范围
 * @returns 排序好（升序）的新数组
 */
template <size_t size>
std::array<int64_t, size> quickSortRP(std::array<int64_t, size> arr,
                                      int64_t start, int64_t end) {
    if (start < end) {
        // 随机选择基准元素并交换到右边界
        int64_t randomIndex = getRandomIndex(start, end);
        std::swap(arr[end], arr[randomIndex]);

        int64_t pivotIndex = 0;
        // 执行划分
        std::tie(pivotIndex, arr) = partition(arr, start, end);

        // 递归排序左半部分和右半部分
        std::array<int64_t, size> rightSortingLeft =
            quickSortRP(arr, start, pivotIndex - 1);
        std::array<int64_t, size> full_sorted =
            quickSortRP(rightSortingLeft, pivotIndex + 1, end);
        arr = full_sorted;
    }
    return arr;
}

/**
 * @brief 生成指定大小和值域范围的随机数数组
 * @tparam size 静态生成的数组大小
 * @param from 范围最小值
 * @param to 范围最大值
 */
template <size_t size>
std::array<int64_t, size> generateUnsortedArray(int64_t from, int64_t to) {
    std::array<int64_t, size> unsortedArray{};
    assert(from < to);
    size_t i = 0;
    while (i < size) {
        int64_t randomNum = from + std::rand() % (to - from + 1);
        if (randomNum != 0) { // 避免零元素填充（本实现的一种偏好限制）
            unsortedArray[i] = randomNum;
            i++;
        }
    }
    return unsortedArray;
}

}  // namespace random_pivot_quick_sort
}  // namespace sorting

/**
 * @brief 包含单元自测用例的类
 */
class TestCases {
 private:
    template <typename T>
    void log(T msg) {
        std::cout << "[TESTS] : ---> " << msg << std::endl;
    }

 public:
    void runTests() {
        log("Running Tests...");
        testCase_1();
        testCase_2();
        testCase_3();
        log("Test Cases over!");
        std::cout << std::endl;
    }

    /**
     * @brief 边界测试：仅包含一个元素的数组
     */
    void testCase_1() {
        const int64_t inputSize = 1;
        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        log("This is test case 1 for Random Pivot Quick Sort Algorithm : ");
        log("Description:");
        log("   EDGE CASE : Only contains one element");
        std::array<int64_t, inputSize> unsorted_arr{2};

        int64_t start = 0;
        int64_t end = unsorted_arr.size() - 1;

        std::array<int64_t, unsorted_arr.size()> sorted_arr =
            sorting::random_pivot_quick_sort::quickSortRP(unsorted_arr, start, end);

        assert(std::is_sorted(sorted_arr.begin(), sorted_arr.end()));
        log("Assertion check passed!");
        log("[PASS] : TEST CASE 1 PASS!");
        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    }

    /**
     * @brief 大数据测试：500 个元素的随机数组
     */
    void testCase_2() {
        const int64_t inputSize = 500;
        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        log("Description:");
        log("   BIG INPUT : Contains 500 elements and repeated elements");
        log("This is test case 2 for Random Pivot Quick Sort Algorithm : ");
        std::array<int64_t, inputSize> unsorted_arr =
            sorting::random_pivot_quick_sort::generateUnsortedArray<inputSize>(1, 10000);

        int64_t start = 0;
        int64_t end = unsorted_arr.size() - 1;

        std::array<int64_t, unsorted_arr.size()> sorted_arr =
            sorting::random_pivot_quick_sort::quickSortRP(unsorted_arr, start, end);

        assert(std::is_sorted(sorted_arr.begin(), sorted_arr.end()));
        log("Assertion check passed!");
        log("[PASS] : TEST CASE 2 PASS!");
        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    }

    /**
     * @brief 大数据测试：1000 个元素的随机数组
     */
    void testCase_3() {
        const int64_t inputSize = 1000;
        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        log("This is test case 3 for Random Pivot Quick Sort Algorithm : ");
        log("Description:");
        log("   LARGE INPUT : Contains 1000 elements and repeated elements");
        std::array<int64_t, inputSize> unsorted_arr =
            sorting::random_pivot_quick_sort::generateUnsortedArray<inputSize>(1, 10000);

        int64_t start = 0;
        int64_t end = unsorted_arr.size() - 1;

        std::array<int64_t, unsorted_arr.size()> sorted_arr =
            sorting::random_pivot_quick_sort::quickSortRP(unsorted_arr, start, end);

        assert(std::is_sorted(sorted_arr.begin(), sorted_arr.end()));
        log("Assertion check passed!");
        log("[PASS] : TEST CASE 3 PASS!");
        log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    }
};

/**
 * @brief 测试入口
 */
static void test() {
    TestCases tc = TestCases();
    tc.runTests();
}

/**
 * @brief 主函数
 */
int main() {
    // 整个程序运行期间，只初始化一次随机发生器种子，确保随机质量
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    test(); // 运行测试用例

    const int64_t inputSize = 10;
    std::array<int64_t, inputSize> unsorted_array =
        sorting::random_pivot_quick_sort::generateUnsortedArray<inputSize>(50, 1000);
    std::cout << "Unsorted array is : " << std::endl;
    sorting::random_pivot_quick_sort::showArray(unsorted_array);

    std::array<int64_t, inputSize> sorted_array =
        sorting::random_pivot_quick_sort::quickSortRP(
            unsorted_array, 0, unsorted_array.size() - 1);
            
    std::cout << "Sorted array is : " << std::endl;
    sorting::random_pivot_quick_sort::showArray(sorted_array);
    return 0;
}
