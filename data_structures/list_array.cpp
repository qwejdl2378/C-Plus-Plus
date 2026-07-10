/**
 * @file
 * @brief [Dynamic Array](https://en.wikipedia.org/wiki/Dynamic_array) (基于静态数组实现的顺序表)
 *
 * @details
 * list_array 是使用 `std::array` 模拟顺序表（类似于动态数组 std::vector 的一种静态上限版本）。
 * 支持增删改查 (CRUD) 以及选择排序、二分查找等算法。
 *
 * ### 核心功能说明
 * 1. `insert`：支持向未排序顺序表的末尾追加插入，也支持向已排序顺序表执行保序插入。
 * 2. `search`：如果数组已被排序（isSorted为true），采用 O(log N) 的二分查找；否则采用 O(N) 的线性顺序查找。
 * 3. `remove`：删除指定值的元素，并整体左移填补空位。
 */

#include <array>     /// 用于 std::array
#include <cassert>   /// 用于 assert 断言
#include <cstdint>
#include <iostream>  /// 用于标准输入输出

/**
 * @namespace data_structures
 * @brief 数据结构算法命名空间
 */
namespace data_structures {
/**
 * @namespace list_array
 * @brief 顺序表算法相关命名空间
 */
namespace list_array {
/**
 * @brief 顺序表结构体，支持最大容量 N
 * @tparam N 顺序表数组的最大容量上限
 */
template <uint64_t N>
struct list {
    std::array<uint64_t, N> data{};  // 实际物理存储的数组
    uint64_t top = 0;                // 当前顺序表有效元素的实际个数
    bool isSorted = false;           // 标识当前顺序表是否已经排序过

    /**
     * @brief 递归实现二分查找算法 (Binary Search)
     * @param dataArr 目标数组引用
     * @param first 搜索区间的左边界（包含）
     * @param last 搜索区间的右边界（包含）
     * @param val 待查找的目标值
     * @return 查找到的目标元素索引，如果不存在则返回 -1 (即 uint64_t 最大值)
     */
    uint64_t BinarySearch(const std::array<uint64_t, N> &dataArr,
                          const uint64_t &first, const uint64_t &last,
                          const uint64_t &val) {
        // 如果左右指针交错，说明元素不存在
        if (last < first) {
            return -1;
        }
        uint64_t mid = (first + last) / 2;
        // 检查中点的值是否命中
        if (dataArr[mid] == val)
            return mid;
        // 若目标值小于中点值，则递归在左半区间搜索
        else if (val < dataArr[mid])
            return (BinarySearch(dataArr, first, mid - 1, val));
        // 若目标值大于中点值，则递归在右半区间搜索
        else if (val > dataArr[mid])
            return (BinarySearch(dataArr, mid + 1, last, val));

        std::cerr << __func__ << ":" << __LINE__ << ": Undefined condition\n";
        return -1;
    }

    /**
     * @brief 顺序扫描线性查找算法 (Linear Search)
     * @param dataArr 目标数组引用
     * @param val 待查找的目标值
     * @return 索引值，不存在返回 -1
     */
    uint64_t LinearSearch(const std::array<uint64_t, N> &dataArr,
                          const uint64_t &val) const {
        for (uint64_t i = 0; i < top; i++) {
            if (dataArr[i] == val) {
                return i;  // 命中，返回索引位置
            }
        }
        return -1; // 未找到
    }

    /**
     * @brief 统一查找接口
     * @details 如果已排好序，调用二分查找；否则调用线性查找。
     * @param val 目标值
     * @return 对应的索引，没找到返回 -1 (uint64_t 最大值)
     */
    uint64_t search(const uint64_t &val) {
        uint64_t pos;
        if (isSorted) {
            pos = BinarySearch(data, 0, top - 1, val);
        } else {
            pos = LinearSearch(data, val);
        }
        
        if (pos != -1) {
            std::cout << "\nElement found at position : " << pos;
        } else {
            std::cout << "\nElement not found";
        }
        return pos;
    }

    /**
     * @brief 使用选择排序（Selection Sort）对顺序表进行原地排序
     */
    void sort() {
        for (uint64_t i = 0; i < top; i++) {
            uint64_t min_idx = i;
            for (uint64_t j = i + 1; j < top; j++) {
                if (data[j] < data[min_idx]) {
                    min_idx = j;
                }
            }
            std::swap(data[min_idx], data[i]); // 交换最小值
        }
        isSorted = true; // 标志位置为已排序
    }

    /**
     * @brief 插入新元素到顺序表中
     * @param val 待插入的元素值
     */
    void insert(const uint64_t &val) {
        if (top == N) {
            std::cout << "\nOverflow"; // 溢出提示
            return;
        }
        
        if (!isSorted) {
            // 如果未排序，直接在末尾追加
            data[top] = val;
            top++;
        } else {
            // 如果已经排好序，先找到正确的保序插入点
            uint64_t pos = 0;
            for (uint64_t i = 0; i < top - 1; i++) {
                if (data[i] <= val && val <= data[i + 1]) {
                    pos = i + 1;
                    break;
                }
            }
            // 如果插入的值比已有的所有值都大，插入在最末尾
            if (pos == 0) {
                pos = top - 1;
            }
            // 将插入点之后的所有元素向右平移一位
            for (uint64_t i = top; i > pos; i--) {
                data[i] = data[i - 1];
            }
            top++;
            data[pos] = val; // 在空出来的位置放置新值
        }
    }

    /**
     * @brief 从顺序表中删除匹配的指定元素
     * @param val 待删除的数据值
     */
    void remove(const uint64_t &val) {
        uint64_t pos = search(val); // 先查找其所在的索引位置
        if (pos == -1) {
            std::cout << "\n Element does not present in the list ";
            return;
        }
        std::cout << "\n" << data[pos] << " deleted";
        
        // 将删除位置之后的所有元素向左平移一位以填补空洞
        for (uint64_t i = pos; i < top; i++) {
            data[i] = data[i + 1];
        }
        top--; // 有效元素总数减 1
    }

    /**
     * @brief 显示顺序表中的所有有效数据
     */
    void show() {
        std::cout << '\n';
        for (uint64_t i = 0; i < top; i++) {
            std::cout << data[i] << " ";
        }
    }
};
}  // namespace list_array
}  // namespace data_structures

/**
 * @brief 单元自测用例集
 */
static void test() {
    data_structures::list_array::list<50> L;

    // 插入测试
    L.insert(11);
    L.insert(12);
    assert(L.top == 2);
    L.insert(15);
    L.insert(10);
    L.insert(12);
    L.insert(20);
    L.insert(18);
    assert(L.top == 7);
    L.show();

    // 删除测试
    L.remove(12);  ///< 删除重复值
    L.remove(15);  ///< 删除存在的值
    assert(L.top == 5);
    L.remove(50);  ///< 尝试删除不存在的值
    assert(L.top == 5);

    // 线性查找测试
    assert(L.search(11) == 0);
    assert(L.search(12) == 2);
    assert(L.search(50) == -1);

    // 排序测试
    L.sort();
    assert(L.isSorted == true);
    L.show();

    // 二分查找测试
    assert(L.search(11) == 1);
    assert(L.search(12) == 2);
    assert(L.search(50) == -1);
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行自测
    return 0;
}
