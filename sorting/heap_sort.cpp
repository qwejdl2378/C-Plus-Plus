/**
 * \file
 * \brief [Heap Sort Algorithm (heap sort)](https://en.wikipedia.org/wiki/Heapsort) implementation (堆排序算法实现)
 *
 * \author [Ayaan Khan](http://github.com/ayaankhan98)
 *
 * \details
 *  堆排序是一种基于比较的排序算法。
 *  它可以被看作是选择排序的改进版：
 *  与选择排序类似，堆排序将数组分为已排序区域和未排序区域，
 *  并通过不断从未排序区域中取出最大（或最小）元素加入到已排序区域来缩小未排序部分。
 *  但不同的是，选择排序是在未排序区域中进行线性扫描（耗时 $O(n)$），
 *  而堆排序则通过构建堆结构，以 $O(\log n)$ 的时间复杂度快速找到并提取当前的最大值。
 *
 *  时间复杂度 - $O(n \log n)$ （最好、最坏、平均情况下均为该时间复杂度）
 *
 */
#include <algorithm>
#include <cassert>
#include <iostream>

/**
 * @brief 打印数组元素的辅助工具函数
 * @tparam T 数组元素类型
 * @param arr 数组指针
 * @param sz 数组大小
 */
template <typename T>
void printArray(T *arr, int sz) {
    for (int i = 0; i < sz; i++) std::cout << arr[i] << "  ";
    std::cout << "\n";
}

/**
 * \addtogroup sorting Sorting Algorithm
 * @{
 *
 * @brief 堆化操作 (Heapify)
 * 堆化操作是维持堆性质的核心。自底向上 sift-down (下滤)，
 * 将当前节点与左右子节点比较，将最大值交换上来，以此建立/维护最大堆的性质。
 *
 * @tparam T 数据类型
 * @param arr 待堆化的数组指针
 * @param n 当前堆的大小（参与堆化的数组长度）
 * @param i 待堆化的当前节点的索引
 */
template <typename T>
void heapify(T *arr, int n, int i) {
    int largest = i;       // 初始化 largest 为当前根节点索引
    int l = 2 * i + 1;     // 左子节点在完全二叉树数组表示中的索引
    int r = 2 * i + 2;     // 右子节点在完全二叉树数组表示中的索引

    // 如果左子节点在合法范围内，且其值大于当前最大值
    if (l < n && arr[l] > arr[largest])
        largest = l;

    // 如果右子节点在合法范围内，且其值大于当前最大值
    if (r < n && arr[r] > arr[largest])
        largest = r;

    // 如果最大值不是当前根节点本身，则进行交换，并递归堆化受影响的子树
    if (largest != i) {
        std::swap(arr[i], arr[largest]);
        
        // 递归堆化被交换的子节点分支，确保其子树满足最大堆性质
        heapify(arr, n, largest);
    }
}

/**
 * @brief 堆排序主体函数
 * @tparam T 数组元素类型
 * @param arr 待排序数组的指针
 * @param n 数组元素个数
 */
template <typename T>
void heapSort(T *arr, int n) {
    // 步骤 1：构建初始最大堆 (Build Max Heap)
    // 从最后一个非叶子节点 (n/2 - 1) 开始，依次向前进行堆化操作
    for (int i = n / 2 - 1; i >= 0; i--) heapify(arr, n, i);

    // 步骤 2：进行堆排序
    // 每次将堆顶的最大值 (arr[0]) 交换到当前堆的末尾 (arr[i])，
    // 然后将堆的有效范围缩小一格，重新对堆顶进行堆化 (heapify)，以此保持最大堆的特性
    for (int i = n - 1; i >= 0; i--) {
        std::swap(arr[0], arr[i]); // 将当前最大值移动到数组末尾
        heapify(arr, i, 0);        // 对缩小后的堆重建最大堆性质
    }
}

/**
 * @brief 自测用例集
 */
void test() {
    std::cout << "Test 1\n";
    int arr[] = {-10, 78, -1, -6, 7, 4, 94, 5, 99, 0};
    int sz = sizeof(arr) / sizeof(arr[0]);  // sz - 数组长度
    printArray(arr, sz);  // 排序前打印
    heapSort(arr, sz);    // 调用堆排序
    printArray(arr, sz);  // 排序后打印
    assert(std::is_sorted(arr, arr + sz));
    std::cout << "Test 1 Passed\n========================\n";

    std::cout << "Test 2\n";
    double arr2[] = {4.5, -3.6, 7.6, 0, 12.9};
    sz = sizeof(arr2) / sizeof(arr2[0]);
    printArray(arr2, sz);
    heapSort(arr2, sz);
    printArray(arr2, sz);
    assert(std::is_sorted(arr2, arr2 + sz));
    std::cout << "Test 2 passed\n";
}

/** 主函数 */
int main() {
    test(); // 运行自测集
    return 0;
}
