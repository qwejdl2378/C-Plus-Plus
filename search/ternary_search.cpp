/**
 * \file
 * \brief [Ternary search](https://en.wikipedia.org/wiki/Ternary_search) algorithm (三分查找算法实现)
 *
 * 三分查找是一种分治算法。它通过每次将搜索区间等分为 3 部分，
 * 借由单调性等性质来判断目标所处区间，从而比二分查找能缩减更多的空间。
 *
 * * 时间复杂度 : O(log3 n)
 * * 空间复杂度 : O(1)
 */

#include <iostream>

/**
 * 精度阈值。当区间长度小于该阈值时，使用顺序扫描以防止除法精度错误和死循环。
 */
#define absolutePrecision 10

/**
 * 默认自测试目标值
 */
#define _target 10

#define MAX 10000000  ///< 数组的最大预设大小

/**
 * 空输入接收函数桩
 */
void get_input() {}

/**
 * @brief 迭代法实现三分查找
 * 
 * @note 
 * 【公式 Bug 预警说明】：
 * 在第 59-60 行中，原作者对三分点的计算方式为：
 * `int oneThird = (left + right) / 3 + 1;`
 * `int twoThird = (left + right) * 2 / 3 + 1;`
 * 这在 `left > 0`（即区间发生右移收缩）时是绝对错误的！例如：若 left=90, right=100，
 * 算出来的 `oneThird` 约为 64，这超出了 [90, 100] 的有效区间范围，从而导致越界或无限死循环。
 * 正确的基于区间宽度的三分点计算应当为：
 * `int oneThird = left + (right - left) / 3;`
 * `int twoThird = right - (right - left) / 3;`
 * 
 * \param[in] left 查找区间的左边界索引（闭区间）
 * \param[in] right 查找区间的右边界索引（闭区间）
 * \param[in] A 排序数组
 * \param[in] target 待检索目标值
 * \returns 目标值的索引位置，找不到则返回 -1
 */
int it_ternary_search(int left, int right, int A[], int target) {
    while (1) {
        if (left < right) {
            // 当区间足够小时，直接顺序线性扫描，防止除法计算死循环
            if (right - left < absolutePrecision) {
                for (int i = left; i <= right; i++)
                    if (A[i] == target)
                        return i;

                return -1;
            }

            // 计算两个三分位点
            int oneThird = (left + right) / 3 + 1;
            int twoThird = (left + right) * 2 / 3 + 1;

            if (A[oneThird] == target)
                return oneThird;
            else if (A[twoThird] == target)
                return twoThird;

            // 根据单调递增属性，判断目标位于哪个子区间内并调整边界
            else if (target > A[twoThird])
                left = twoThird + 1;      // 目标在右三分之一区 [twoThird + 1, right]
            else if (target < A[oneThird])
                right = oneThird - 1;     // 目标在左三分之一区 [left, oneThird - 1]
            else
                // 目标在中三分之一区 [oneThird + 1, twoThird - 1]
                left = oneThird + 1, right = twoThird - 1;
        } else {
            return -1;
        }
    }
}

/**
 * @brief 递归法实现三分查找
 * \param[in] left 左边界索引
 * \param[in] right 右边界索引
 * \param[in] A 排序数组
 * \param[in] target 待检索目标值
 * \returns 目标值索引，找不到返回 -1
 */
int rec_ternary_search(int left, int right, int A[], int target) {
    if (left < right) {
        if (right - left < absolutePrecision) {
            for (int i = left; i <= right; i++)
                if (A[i] == target)
                    return i;

            return -1;
        }

        // 计算三分点（与迭代法有相同的区间偏移计算 Bug）
        int oneThird = (left + right) / 3 + 1;
        int twoThird = (left + right) * 2 / 3 + 1;

        if (A[oneThird] == target)
            return oneThird;
        if (A[twoThird] == target)
            return twoThird;

        if (target < A[oneThird])
            return rec_ternary_search(left, oneThird - 1, A, target);
        if (target > A[twoThird])
            return rec_ternary_search(twoThird + 1, right, A, target);

        return rec_ternary_search(oneThird + 1, twoThird - 1, A, target);
    } else {
        return -1;
    }
}

/**
 * @brief 对比调用迭代和递归的三分查找函数
 * \param[in] N 数组大小
 * \param[in] A 排序数组
 * \param[in] target 待查找的目标键值
 */
void ternary_search(int N, int A[], int target) {
    std::cout << it_ternary_search(0, N - 1, A, target) << '\t';
    std::cout << rec_ternary_search(0, N - 1, A, target) << '\t';
    std::cout << std::endl;
}

/** 
 * @brief 主函数
 * @returns 0
 */
int main() {
    int N = 21;
    // 准备一个测试排序数组，只有 10 在末尾
    int A[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 4, 10};
    get_input();
    ternary_search(N, A, _target); // 执行搜索
    return 0;
}
