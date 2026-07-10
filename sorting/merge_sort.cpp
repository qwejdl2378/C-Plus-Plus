/**
 *  \addtogroup sorting Sorting Algorithms
 *  @{
 *  \file
 *  \brief [Merge Sort Algorithm (MERGE SORT)](https://en.wikipedia.org/wiki/Merge_sort) implementation (归并排序算法实现)
 *
 *  \author [Ayaan Khan](http://github.com/ayaankhan98)
 *
 *  \details
 *  归并排序是一种高效、通用的基于比较的排序算法。
 *  它采用“分治法”(Divide and Conquer)，将问题分解为较小的子问题解决，然后再合并结果。
 *  时间复杂度：$O(n \log n)$ （在最好、最坏和平均情况下均保持一致）
 *  归并排序对于大规模排序，特别是外排序（磁盘数据排序）和链表排序非常高效且稳定。
 */
#include <iostream>
#include <vector>

/**
 * @brief merge 函数用于将两个已经排好序的子数组合并为一个有序数组。
 * 假设 arr[l..m] 和 arr[m+1..r] 分别是有序的，合并后的结果仍保存在 arr[l..r] 中。
 *
 * @param arr - 包含两个待合并子数组的源数组指针
 * @param l - 第一个子数组的起始索引 (左端点)
 * @param m - 第一个子数组的结束索引 (中点)
 * @param r - 第二个子数组的结束索引 (右端点，第二个子数组的范围是 m+1 到 r)
 */
void merge(int *arr, int l, int m, int r) {
    int n1 = m - l + 1; // 左半子数组的长度
    int n2 = r - m;     // 右半子数组的长度

    // 创建临时容器保存左右两半的数据
    std::vector<int> L(n1), R(n2);

    // 将数据拷贝到临时容器中
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;

    // 依次比较 L 和 R 中的元素，将较小者放回原数组 arr 中
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // 如果左半子数组还有剩余元素，拷贝回原数组
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // 如果右半子数组还有剩余元素，拷贝回原数组
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

/**
 * @brief 归并排序递归主体
 * 采用分治策略，将数组二分，对两半分别递归排序，最后合并。
 *
 * @param arr - 待排序数组的指针
 * @param l - 子数组的起始索引
 * @param r - 子数组的结束索引
 */
void mergeSort(int *arr, int l, int r) {
    if (l < r) {
        // 计算中点，防止 (l + r) 溢出
        int m = l + (r - l) / 2;
        
        // 递归对左半部分排序
        mergeSort(arr, l, m);
        
        // 递归对右半部分排序
        mergeSort(arr, m + 1, r);
        
        // 合并两个有序的子部分
        merge(arr, l, m, r);
    }
}

/**
 * @brief 打印数组元素的辅助工具函数
 * @param arr 数组指针
 * @param size 数组大小
 */
void show(int *arr, int size) {
    for (int i = 0; i < size; i++) std::cout << arr[i] << " ";
    std::cout << "\n";
}

/** 主函数 */
int main() {
    int size;
    std::cout << "Enter the number of elements: ";
    std::cin >> size;

    if (size <= 0) {
        std::cout << "Invalid size.\n";
        return 1;
    }

    int *arr = new int[size];
    std::cout << "Enter the unsorted elements: ";
    for (int i = 0; i < size; ++i) {
        std::cin >> arr[i];
    }

    // 执行归并排序
    mergeSort(arr, 0, size - 1);
    
    std::cout << "Sorted array: ";
    show(arr, size);
    
    // 释放动态分配的数组空间
    delete[] arr;
    return 0;
}
/** @} */
