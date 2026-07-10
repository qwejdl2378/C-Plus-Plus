/**
 * \file
 * \brief [Exponential search algorithm](https://en.wikipedia.org/wiki/Exponential_search) (指数搜索/双倍搜索算法实现)
 * \copyright 2020 Divide-et-impera-11
 *
 * 指数搜索（也叫 Struzik 搜索）适用于无界或极大的有序数组。
 * 1. 算法首先通过指数级倍增（1, 2, 4, 8...）来快速确定目标元素所可能处的区间范围。
 * 2. 确定区间范围后，在对应的子区间上执行标准的二分查找。
 * 3. 其时间复杂度为 O(log i)，其中 i 是目标值在数组中的真实索引位置。特别适用于搜索目标位于数组前部的情况。
 */
#include <cassert>
#include <cmath>
#include <cstdint>
#ifdef _MSC_VER
#include <string>  // MS Visual C++ 兼容支持
#else
#include <cstring>  // 其他编译器头文件
#endif

/** 
 * @brief 在区间段内执行经典的折半二分查找算法（内联模板函数）
 * * 时间复杂度 O(log n)
 * * 空间复杂度 O(1)
 * 
 * @tparam Type 数组元素类型
 * @param array 指向区间起始位置的指针
 * @param size 区间长度
 * @param key 待查找的目标值
 * @returns 指向查找到的数组元素的指针；若未找到则返回 `nullptr`
 */
template <class Type>
inline Type* binary_s(Type* array, size_t size, Type key) {
    int32_t lower_index(0), upper_index(size - 1), middle_index;

    while (lower_index <= upper_index) {
        middle_index = std::floor((lower_index + upper_index) / 2);

        if (*(array + middle_index) < key)
            lower_index = (middle_index + 1);
        else if (*(array + middle_index) > key)
            upper_index = (middle_index - 1);
        else
            return (array + middle_index); // 命中，返回指针
    }

    return nullptr; // 未找到
}

/** 
 * @brief 指数查找算法核心函数 (Struzik Search)
 * * 时间复杂度 O(log i)，i 是目标键值的真实索引
 * * 空间复杂度 O(1)
 * 
 * @tparam Type 数组元素类型
 * @param array 指向排序数组的起始位置指针
 * @param size 数组的总大小
 * @param key 待查找的目标值
 * @returns 指向查找到的数组元素的指针；若未找到则返回 `nullptr`
 */
template <class Type>
Type* struzik_search(Type* array, size_t size, Type key) {
    uint32_t block_front(0), block_size = size == 0 ? 0 : 1;
    
    // 不断倍增定位区间范围 [block_front, block_size]
    while (block_front != block_size) {
        // 如果当前区间尾部元素仍小于目标值，说明目标还在右侧更远处
        if (*(array + block_size - 1) < key) {
            block_front = block_size; // 左边界向右收缩
            // 指数级倍增右边界大小，若超出范围则截断为数组最大长度 size
            (block_size * 2 - 1 < size) ? (block_size *= 2) : block_size = size;
            continue;
        }
        
        // 发现区间尾部元素已大于或等于目标值，说明目标处于当前划分的块 [block_front, block_size] 内
        // 对该局部区间进行标准的二分查找
        return binary_s<Type>(array + block_front, (block_size - block_front), key);
    }
    return nullptr;
}

/** 
 * @brief 主函数，演示测试用例
 * @returns 0
 */
int main() {
    // 准备排好序的测试数组
    int* sorted_array = new int[7]{7, 10, 15, 23, 70, 105, 203};
    
    assert(struzik_search<int>(sorted_array, 7, 0) == nullptr);
    assert(struzik_search<int>(sorted_array, 7, 1000) == nullptr);
    assert(struzik_search<int>(sorted_array, 7, 50) == nullptr);
    assert(struzik_search<int>(sorted_array, 7, 7) == sorted_array);
    
    delete[] sorted_array;
    return 0;
}
