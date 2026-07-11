/**
 * @file
 * @brief Implementation of [Gnome Sort](https://en.wikipedia.org/wiki/Gnome_sort) algorithm (侏儒排序 / 地精排序算法实现)
 *
 * @details
 * 地精排序（Gnome Sort）也被称为“花园侏儒排序（Garden Gnome Sort）”。
 * 它的思想和插入排序非常类似，但是其代码实现极其简洁，没有嵌套循环，只靠单层 `while` 和一个 `index` 变量的前进与后退完成排序。
 *
 * ### 侏儒视角下的排序直观隐喻：
 * 侏儒在给一排花园花盆排序（按高度）。
 * 1. 他看相邻的两个花盆。如果它们的顺序正确，他就向前跨一步（`index++`）。
 * 2. 如果顺序错误，他就把这两个花盆交换，并且向后退一步（`index--`），以便继续校验被换到前方的花盆是否依然比前一个矮。
 * 3. 边界处理：如果退到了最起点（`index == 0`），他就必须向前跨一步（`index++`）。
 * 4. 当他走到队列最尾端（`index == size`）时，排序结束。
 *
 * ### 稳定性
 * - 地精排序是**稳定的（Stable）**。因为条件 `arr[index] >= arr[index - 1]` 包含了等于号，
 *   相同值的相邻项不会执行交换，直接让 `index++` 前进，从而保持了等值元素的初始相对顺序不变。
 *
 * 时间复杂度:
 *   - 最好情况: $O(N)$ (数组已是有序，侏儒一路直行向前)
 *   - 最坏情况: $O(N^2)$ (逆序数组，侏儒进一退一，相当于插入排序的不断回推)
 *   - 平均情况: $O(N^2)$
 * 空间复杂度:
 *   - 动态指针版本: $O(1)$
 *   - 容器 std::array 值传递版本: $O(N)$（由于发生了完整副本构造）
 *
 * @author [beqakd](https://github.com/beqakd)
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <algorithm>  // 用于 std::swap
#include <array>      // 用于 std::array
#include <cassert>    // 用于 assert 断言
#include <iostream>   // 用于输入输出

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {
/**
 * @brief 针对 C 风格原始指针数组的地精排序（就地排序）
 * @tparam T 元素模板类型
 * @param arr 待排序数组指针
 * @param size 数组长度
 */
template <typename T>
void gnomeSort(T *arr, int size) {
    if (size <= 1) {
        return;
    }

    int index = 0;
    while (index < size) {
        // 如果当前是第一个元素，或者大小顺序正确，则继续前进
        if ((index == 0) || (arr[index] >= arr[index - 1])) {
            index++;
        } 
        // 发现逆序对，执行交换并回退一步以进一步追踪
        else {
            std::swap(arr[index], arr[index - 1]);
            index--;
        }
    }
}

/**
 * @brief 针对 C++ 容器 std::array 的地精排序（传值产生副本）
 * @tparam T 元素模板类型
 * @tparam size 容器长度静态常量
 * @param arr 传入的数组容器副本
 * @returns 排序完毕的新容器
 */
template <typename T, size_t size>
std::array<T, size> gnomeSort(std::array<T, size> arr) {
    if (size <= 1) {
        return arr;
    }

    int index = 0;
    while (index < size) {
        if ((index == 0) || (arr[index] >= arr[index - 1])) {
            index++;
        } else {
            std::swap(arr[index], arr[index - 1]);
            index--;
        }
    }
    return arr;
}
}  // namespace sorting

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1: 使用 C 风格指针就地排序
    std::cout << "Test 1 - as a C-array...";
    const int size = 6;
    std::array<int, size> arr = {-22, 100, 150, 35, -10, 99};
    sorting::gnomeSort(arr.data(), size);
    assert(std::is_sorted(std::begin(arr), std::end(arr)));
    std::cout << " Passed\n";
    for (int i = 0; i < size; i++) {
        std::cout << arr[i] << ", ";
    }
    std::cout << std::endl;

    // 测试 2: 使用 std::array 容器并返回新数组
    std::cout << "\nTest 2 - as a std::array...";
    std::array<double, size> double_arr = {-100.2, 10.2, 20.0, 9.0, 7.5, 7.2};
    std::array<double, size> sorted_arr = sorting::gnomeSort(double_arr);
    assert(std::is_sorted(std::begin(sorted_arr), std::end(sorted_arr)));
    std::cout << " Passed\n";
    for (int i = 0; i < size; i++) {
        std::cout << sorted_arr[i] << ", ";
    }
    std::cout << std::endl;

    // 测试 3: 生成 200 个随机数进行排序
    std::cout << "\nTest 3 - 200 random numbers as a std::array...";
    const int size2 = 200;
    std::array<float, size2> rand_arr{};

    for (auto &a : rand_arr) {
        a = float(std::rand() % 1000 - 500) / 100.f;
    }

    std::array<float, size2> float_arr = sorting::gnomeSort(rand_arr);
    assert(std::is_sorted(std::begin(float_arr), std::end(float_arr)));
    std::cout << " Passed\n\n";
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行单元测试
    return 0;
}
