/**
 * @file
 * @brief Implementation of [Bubble Sort](https://en.wikipedia.org/wiki/Bubble_sort) algorithm (优化版冒泡排序算法实现)
 *
 * @details
 * 冒泡排序（Bubble Sort）是一种简单的排序算法。它重复地走访过要排序的数列，
 * 一次比较两个元素，如果它们的顺序错误就把它们交换过来。
 * 走访数列的工作是重复地进行直到没有再需要交换，也就是说该数列已经排序完成。
 * 元素就像气泡一样，最大（或最小）的元素会逐步“浮”到数列的末尾。
 *
 * ### 优化策略（使用 swap_check）
 * - 引入 `swap_check` 布尔标记。如果在某趟遍历中**没有发生任何交换**，
 *   说明整个数组已经处于完全有序状态，算法可以**提前终止**，无需继续执行剩余的循环趟数。
 * - 这一优化使得算法在**最有利情况（输入数组本身已是有序的）**下的时间复杂度降为 $O(N)$。
 *
 * 时间复杂度:
 *   - 最好情况: $O(N)$ (已排序数组)
 *   - 最坏情况: $O(N^2)$ (逆序数组)
 *   - 平均情况: $O(N^2)$
 * 空间复杂度: $O(1)$
 *
 * @author [Deepak](https://github.com/Deepak-j-p)
 * @author [Nguyen Phuc Chuong](https://github.com/hollowcrust)
 */

#include <algorithm> /// 用于 std::is_sorted
#include <cassert>   /// 用于 assert 断言
#include <iostream>  /// 用于输入输出
#include <string>    /// 用于 std::string
#include <utility>   /// 用于 std::pair, std::swap
#include <vector>    /// 用于 std::vector

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {
/**
 * @namespace bubble_sort
 * @brief 冒泡排序算法命名空间
 */
namespace bubble_sort {
/**
 * @brief 优化版冒泡排序主函数
 * @tparam T 数组元素模板类型
 * @param array 待排序数组的引用
 * @returns 排序好（升序）的数组
 */
template <typename T> 
std::vector<T> bubble_sort(std::vector<T>& array) {
  // swap_check 标记用来检测单趟内是否发生了交换，若无交换则直接提前终止
  bool swap_check = true;
  int size = array.size();
  
  for (int i = 0; (i < size) && (swap_check); i++) {
    swap_check = false; // 初始设为无交换
    for (int j = 0; j < size - 1 - i; j++) {
      if (array[j] > array[j + 1]) {
        swap_check = true; // 发生交换，更新标记
        std::swap(array[j], array[j + 1]);
      }
    }
  }

  return array;
}
} // namespace bubble_sort
} // namespace sorting

/**
 * @brief 单元自测用例
 */
static void test() {
  // 测试 1
  std::vector<int> vec_1 = {3, 1, -9, 0};
  std::vector<int> sorted_1 = sorting::bubble_sort::bubble_sort(vec_1);

  // 测试 2: 只有一个元素
  std::vector<int> vec_2 = {3};
  std::vector<int> sorted_2 = sorting::bubble_sort::bubble_sort(vec_2);

  // 测试 3: 全部相同的元素
  std::vector<int> vec_3 = {10, 10, 10, 10, 10};
  std::vector<int> sorted_3 = sorting::bubble_sort::bubble_sort(vec_3);

  // 测试 4: 浮点数类型
  std::vector<float> vec_4 = {1234, -273.1, 23, 150, 1234, 1555.55, -2000};
  std::vector<float> sorted_4 = sorting::bubble_sort::bubble_sort(vec_4);

  // 测试 5: 字符类型
  std::vector<char> vec_5 = {'z', 'Z', 'a', 'B', ' ', 'c', 'a'};
  std::vector<char> sorted_5 = sorting::bubble_sort::bubble_sort(vec_5);

  // 测试 6: 字符串类型
  std::vector<std::string> vec_6 = {"Hello", "hello", "Helo", "Hi", "hehe"};
  std::vector<std::string> sorted_6 = sorting::bubble_sort::bubble_sort(vec_6);

  // 测试 7: 对组类型 (std::pair)
  std::vector<std::pair<int, char>> vec_7 = {{10, 'c'}, {2, 'z'}, {10, 'a'}, {0, 'b'}, {-1, 'z'}};
  std::vector<std::pair<int, char>> sorted_7 = sorting::bubble_sort::bubble_sort(vec_7);

  assert(std::is_sorted(sorted_1.begin(), sorted_1.end()));
  assert(std::is_sorted(sorted_2.begin(), sorted_2.end()));
  assert(std::is_sorted(sorted_3.begin(), sorted_3.end()));
  assert(std::is_sorted(sorted_4.begin(), sorted_4.end()));
  assert(std::is_sorted(sorted_5.begin(), sorted_5.end()));
  assert(std::is_sorted(sorted_6.begin(), sorted_6.end()));
  assert(std::is_sorted(sorted_7.begin(), sorted_7.end()));
}

/**
 * @brief 主函数
 */
int main() {
  test(); // 运行自测
  return 0;
}
