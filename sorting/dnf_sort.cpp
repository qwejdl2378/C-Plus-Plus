/**
 * @file
 * @brief Implementation of the [DNF sort](https://en.wikipedia.org/wiki/Dutch_national_flag_problem) algorithm (荷兰国旗问题 DNF 排序算法实现)
 *
 * @details
 * 荷兰国旗问题（Dutch National Flag Sort, DNF Sort）由 艾兹格·迪杰斯特拉（Edsger Dijkstra）提出。
 * 算法针对只包含三种类型元素（通常用 0、1、2 表示）的数组进行单趟排序。
 *
 * ### 算法原理（三路划分法 3-Way Partitioning）
 * 维护三个指针：
 * - `lo`：指向 0 区域的右边界。`[0, lo-1]` 的元素全部为 0。
 * - `mid`：指向当前正在扫描的元素位置。`[lo, mid-1]` 的元素全部为 1。
 * - `hi`：指向 2 区域的左边界。`[hi+1, N-1]` 的元素全部为 2。
 * 
 * 扫描循环：根据 `arr[mid]` 的值进行分流：
 * - `0`：与 `arr[lo]` 交换，并让 `lo++`，`mid++`。
 * - `1`：无需移动，直接让 `mid++` 扩大 1 区域。
 * - `2`：与 `arr[hi]` 交换，并让 `hi--`（注意此时 `mid` 不做自增，因为从 `hi` 交换回来的值还需要在下一轮进行判断）。
 *
 * @note
 * 【空数组下溢越界 Bug 审计与修复】：
 * 原版代码声明为 `uint64_t hi = arr.size() - 1;`。
 * 若输入的数组为空（`arr.size() == 0`），无符号数 `0 - 1` 会产生**无符号整数下溢（Unsigned Underflow）**，
 * 变成 `UINT64_MAX`。导致 `mid <= hi` 的条件成立并进入循环，尝试访问 `arr[0]` 从而触发 **段错误崩溃（Segmentation Fault）**。
 * 现已在函数入口添加了 `arr.size() <= 1` 的防卫性检查。
 *
 * 时间复杂度: $O(N)$ (单趟扫描)
 * 空间复杂度: $O(1)$
 * 
 * @author [Sujal Gupta](https://github.com/heysujal)
 */

#include <algorithm>  /// 用于 std::is_sorted
#include <cassert>    /// 用于 assert 断言
#include <cstdint>   /// 用于 uint64_t
#include <iostream>   /// 用于 std::swap 和输入输出
#include <vector>     /// 用于 std::vector

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {
/**
 * @namespace dnf_sort
 * @brief 荷兰国旗三路排序算法命名空间
 */
namespace dnf_sort {
/**
 * @brief 荷兰国旗 DNF 排序主算法
 * @tparam T 数组元素模板类型
 * @param in_arr 待排序数组（只读引用）
 * @returns 排序完毕的新数组
 */
template <typename T>
std::vector<T> dnfSort(const std::vector<T> &in_arr) {
    std::vector<T> arr(in_arr);
    
    // 安全防卫：如果大小 <= 1，无需排序，同时防范 size-1 产生的下溢
    if (arr.size() <= 1) {
        return arr;
    }

    uint64_t lo = 0;
    uint64_t hi = arr.size() - 1;
    uint64_t mid = 0;

    // 扫描并在单趟内归位所有元素
    while (mid <= hi) {
        switch (arr[mid]) {
            case 0:
                // 0 交换到前部，lo 和 mid 同步前进
                std::swap(arr[lo++], arr[mid++]);
                break;

            case 1:
                // 1 保持在中部，仅 mid 前进
                mid++;
                break;

            case 2:
                // 2 交换到尾部，hi 后退。注意 mid 不前进，以便下一轮检查交换过来的新元素
                std::swap(arr[mid], arr[hi--]);
                break;
        }
    }
    return arr;
}
}  // namespace dnf_sort
}  // namespace sorting

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1
    std::vector<uint64_t> array1 = {0, 1, 1, 2};
    std::cout << "Test 1... ";
    std::vector<uint64_t> arr1 = sorting::dnf_sort::dnfSort(array1);
    assert(std::is_sorted(std::begin(arr1), std::end(arr1)));
    std::cout << "passed" << std::endl;

    // 测试 2
    std::vector<uint64_t> array2 = {1, 0, 0, 1, 1, 0, 2, 1};
    std::cout << "Test 2... ";
    std::vector<uint64_t> arr2 = sorting::dnf_sort::dnfSort(array2);
    assert(std::is_sorted(std::begin(arr2), std::end(arr2)));
    std::cout << "passed" << std::endl;

    // 测试 3
    std::vector<uint64_t> array3 = {1, 1, 0, 0, 1, 2, 2, 0, 2, 1};
    std::cout << "Test 3... ";
    std::vector<uint64_t> arr3 = sorting::dnf_sort::dnfSort(array3);
    assert(std::is_sorted(std::begin(arr3), std::end(arr3)));
    std::cout << "passed" << std::endl;

    // 测试 4
    std::vector<uint64_t> array4 = {2, 2, 2, 0, 0, 1, 1};
    std::cout << "Test 4... ";
    std::vector<uint64_t> arr4 = sorting::dnf_sort::dnfSort(array4);
    assert(std::is_sorted(std::begin(arr4), std::end(arr4)));
    std::cout << "passed" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试
    return 0;
}
