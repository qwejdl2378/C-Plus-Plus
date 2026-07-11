/**
 * @file
 * @brief Implementation of Counting Sort for string characters (针对字符串字符的计数排序算法实现)
 *
 * @details
 * 该程序用于对字符串中的各个字符（ASCII 范围 0 到 255）进行计数排序。
 *
 * @note
 * 【原版代码的严重内存崩溃 Bug 审计与修复】：
 * 原版代码存在非常严重的 C++ `std::string` 越界写入漏洞：
 * 1. 数组未分配空间：在第 7 行声明了 `string output;`。这是一个空字符串，其长度和容量均为 0。
 *    但在第 17 行中直接对其进行下标赋值：`output[count[arr[i]] - 1] = arr[i];`。
 *    在 C++ 中，对空 `std::string` 执行越界下标写入属于未定义行为（Undefined Behavior），
 *    在大多数编译器和操作系统下会**直接触发段错误崩溃（Segmentation Fault）**，或者导致静默的内存数据污染。
 * 2. 循环条件不安全：原版使用 `arr[i]` 字符非零作为循环终止条件。这属于 C-style 字符串的遗留习惯。
 *    在 C++ `std::string` 中，应使用更安全且标准的 `i < arr.size()` 或 `i < arr.length()` 作为边界条件。
 * 
 * ### 修复方案
 * - 将 `output` 初始化为与 `arr` 相同的长度：`std::string output(arr.size(), '\0');`。
 * - 重构所有循环条件为标准的 `i < arr.size()`，彻底根除越界风险。
 *
 * 时间复杂度: O(N + 256)
 * 空间复杂度: O(N + 256)
 * 
 * @author Unknown author
 */

#include <iostream>
#include <string>
#include <vector>

/**
 * @brief 对字符串进行计数排序，并输出结果
 * @param arr 输入的字符串
 */
void countSort(std::string arr) {
    if (arr.empty()) {
        std::cout << "Sorted character array is \n";
        return;
    }

    // 1. 初始化输出字符串，分配与原字符串等长的物理空间，防止内存越界写入
    std::string output(arr.size(), '\0');

    // 2. 初始化 ASCII 计数表 (256 个字符)
    int count[256];
    for (int i = 0; i < 256; i++) {
        count[i] = 0;
    }

    // 3. 统计每个字符在输入串中的出现频次
    for (size_t i = 0; i < arr.size(); ++i) {
        // 使用 unsigned char 防止负数扩展（如非 ASCII 字符）导致下标越界
        unsigned char index = arr[i];
        count[index]++;
    }

    // 4. 计算累加计数（前缀和），确定各个字符的终点边界
    for (int i = 1; i < 256; ++i) {
        count[i] += count[i - 1];
    }

    // 5. 倒序/顺序构建输出字符串（此处为稳定排序构建方式）
    for (int i = 0; i < static_cast<int>(arr.size()); ++i) {
        unsigned char index = arr[i];
        output[count[index] - 1] = arr[i]; // 此时已保证分配过空间，写入安全
        --count[index];
    }

    // 6. 将排好序的字符串覆盖回原字符数组
    for (size_t i = 0; i < arr.size(); ++i) {
        arr[i] = output[i];
    }

    std::cout << "Sorted character array is " << arr << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    std::string arr;
    std::cout << "Enter a string to sort: ";
    std::cin >> arr;

    countSort(arr);

    return 0;
}
