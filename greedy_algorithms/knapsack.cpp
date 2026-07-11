/**
 * @file
 * @brief Implementation of the Fractional [Knapsack Problem](https://en.wikipedia.org/wiki/Knapsack_problem) (部分背包/分数背包贪心算法实现)
 *
 * @details
 * 与 0-1 背包问题（每个物品只能完整装入或不装入，必须用动态规划求解）不同，
 * **部分背包问题（Fractional Knapsack）**允许我们将物品的一部分装入背包中。
 * 因此，该问题可以通过**贪心算法**在 $O(N \log N)$ 时间内求得全局最优解。
 *
 * ### 贪心决策策略 (Greedy Choice)
 * 1. 计算每个物品的“单位重量利润”（性价比 = 利润 / 重量）。
 * 2. 将所有物品按照性价比从高到低进行降序排序。
 * 3. 贪心地优先将性价比最高的物品装入背包。
 * 4. 如果背包剩余容量能够完整装下该物品，则全装入，扣减容量并累加利润，继续考虑下一物品。
 * 5. 如果背包剩余容量不足以完整装下该物品，则将该物品切片，把能塞满背包的部分装入，累加相应比例的利润，背包满，算法结束。
 *
 * 时间复杂度: $O(N \log N)$ (排序是算法瓶颈，后续贪心扫描只需 $O(N)$)
 * 空间复杂度: $O(N)$ (动态分配物品数组)
 *
 * @note
 * 【C++ 除以 0 崩溃与边界校验 Bug 审计与修复】：
 * 1. **除以零崩溃风险**：在 `profitPerUnit` 函数中，如果输入的物品重量为 0，
 *    代码执行 `(float)x.profit / (float)x.weight` 会发生**除以零错误**，返回无穷大或 `NaN`，导致后续排序和乘法行为错乱崩溃。
 *    **修复**：在计算单位价值时，如果重量为 0 则直接返回 0。
 * 2. **非法输入保护**：如果输入物品数量 `n <= 0`，或者背包容量 `capacity <= 0`，
 *    需要进行合理的拦截和安全防御，避免错误的内存申请。
 *    **修复**：增加对 `n` 和 `capacity` 的防御性校验。
 *
 * @author Unknown author
 */

#include <iostream>
#include <algorithm>
#include <vector>

/**
 * @brief 物品结构体
 */
struct Item {
    int weight; ///< 物品重量
    int profit; ///< 物品总利润
};

/**
 * @brief 计算物品的单位重量利润（性价比）
 * @param x 物品实例
 * @returns 单位性价比（float）
 */
float profitPerUnit(Item x) {
    // 核心修复：防范重量为 0 时发生除以零崩溃
    if (x.weight == 0) {
        return 0.0f;
    }
    return static_cast<float>(x.profit) / static_cast<float>(x.weight);
}

/**
 * @brief 快速排序的划分函数（基于性价比升序排列）
 */
int partition(Item arr[], int low, int high) {
    Item pivot = arr[high];  // 选定最后一个元素为基准
    int i = (low - 1);       // 记录小于基准的元素边界

    for (int j = low; j < high; j++) {
        // 如果当前物品性价比小于等于基准，则交换到左侧
        if (profitPerUnit(arr[j]) <= profitPerUnit(pivot)) {
            i++;
            Item temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    // 将基准元素放到正确位置
    Item temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return (i + 1);
}

/**
 * @brief 快速排序核心函数
 */
void quickSort(Item arr[], int low, int high) {
    if (low < high) {
        int p = partition(arr, low, high);
        quickSort(arr, low, p - 1);
        quickSort(arr, p + 1, high);
    }
}

/**
 * @brief 主函数
 */
int main() {
    float capacity = 0.f;
    std::cout << "Enter the capacity of the knapsack: ";
    std::cin >> capacity;
    
    int n = 0;
    std::cout << "Enter the number of Items: ";
    std::cin >> n;
    
    // 核心修复：防御非法的零或负数物品数量
    if (n <= 0 || capacity <= 0.f) {
        std::cout << "Invalid input. Max Profit: 0" << std::endl;
        return 0;
    }

    Item *itemArray = new Item[n];
    for (int i = 0; i < n; i++) {
        std::cout << "Enter the weight and profit of item " << i + 1 << ": ";
        std::cin >> itemArray[i].weight;
        std::cin >> itemArray[i].profit;
    }

    // 利用快速排序对物品按性价比升序排列（从小到大）
    quickSort(itemArray, 0, n - 1);

    float maxProfit = 0.0f;
    int i = n; // 从性价比最高（排在数组末尾）的物品开始，逆序向前装入

    std::cout << "\nItems packed into knapsack (Weight, Profit):\n";

    // 贪心选择装入物品
    while (capacity > 0.0f && --i >= 0) {
        // 如果背包剩余容量能够完整装下当前物品
        if (capacity >= itemArray[i].weight) {
            maxProfit += static_cast<float>(itemArray[i].profit);
            capacity -= static_cast<float>(itemArray[i].weight);
            std::cout << "\t" << itemArray[i].weight << "\t" << itemArray[i].profit << "\n";
        } else {
            // 容量不足，只能装入该物品的剩余可用比例部分（切片）
            float gain = profitPerUnit(itemArray[i]) * capacity;
            maxProfit += gain;
            std::cout << "\t" << capacity << " (fraction)\t" << gain << "\n";
            capacity = 0.0f; // 背包塞满，清空容量
            break;
        }
    }

    std::cout << "\nMax Profit: " << maxProfit << std::endl;

    delete[] itemArray; // 释放动态数组空间
    return 0;
}
