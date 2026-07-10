/**
 * \file
 * \brief [Linear search algorithm](https://en.wikipedia.org/wiki/Linear_search) (线性/顺序查找算法实现)
 *
 * @author Unknown author
 * @author [Ritika Mukherjee](https://github.com/ritikaa17)
 */

#include <cassert>   /// 用于 assert 断言
#include <iostream>  /// 用于标准输入输出

/**
 * \brief 顺序遍历数组寻找目标值
 * \param [in] array 待搜索的数组首地址指针
 * \param [in] size 数组大小
 * \param [in] key 待查找的目标键值
 * \returns 查找到的目标元素在数组中的索引；如果未找到则返回 -1
 */
int LinearSearch(int *array, int size, int key) {
    for (int i = 0; i < size; ++i) {
        if (array[i] == key) {
            return i; // 找到目标，立即返回索引
        }
    }
    // 遍历整个数组仍没有找到，返回 invalid 标志值 -1
    return -1;
}

/**
 * @brief 单元自测用例
 */
static void tests() {
    int size = 4;
    int *array = new int[size];
    for (int i = 0; i < size; i++) {
        array[i] = i;
    }

    assert(LinearSearch(array, size, 0) == 0);
    assert(LinearSearch(array, size, 1) == 1);
    assert(LinearSearch(array, size, 2) == 2);

    delete[] array;

    size = 6;
    array = new int[size];
    for (int i = 0; i < size; i++) {
        array[i] = i;
    }

    assert(LinearSearch(array, size, 3) == 3);
    assert(LinearSearch(array, size, 1) == 1);
    assert(LinearSearch(array, size, 5) == 5);

    std::cout << "All tests have successfully passed!\n";
    delete[] array;  // 释放分配的内存，避免泄露
}

/**
 * @brief 主函数，支持交互模式和自测模式
 * @returns 0
 */
int main() {
    int mode = 0;

    std::cout << "Choose mode\n";
    std::cout << "Self-test mode (1), interactive mode (2): ";
    std::cin >> mode;

    if (mode == 2) {
        // 交互输入模式
        int size = 0;
        std::cout << "\nEnter the size of the array [in range 1-30 ]: ";
        std::cin >> size;

        while (size <= 0 || size > 30) {
            std::cout << "Size can only be 1-30. Please choose another value: ";
            std::cin >> size;
        }

        int *array = new int[size];
        int key = 0;

        // 读入数组各元素
        std::cout << "Enter the array of " << size << " numbers: ";
        for (int i = 0; i < size; i++) {
            std::cin >> array[i];
        }

        std::cout << "\nEnter the number to be searched: ";
        std::cin >> key;

        int index = LinearSearch(array, size, key);
        if (index != -1) {
            std::cout << "Number found at index: " << index << "\n";
        } else {
            std::cout << "Array element not found\n";
        }
        delete[] array; // 释放内存
    } else {
        tests();  // 运行测试用例
    }
    return 0;
}
