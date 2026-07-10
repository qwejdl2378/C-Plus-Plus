/**
 * \file
 * \brief C++ program to implement [Jump Search](https://en.wikipedia.org/wiki/Jump_search) (跳转查找/块查找算法实现)
 */
#include <algorithm>
#include <cmath>
#include <iostream>

/** 
 * @brief 在已排序数组中执行跳转查找
 * @details 
 * 跳转查找（Jump Search / Block Search）的原理是将有序数组分割为若干个大小为 m = sqrt(n) 的块。
 * 1. 首先以 m 为步长进行跳转，确定目标元素可能存在于哪一个块内。
 * 2. 找到候选块后，在块内部执行一次局部的线性查找。
 * 
 * 相比线性查找 O(n) 和二分查找 O(log n)，跳转查找的复杂度介于二者之间，为 O(sqrt(n))。
 * 优点：在某些不支持双向随机访问（跳跃成本高）或只能单向流式读入的高开销介质上表现良好。
 * 
 * @param arr 有序数组
 * @param x 待查找的目标值
 * @param n 数组的元素总长度
 * @returns 目标值的索引，找不到则返回 -1
 */
int jumpSearch(int arr[], int x, int n) {
    // 计算最优的跳转步长（块大小），通常设为 sqrt(n)
    int step = std::sqrt(n);

    // 1. 寻找可能包含目标元素 x 的具体分块区间 [prev, step]
    int prev = 0;
    while (arr[std::min(step, n) - 1] < x) {
        prev = step; // 更新前驱块起始索引
        step += std::sqrt(n); // 前进一个步长
        if (prev >= n)
            return -1; // 越界，说明元素不在数组中
    }

    // 2. 在确定的候选块 [prev, min(step, n)] 内执行局部顺序扫描（线性查找）
    while (arr[prev] < x) {
        prev++;

        // 如果已经扫描完了当前块（或者到达数组末尾）仍没有发现比目标值大或相等的数，则代表不存在
        if (prev == std::min(step, n))
            return -1;
    }
    
    // 3. 判断当前指向的元素是否等于目标值
    if (arr[prev] == x)
        return prev; // 命中，返回其索引

    return -1;
}

/** 
 * @brief 主函数，演示自测用例
 * @returns 0
 */
int main() {
    int arr[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610};
    int x = 55;
    int n = sizeof(arr) / sizeof(arr[0]);

    // 调用跳转查找
    int index = jumpSearch(arr, x, n);

    // 输出索引位置
    std::cout << "\nNumber " << x << " is at index " << index << std::endl;
    return 0;
}
