/**
 * @file
 * @brief Implementation of numeric string sorting using a custom comparator (基于自定义比较器的数字字符串自然排序实现)
 *
 * @details
 * 传统字符串排序采用的是字典序（Alphanumeric Sort）。
 * 例如，对于字符串数组 `["1", "10", "2", "20"]`，默认字典序排序结果是 `["1", "10", "2", "20"]`。
 * 但我们知道，以数值大小进行排序的正确结果应该是 `["1", "2", "10", "20"]`。
 * 
 * 本程序设计了一个数值比较器 `NumericSort`，用于实现数字字符串的“自然数值顺序（Numeric Order）”排序。
 *
 * ### 比较器逻辑
 * 1. 过滤前导零（例如 `"007"` 净化为 `"7"`）。
 * 2. 比较净化后字符串的长度：长度较长者代表其对应的数值较大（例如 `"10"` 比 `"9"` 长，因此 10 > 9）。
 * 3. 若长度相等，则直接进行字典序比较（例如 `"12"` 和 `"15"` 长度相同，按字符大小比较 `"12" < "15"`）。
 *
 * @note
 * 【空串越界隐患与效率改进审计】：
 * 1. **空安全防卫**：原代码直接写为 `while (a[0] == '0')`。如果输入包含空字符串 `""`，
 *    或者经过前导零擦除后变成了空串，直接访问 `a[0]` 虽然在现代 C++11 下会返回 `\0`，
 *    但在老旧编译器或特定边界下极易产生越界未定义行为。现已修正为安全的 `!a.empty() && a[0] == '0'`。
 * 2. **擦除效率隐患**：原代码使用 `a.erase(a.begin())` 在循环中删除前导零。
 *    在 `std::string` 中删除首字符会导致后续所有字符向前平移，时间复杂度为 $O(L^2)$。
 *    若字符串长度极长（例如大数比较），会产生严重的效率损耗。在工业界推荐改用 `std::string_view` 指针偏移进行 $O(1)$ 级别匹配。
 *
 * 时间复杂度: 比较单次耗时 $O(L)$，总时间复杂度 $O(N \log N \cdot L)$，其中 L 为数字最大长度。
 * 空间复杂度: $O(L)$ (在比较器传参和擦除时产生了临时副本)
 * 
 * @author Unknown author
 */

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief 数字字符串自然数值排序比较器
 * @param a 第一个数字字符串（值传递，在内部进行修改）
 * @param b 第二个数字字符串（值传递，在内部进行修改）
 * @returns 若数值 a 小于数值 b 返回 true，否则返回 false
 */
bool NumericSort(std::string a, std::string b) {
    // 1. 安全过滤 a 的前导零
    while (!a.empty() && a[0] == '0') {
        a.erase(a.begin());
    }
    // 2. 安全过滤 b 的前导零
    while (!b.empty() && b[0] == '0') {
        b.erase(b.begin());
    }

    int n = a.length();
    int m = b.length();

    // 3. 长度相同，通过字典序比大小
    if (n == m) {
        return a < b;
    }
    // 4. 长度不同，长度长的数值大
    return n < m;
}

/**
 * @brief 主函数
 */
int main() {
    int n = 0;
    std::cout << "Enter number of elements to be sorted Numerically: ";
    std::cin >> n;
    if (n <= 0) {
        return 0;
    }

    std::vector<std::string> v(n);
    std::cout << "Enter the string of Numbers:" << std::endl;
    for (int i = 0; i < n; i++) {
        std::cin >> v[i];
    }

    // 1. 常规字典序排序演示
    std::vector<std::string> normal_sorted(v);
    std::sort(normal_sorted.begin(), normal_sorted.end());
    std::cout << "Elements sorted normally: \n";
    for (int i = 0; i < n; i++) {
        std::cout << normal_sorted[i] << " ";
    }
    std::cout << "\n";

    // 2. 自定义数值比较器排序演示
    std::sort(v.begin(), v.end(), NumericSort);
    std::cout << "Elements sorted Numerically: \n";
    for (int i = 0; i < n; i++) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
