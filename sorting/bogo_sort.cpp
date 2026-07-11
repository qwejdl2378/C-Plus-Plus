/**
 * @file
 * @brief Implementation of [Bogosort algorithm](https://en.wikipedia.org/wiki/Bogosort) (猴子排序 / 愚蠢排序算法实现)
 *
 * @details
 * 猴子排序（Bogosort，又称 stupid sort, random sort, monkey sort, shuffle sort 等）是一种极度低效的排序算法。
 * 它采用“生成与测试”（generate and test）的策略：
 * 1. 检查当前数组是否有序。
 * 2. 如果无序，随机打乱（Shuffle）整个数组，然后再重新检查。
 * 3. 循环往复，直到运气爆发打乱出一个完全有序的数组为止。
 *
 * ### 性能警告
 * - 平均时间复杂度: $O(N \cdot N!)$。
 * - 最坏时间复杂度: 无界（Infinite），理论上有概率永远无法排好序。
 * - 因此，该算法仅具有理论教学和娱乐意义，**绝对不能用于任何实际开发**，且测试数组大小通常限制在 $N \le 10$ 以内（否则运行时间将以年为单位计算）。
 *
 * @author [Deep Raval](https://github.com/imdeep2905)
 */

#include <iostream>
#include <algorithm>
#include <array>
#include <cassert>
#include <random>

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {
/**
 * @brief 自定义打乱函数（未被核心排序调用，仅作算法思路参考）
 * @tparam T 数组元素类型
 * @tparam N 数组长度
 * @param arr 传入的数组副本
 * @returns 打乱后的新数组
 */
template <typename T, size_t N>
std::array <T, N> shuffle (std::array <T, N> arr) {
    for (size_t i = 0; i < N; i++) {
        // 随机交换当前元素与任一元素位置
        std::swap(arr[i], arr[std::rand() % N]);
    }
    return arr;
}

/**
 * @brief 随机猴子排序主函数
 * @tparam T 数组元素类型
 * @tparam N 数组长度
 * @param arr 待排序数组
 * @returns 排序完毕的新数组
 */
template <typename T, size_t N>
std::array <T, N> randomized_bogosort (std::array <T, N> arr) {
    std::random_device random_device;
    std::mt19937 generator(random_device()); // 使用真随机种子初始化梅森旋转算法生成器
    
    // 只要数组尚未排好序，就不断随机打乱
    while (!std::is_sorted(arr.begin(), arr.end())) {
        std::shuffle(arr.begin(), arr.end(), generator); // 随机洗牌
    }
    return arr;
}

}  // namespace sorting

/**
 * @brief 打印数组内容
 */
template <typename T, size_t N>
void show_array (const std::array <T, N> &arr) {
    for (int x : arr) {
        std::cout << x << ' ';
    }
    std::cout << '\n';
}

/**
 * @brief 单元自测用例
 */
void test() {
    // 测试 1
    std::array <int, 5> arr1;
    for (int &x : arr1) {
        x = std::rand() % 100;
    }
    std::cout << "Original Array : ";
    show_array(arr1);
    arr1 = sorting::randomized_bogosort(arr1);
    std::cout << "Sorted Array : ";
    show_array(arr1);
    assert(std::is_sorted(arr1.begin(), arr1.end()));

    // 测试 2
    std::array <int, 5> arr2;
    for (int &x : arr2) {
        x = std::rand() % 100;
    }
    std::cout << "Original Array : ";
    show_array(arr2);
    arr2 = sorting::randomized_bogosort(arr2);
    std::cout << "Sorted Array : ";
    show_array(arr2);
    assert(std::is_sorted(arr2.begin(), arr2.end()));
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行自测

    std::array <int, 5> arr = {3, 7, 10, 4, 1}; 
    std::cout << "Original Array : ";
    show_array(arr);
    
    arr = sorting::randomized_bogosort(arr); 
    std::cout << "Sorted Array : ";
    show_array(arr); 
    
    return 0;
}
