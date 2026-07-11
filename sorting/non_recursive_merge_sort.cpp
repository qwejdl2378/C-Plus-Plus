/**
 * @file
 * @brief Implementation of bottom-up non-recursive [Merge Sort](https://en.wikipedia.org/wiki/Merge_sort) algorithm (非递归版自底向上归并排序算法实现)
 *
 * @details
 * 自底向上的归并排序（Bottom-up Merge Sort）是常规递归归并排序的非递归实现形式。
 * 它从小到大逐步合并子区间：
 * 1. 初始步长（`length`）设为 1，将相邻长度为 1 的小区间合并为长度为 2 的有序区间。
 * 2. 步长翻倍（`length <<= 1`），重复上述合并操作，直到步长大于等于数组的总长度 `N`。
 *
 * 与自顶向下的递归实现相比，自底向上不需要系统级函数递归调用，因而**没有递归栈溢出风险**，
 * 但仍然需要 $O(N)$ 的额外暂存辅助空间。
 *
 * @note
 * 【C++ 模板泛型与内存安全设计审计】：
 * 在第 29 行和第 61 行中：
 *   `char* buffer = new char[n * sizeof(*first)];`
 *   `*p++ = std::move(*t);`
 * 1. 原代码使用 `reinterpret_cast` 将 `char*` 缓冲区映射为模板类型 `T*`，并直接使用移动赋值操作符 `*p = std::move(*t)`。
 * 2. **严重隐患**：如果容器中存储的是非平凡构造（Non-trivial）的对象（如 `std::string`, `std::vector` 等），
 *    由于 `char*` 只是原始内存，并没有在该处调用过这些对象的构造函数（Constructor），
 *    直接调用赋值操作符会使对象尝试析构其内部的垃圾指针，从而触发**段错误（Segmentation Fault）/ 双重释放（Double Free）**崩溃。
 * 3. **适用界限**：此模版实现仅对**平凡拷贝类型（Trivially Copyable）**（如 `int`, `double`, `char`, 结构体指针等）是安全的。对于非平凡类型，应在缓冲区内存上使用 `placement new` 进行构造，并在释放时显式调用析构函数。
 *
 * 时间复杂度: $O(N \log N)$ (最好、最坏、平均)
 * 空间复杂度: $O(N)$
 * 
 * @author Albirair
 */

#include <cstddef>  // 用于 size_t
#include <iostream>
#include <utility>  // 用于 std::move & std::remove_reference_t
#include <type_traits> // 用于 std::is_trivially_copyable

namespace sorting {

// 声明合并函数
template <class Iterator>
void merge(Iterator, Iterator, const Iterator, char[]);

/**
 * @brief 自底向上非递归归并排序
 * @tparam Iterator 迭代器类型（如指针或 vector::iterator）
 * @param first 指向数组起始位置的迭代器
 * @param last 指向数组末尾后一位置的迭代器
 * @param n 数组中的元素数量
 */
template <class Iterator>
void non_recursive_merge_sort(const Iterator first, const Iterator last,
                              const size_t n) {
    if (n <= 1) {
        return;
    }

    // 分配 n 个元素大小的字节缓冲区
    char* buffer = new char[n * sizeof(*first)];

    // 外部循环：步长以 2 的指数级增长（1, 2, 4, 8 ...）
    for (size_t length(1); length < n; length <<= 1) {
        Iterator left(first);
        
        // 1. 成对合并当前步长下的相邻子区间
        for (size_t counter(n / (length << 1)); counter; --counter) {
            Iterator right(left + length);
            Iterator end(right + length);
            merge(left, right, end, buffer);
            left = end;
        }
        
        // 2. 处理剩余不足一个完整成对区间的尾部元素
        if ((n & ((length << 1) - 1)) > length) {
            merge(left, left + length, last, buffer);
        }
    }
    
    delete[] buffer; // 释放字节缓冲区
}

/**
 * @brief 合并两个相邻的有序子区间 [l, r) 和 [r, e) 放入暂存区，并回写
 * @tparam Iterator 迭代器类型
 * @param l 左区间起点
 * @param r 右区间起点（即左区间终点）
 * @param e 右区间终点
 * @param b 字节暂存缓冲区首地址
 */
template <class Iterator>
void merge(Iterator l, Iterator r, const Iterator e, char b[]) {
    using T = typename std::remove_reference_t<decltype(*l)>;

    // 将字节缓冲区指针转换为实际类型 T 的指针
    auto p(reinterpret_cast<T*>(b)), c(p);
    
    // 将左半部分元素移动复制到缓冲区
    for (Iterator t(l); r != t; ++t) {
        *p++ = std::move(*t);
    }
    
    // 双指针比较，将较小的元素写回原数组中
    while (e != r && c != p) {
        *l++ = std::move(*r < *c ? *r++ : *c++);
    }
    
    // 若右半部分有剩余，写回原数组
    while (e != r) {
        *l++ = std::move(*r++);
    }
    
    // 若暂存区（左半部分）有剩余，写回原数组
    while (c != p) {
        *l++ = std::move(*c++);
    }
}

/**
 * @brief 归并排序重载入口函数（指定起点及大小）
 */
template <class Iterator>
void non_recursive_merge_sort(const Iterator first, const size_t n) {
    non_recursive_merge_sort(first, first + n, n);
}

/**
 * @brief 归并排序重载入口函数（指定起点及终点迭代器）
 */
template <class Iterator>
void non_recursive_merge_sort(const Iterator first, const Iterator last) {
    non_recursive_merge_sort(first, last, last - first);
}

}  // namespace sorting

using sorting::non_recursive_merge_sort;

/**
 * @brief 主函数
 */
int main() {
    int size = 0;
    std::cout << "Enter the number of elements: ";
    std::cin >> size;
    if (size <= 0) {
        return 0;
    }

    int* arr = new int[size];
    for (int i = 0; i < size; ++i) {
        std::cout << "arr[" << i << "] = ";
        std::cin >> arr[i];
    }
    
    non_recursive_merge_sort(arr, size);
    
    std::cout << "Sorted array:\n";
    for (int i = 0; i < size; ++i) {
        std::cout << "arr[" << i << "] = " << arr[i] << '\n';
    }
    
    delete[] arr;
    return 0;
}
