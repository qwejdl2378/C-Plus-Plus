/**
 * @file
 * @brief Implementation of [Strand Sort](https://en.wikipedia.org/wiki/Strand_sort) algorithm using std::list (基于 std::list 的线索排序/缕排序算法实现)
 *
 * @details
 * 线索排序（Strand Sort）是一种适合链表数据结构的分配式比较排序算法。
 * 
 * ### 缕排序工作原理
 * 1. 从未排序链表 `lst` 中提取出第一个元素放入辅助链表 `sorted`。
 * 2. 顺序扫描 `lst` 中的其余元素，如果发现某个元素大于等于 `sorted` 的末尾元素（即可以构成有序子序列），
 *    则将其从 `lst` 中移出（通过 `lst.erase`）并追加到 `sorted` 链表尾端。这一步拉出了一条“线索”（Strand）。
 * 3. 将拉出来的有序“线索” `sorted` 归并到主结果链表 `result` 中（利用 `std::list::merge`，它在 $O(A+B)$ 时间内将两个有序链表合并并清空源链表）。
 * 4. 重复步骤 1-3，直到未排序链表 `lst` 变为空。
 *
 * ### 性能
 * - 时间复杂度: 
 *   - 最好情况: $O(N)$ (如果输入链表本身已经是升序的，单趟扫描就拉出全部线索)
 *   - 最坏情况: $O(N^2)$ (如果输入链表是降序的，每次只能拉出长度为 1 的线索，退化为插入排序)
 *   - 平均情况: $O(N^2)$
 * - 空间复杂度: $O(N)$ (由于分配了存储结果的链表节点)
 *
 * @author [Mertcan Davulcu](https://github.com/mertcandav)
 */

#include <iostream>
#include <list>

/**
 * @namespace sorting
 * @brief 排序算法命名空间
 */
namespace sorting {
/**
 * @namespace strand
 * @brief 线索排序算法命名空间
 */
namespace strand {
/**
 * @brief 线索排序主算法
 * @tparam T 元素模板类型
 * @param lst 传入的待排序链表（值传递，在内部修改并返回新链表）
 * @returns 排序好（升序）的新 std::list 容器
 */
template <typename T>
std::list<T> strand_sort(std::list<T> lst) {
    // 边界条件：若大小为 0 或 1，则无需排序直接返回
    if (lst.size() < 2) {
        return lst;
    }

    std::list<T> result; // 最终合并存放的有序结果链表
    std::list<T> sorted; // 单趟拉出的线索链表

    while (!lst.empty()) {
        // 1. 将待排链表的首元素弹出放入线索链表作为基准
        sorted.push_back(lst.front());
        lst.pop_front();

        // 2. 遍历剩余的链表元素，拉出一条递增线索
        for (auto it = lst.begin(); it != lst.end(); ) {
            if (sorted.back() <= *it) {
                sorted.push_back(*it); // 加入线索
                it = lst.erase(it);    // 从原链表中移除，erase 会返回指向下一项的迭代器
            } else {
                it++; // 不满足单调性，继续向下查找
            }
        }

        // 3. 将当前拉出的有序线索并入主结果链表中，merge 完毕后 sorted 会自动被置空
        result.merge(sorted);
    }
    return result;
}
}  // namespace strand
}  // namespace sorting

/**
 * @brief 单元自测用例
 */
static void test() {
    std::list<int> lst = { -333, 525, 1, 0, 94, 52, 33 };

    std::cout << "Before: ";
    for (auto item : lst) {
        std::cout << item << " ";
    }

    // 调用线索排序
    lst = sorting::strand::strand_sort(lst);

    std::cout << "\nAfter: ";
    for (auto item : lst) {
        std::cout << item << " ";
    }
    std::cout << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行测试
    return 0;
}
