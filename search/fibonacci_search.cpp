/**
 * @author sprintyaf
 * @file fibonacci_search.cpp
 * @brief [Fibonacci search algorithm](https://en.wikipedia.org/wiki/Fibonacci_search_technique) (斐波那契查找算法实现)
 */

#include <iostream>
#include <vector>    // 用于 std::vector
#include <cassert>   // 用于 assert
#include <cstdlib>   // 用于随机数生成
#include <algorithm> // 用于 std::sort 和 std::find

/**
 * @brief 使用斐波那契查找算法在已排序数组中搜索指定元素的位置
 * @details 
 * 斐波那契查找利用斐波那契数列划分区间。相比二分查找：
 * 1. 斐波那契查找仅使用加减法进行区间分割，无需进行除法或乘法操作（在某些老旧硬件或特定 CPU 上加减法远比除法高效）。
 * 2. 它的区间划分比例并非 1/2，而是约等于 0.618（黄金分割）。
 * 
 * 时间复杂度为 O(log n)。
 * 
 * @param arr 已排序的数组容器
 * @param value 待查找的目标值
 * @returns 目标值在数组中的索引位置，若未找到则返回 -1
 */
int fibonacci_search(const std::vector<int> &arr, int value){
    // 初始化斐波那契数：last (F_{k-2}), current (F_{k-1})
    int last = 0, current = 1;
    int length = arr.size(); // 数组长度
    // next (F_k = F_{k-1} + F_{k-2})
    int next = last + current; 

    // 1. 寻找最小的、且大于或等于数组长度 length 的斐波那契数 F_k
    while(next < length){
        last = current;
        current = next;
        next = last + current;
    }

    // offset 记录当前已排除的左侧区间终点
    int offset = -1, index;
    
    // 2. 循环缩小检索区间
    while(next > 1){
        // 尝试用斐波那契数 F_{k-2} 作为偏移，计算探测点 index
        index = std::min(offset + last, length - 1);
        
        // 若探测点的值小于目标值，说明目标位于探测点右侧区间
        if(arr[index] < value){
            // 将斐波那契数向下调整 1 阶（对应划分右半区）
            next = current;
            current = last;
            last = next - current;
            offset = index; // 更新左边界偏移量
        // 若探测点的值大于目标值，说明目标位于探测点左侧区间
        } else if(arr[index] > value){
            // 将斐波那契数向下调整 2 阶（对应划分左半区）
            next = last;
            current = current - last;
            last = next - current;
        // 找到目标值，返回对应索引
        } else {
            return index;
        }
    }
    
    // 3. 特判：比较遗留的最后一个可能位置（探测边界）
    if(current && !arr.empty() && arr[offset + 1] == value){
        return offset + 1;
    }
    
    // 未找到，返回 -1
    return -1;
}

/**
 * @brief 随机测试：检测目标值绝对不在数组中时的查找表现（应该一致返回 -1）
 */
bool no_occurence_tests(){
    bool passed = true;
    int rand_num, rand_value, index, num_tests = 1000;
    std::vector<int> arr;
    while(num_tests--){
        arr.clear();
        for(int i = 0; i < 100; i++){
            rand_num = std::rand() % 1000;
            arr.push_back(rand_num);
        }
        rand_value = std::rand() % 1000;
        // 确保数组里不含 rand_value
        while(std::find(arr.begin(), arr.end(), rand_value) != arr.end()){
            std::remove(arr.begin(), arr.end(), rand_value);
        }
        std::sort(arr.begin(), arr.end());
        index = fibonacci_search(arr, rand_value);
        passed = passed && (index == -1);
    }
    return passed;
}

/**
 * @brief 随机常规测试：覆盖存在单一、多个或没有目标值的情况
 */
bool random_tests(){
    bool passed = true;
    int rand_num, rand_value, index, real_value, num_tests = 10000;
    std::vector<int> arr;
    while(num_tests--){
        arr.clear();
        for(int i = 0; i < 100; i++){
            rand_num = std::rand() % 1000;
            arr.push_back(rand_num);
        }
        rand_value = std::rand() % 1000;
        std::sort(arr.begin(), arr.end());
        index = fibonacci_search(arr, rand_value);
        if(index != -1){
            real_value = arr[index];
            passed = passed && (real_value == rand_value);
        } else {
            passed = passed && (std::find(arr.begin(), arr.end(), rand_value) == arr.end());
        }
    }
    return passed;
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    assert(no_occurence_tests());
    assert(random_tests());
    std::cout << "All Fibonacci search tests passed successfully!" << std::endl;
    return 0;
}
