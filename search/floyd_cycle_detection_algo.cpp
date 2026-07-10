/**
 * @file
 * @brief Implementation of [Floyd's Cycle Detection](https://en.wikipedia.org/wiki/Cycle_detection) algorithm (弗洛伊德判圈算法/快慢指针找重复数)
 * @details
 * 给定一个大小为 n + 1 的整数数组，其元素取值范围在 [1, n] 之间。
 * 根据鸽巢原理（Pigeonhole Principle），数组中必定存在至少一个重复的整数。
 * 本算法利用“快慢指针（Tortoise and Hare）”思想，在不修改原数组、O(1) 额外空间复杂度的限制下，以 O(N) 时间复杂度找出重复的数字。
 * 
 * ### 核心原理
 * 将数组索引 i 视为节点，将值 arr[i] 视为指向下一个节点的指针。由于存在重复数字，
 * 指针链条中必然会形成环。重复的数字即为环的入口节点。
 * 
 * @author [Swastika Gupta](https://github.com/Swastyy)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>
#include <iostream>  /// 用于输入输出
#include <vector>    /// 用于 std::vector

/**
 * @namespace search
 * @brief 搜索算法命名空间
 */
namespace search {
/**
 * @namespace cycle_detection
 * @brief 快慢指针判圈算法相关命名空间
 */
namespace cycle_detection {
/**
 * @brief 查找数组中的重复数字
 * @details
 * 1. 第一阶段（判断环存在）：慢指针 tortoise 每次走一步，快指针 hare 每次走两步。
 *    二者从起点出发后，若有环，快指针必定会在环中追上慢指针并相遇。
 * 2. 第二阶段（寻找环入口）：当二者相遇后，将慢指针重置回起点 (in_arr[0])，
 *    快慢指针此后均每次仅前进一步。当它们再次相遇时，相遇点即为环的入口（即重复的值）。
 * 
 * @tparam T 数组元素数据类型
 * @param in_arr 输入数组
 * @param n 数组的大小（应满足包含至少两个元素）
 * @returns 查找到的重复数字；若不满足条件则返回 -1
 */
template <typename T>
int32_t duplicateNumber(const std::vector<T> &in_arr, const uint32_t &n) {
    if (n == 0 || n == 1) { 
        return -1; // 寻找重复值数组大小应至少为 2
    }
    
    // 初始化快慢指针为起点值
    uint32_t tortoise = in_arr[0]; 
    uint32_t hare = in_arr[0];     
    
    // 阶段一：快慢指针在环内首次相遇
    do {
        tortoise = in_arr[tortoise];       // 慢指针走一步
        hare = in_arr[in_arr[hare]];       // 快指针走两步
    } while (tortoise != hare);            // 当二者相遇时退出循环
    
    // 阶段二：重置慢指针至起点，快慢指针同步单步前进，寻找环入口
    tortoise = in_arr[0];
    while (tortoise != hare) {
        tortoise = in_arr[tortoise];       // 慢指针走一步
        hare = in_arr[hare];               // 快指针也走一步
    }
    
    return tortoise; // 相遇点的值即为环的入口，也就是重复数
}
}  // namespace cycle_detection
}  // namespace search

/**
 * @brief 单元自测用例
 */
static void test() {
    // 测试 1
    // [3, 4, 8, 5, 9, 1, 2, 6, 7, 4] 应返回 4
    std::vector<uint32_t> array1 = {3, 4, 8, 5, 9, 1, 2, 6, 7, 4};
    std::cout << "Test 1... ";
    assert(search::cycle_detection::duplicateNumber(array1, array1.size()) == 4);
    std::cout << "passed" << std::endl;

    // 测试 2
    // [1, 2, 3, 4, 2] 应返回 2
    std::vector<uint32_t> array2 = {1, 2, 3, 4, 2};
    std::cout << "Test 2... ";
    assert(search::cycle_detection::duplicateNumber(array2, array2.size()) == 2);
    std::cout << "passed" << std::endl;

    // 测试 3
    // 空数组，应返回 -1
    std::vector<uint32_t> array3 = {};
    std::cout << "Test 3... ";
    assert(search::cycle_detection::duplicateNumber(array3, array3.size()) == -1);
    std::cout << "passed" << std::endl;
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行测试
    return 0;
}
