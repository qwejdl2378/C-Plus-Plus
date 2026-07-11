/**
 * @file
 * @brief Demonstration of dynamic 2D jagged array allocation and lookup (动态二维交错数组分配与查找演示)
 * @details
 * 本程序演示了如何在运行时动态分配一个具有可变列数的二维交错数组（Jagged Array），并支持用户通过行、列索引进行实时查询。
 * 
 * @note
 * 【命名空间与分类提示】：
 * 尽管该文件被放在了 `dynamic_programming` 目录中，但它仅仅是关于“动态内存分配（Dynamic Memory Allocation）”的指针演示，
 * 与“动态规划（Dynamic Programming）”算法无任何关系。
 *
 * 【严重缺陷分析】：
 * 1. **内存泄漏（Memory Leak）**：程序使用 `new[]` 运算符在堆上分配了大量的行级内存（`ac`）和二维指针数组（`ar`），
 *    但是直到程序结束也未调用对应的 `delete[]` 进行资源回收。这在实际的 C++ 项目中属于严重内存泄漏。
 * 2. **越界访问风险（Out-of-Bounds Risk）**：在查询部分，如果用户输入的行号 `r1` 超过了存储数组范围 `x`，
 *    或者输入的列号 `q1` 超出了当前行实际分配的大小 `mr`，程序会直接读取非法地址从而导致段错误（Segmentation Fault）崩溃。
 */

#include <iostream>

/**
 * @brief 主函数
 */
int main() {
    int64_t r, mr = 0, x, q, i, z;
    std::cout << "Enter Number of array you want to Store :";
    std::cin >> x;
    std::cout << "Enter Number of Question or Quary you want to do Related to Array :";
    std::cin >> q;

    // 动态分配大小为 x 的二级指针数组，用于保存各行（1D 数组）的首地址
    int** ar = new int*[x]();

    // 循环录入每行元素数，并动态分配对应大小的 1D 数组
    for (r = 0; r < x; r++) {
        std::cout << "Enter number of element in " << r + 1 << " rows :";
        std::cin >> mr;
        
        // 分配第 r 行的物理空间
        int* ac = new int[mr]();
        std::cout << "Enter the element of Array ";
        
        // 录入当前行的每个数值
        for (i = 0; i < mr; i++) {
            std::cin >> ac[i];
        }
        // 将当前行数组的地址存入二级指针数组中
        ar[r] = ac;
    }

    // 循环执行用户指定的 Q 次查询
    for (z = 0; z < q; z++) {
        int64_t r1 = 0, q1 = 0;
        std::cout << "enter the number of row which element you want to find :";
        std::cin >> r1;
        r1 = r1 - 1; // 转换为 0-indexed 索引
        
        std::cout << "enter the position of element which you want to find :";
        std::cin >> q1;
        q1 = q1 - 1; // 转换为 0-indexed 索引
        
        // 警告：如果 r1 < 0 或 r1 >= x，或者 q1 越界，此处将发生未定义行为或崩溃
        std::cout << "The element is " << ar[r1][q1] << std::endl;
    }

    // 提示：此处遗漏了对应的内存释放代码：
    // for (r = 0; r < x; r++) { delete[] ar[r]; }
    // delete[] ar;

    return 0;
}
