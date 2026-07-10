/**
 * @file
 * @brief Implementation of [Saddleback Algorithm](https://www.geeksforgeeks.org/saddleback-search-algorithm-in-a-2d-array) for 2D arrays (二维有序矩阵检索/Saddleback算法实现)
 *
 * @details
 * Saddleback 算法可在 O(m + n) 线性时间内完成对二维矩阵的查找（其中 m 为行数，n 为列数）。
 * 它要求目标二维矩阵的每一行和每一列均已预先排好序（升序）。
 *
 * @author [Hashir Niazi](https://github.com/HashirGJ8842)
 */
#include <cassert>   /// 用于 assert 断言
#include <cstdint>
#include <iostream>  /// 用于输入输出及 std::pair
#include <vector>    /// 用于 std::vector

/** \namespace search
 * \brief 搜索算法命名空间
 */
namespace search {
/** \namespace saddleback
 * \brief Saddleback 搜索算法相关命名空间
 */
namespace saddleback {
/**
 * @brief 在行和列都已排序的二维矩阵中执行 Saddleback 搜索算法
 * @details 
 * 从矩阵的右上角开始搜索 (行 = 0, 列 = 列数 - 1)。
 * 1. 若当前元素等于目标值，直接返回 1 起始的行列索引对 (row + 1, col + 1)。
 * 2. 若目标值大于当前元素，由于当前行左侧元素均更小，目标必不可能在该行，向下移动一行：`++left_index`。
 * 3. 若目标值小于当前元素，由于当前列下方元素均更大，目标必不可能在该列，向左移动一列：`--right_index`。
 * 
 * 这种单向缩减的贪心走法确保了最多遍历 m + n 次，时间复杂度为 O(m + n)。
 * 
 * @param matrix 行列均升序排好序的二维矩阵
 * @param element 待检索的目标值
 * @return 目标元素在矩阵中 1 起始的 (行, 列) 位置，找不到则返回 (0, 0)
 */
std::pair<uint32_t, uint32_t> saddleback(
    std::vector<std::vector<int32_t>> matrix, int32_t element) {
    uint32_t left_index = 0; // 行索引
    uint32_t right_index = matrix[0].size() - 1;  // 从右上角列索引开始

    // 在索引未越界的情况下循环查找
    while (left_index < matrix.size()) {  
        if (element == matrix[left_index][right_index]) {  
            // 命中目标，返回对应的 1-indexed 坐标位置对
            return std::make_pair(left_index + 1, right_index + 1);
        } else if (element > matrix[left_index][right_index]) {  
            // 目标值大于当前探测值，说明目标只能存在于下方，向下走
            ++left_index;
        } else if (element < matrix[left_index][right_index]) {  
            // 目标值小于当前探测值，说明目标只能存在于左侧，向左走
            if (!right_index)
                break; // 已到达最左列，无法继续向左，退出
            else
                --right_index;
        }
    }
    return std::make_pair(0, 0);  // 越界出矩阵，代表目标不存在，返回 (0, 0)
}
}  // namespace saddleback
}  // namespace search

/**
 * @brief 单元自测用例
 */
static void test() {
    // 5x5 的升序行列测试矩阵
    std::vector<std::vector<int32_t>> matrix = {{1, 10, 100, 1000, 10000},
                                                 {2, 20, 200, 2000, 20000},
                                                 {3, 30, 300, 3000, 30000},
                                                 {4, 40, 400, 4000, 40000},
                                                 {5, 50, 500, 5000, 50000}};

    std::pair<uint32_t, uint32_t> not_found = std::make_pair(0, 0);
    std::pair<uint32_t, uint32_t> test_answer;
    
    // 测试 1：查找不存在的元素 123
    std::pair<uint32_t, uint32_t> answer1 =
        search::saddleback::saddleback(matrix, 123);
    assert(not_found == answer1);
    
    // 测试 2：查找不存在的下界外元素 0
    answer1 = search::saddleback::saddleback(matrix, 0);
    assert(not_found == answer1);
    
    // 测试 3：查找左上角端点元素 1
    answer1 = search::saddleback::saddleback(matrix, 1);
    test_answer = std::make_pair(1, 1);
    assert(test_answer == answer1);
    
    // 测试 4：查找右下角端点元素 50000
    answer1 = search::saddleback::saddleback(matrix, 50000);
    test_answer = std::make_pair(5, 5);
    assert(test_answer == answer1);
    
    // 测试 5：查找内部元素 300
    answer1 = search::saddleback::saddleback(matrix, 300);
    test_answer = std::make_pair(3, 3);
    assert(test_answer == answer1);
    
    std::cout << "All Saddleback search tests passed!" << std::endl;
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行测试
    return 0;
}
