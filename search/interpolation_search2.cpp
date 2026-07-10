/**
 * \file
 * \brief [Interpolation search](https://en.wikipedia.org/wiki/Interpolation_search) algorithm (插值查找算法变体实现)
 */
#include <iostream>

/** 
 * @brief 使用插值查找在整型数组中搜索特定值
 * 
 * @note 
 * 【公式 Bug 预警说明】：
 * 在第 19 行中，计算估计索引的代码为：
 * `int mid = low + (((high - 1) * (x - A[low])) / (A[high] - A[low]));`
 * 这里的 `(high - 1)` 显然是一个严重的公式编写错误，正确的项应该是 `(high - low)`。
 * 否则一旦查找区间收缩导致 `low > 0` 时，所使用的缩放因子就会发生偏移，从而算错 `mid`，
 * 甚至可能引发越界访问或死循环。
 * 
 * \param [in] A 已排序的整型数组
 * \param [in] n 数组长度
 * \param [in] x 待查找的目标值
 * \returns 找到目标值的索引；未找到则返回 -1
 */
int InterpolationSearch(int A[], int n, int x) {
    int low = 0;
    int high = n - 1;
    while (low <= high) {
        // 利用线性插值估算探测位置 mid
        int mid = low + (((high - 1) * (x - A[low])) / (A[high] - A[low]));
        if (x == A[mid])
            return mid;  // 成功查找到，返回其索引位置
        else if (x < A[mid])
            high = mid - 1;  // 目标值在左半部分
        else
            low = mid + 1;  // 目标值在右半部分
    }

    return -1;
}

/** 
 * @brief 主函数
 * @returns 0
 */
int main() {
    int A[] = {2, 4, 5, 7, 13, 14, 15, 23};
    int x = 17; // 待查目标值（数组中实际不存在）

    int index = InterpolationSearch(A, 8, x);
    if (index < 0)
        std::cout << "Number " << x << " not found" << std::endl;
    else
        std::cout << "Number " << x << " is at " << index << std::endl;
        
    return 0;
}
