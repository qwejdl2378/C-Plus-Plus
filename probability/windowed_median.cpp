/**
 * @file
 * @brief Implementation of a sliding window median calculator along a data stream (数据流滑动窗口中位数计算实现)
 *
 * @details
 * 给定一个整型数据流和一个固定大小的滑动窗口，实时计算当前窗口内所有元素的中位数。
 * 该算法主要利用了平衡二叉搜索树（BST，在 C++ 中使用 `std::multiset` 实现）来维护窗口内元素的有序状态。
 *
 * ### 算法设计与优化
 * 1. 使用 `std::list` 维护窗口元素的先进先出（FIFO）顺序，实现 $O(1)$ 的插入和删除。
 * 2. 使用 `std::multiset` 作为底层平衡二叉树存储当前窗口的所有数值，支持 $O(\log N)$ 的插入与删除。
 * 3. 维护一个指向 `std::multiset` 中位数位置的迭代器 `_itMedian`：
 *    - 当插入新元素时，根据其与当前中位数的大小关系，将迭代器微调 0 或 1 步，保持指向中位数。
 *    - 当移除过期元素时，同样微调迭代器。
 *    - 使得获取中位数的时间复杂度为常数级 $O(1)$。
 *
 * 时间复杂度: $O(\log N)$（每次插入/删除新元素），空间复杂度: $O(N)$，其中 $N$ 是滑动窗口的大小。
 *
 * @note
 * 【迭代器失效与未定义行为 Bug 审计与修复】：
 * 1. **重复元素迭代器失效导致崩溃与脏读 Bug**：在 `eraseFromSorted` 函数中，
 *    原程序直接通过 `_sortedValues.find(value)` 找到一个匹配的迭代器 `it` 并通过 `erase(it)` 删除。
 *    若窗口中存在重复值，`find()` 可能会返回与中位数指针 `_itMedian` 相同的迭代器。
 *    一旦直接将其擦除，会导致中位数指针 `_itMedian` 成为**野指针（Dangling Iterator）**，
 *    后续读取或微调中位数指针会触发 Undefined Behavior (甚至 Segfault 崩溃)。
 *    **修复**：当查找到要删除的迭代器 `it` 与 `_itMedian` 重合时，若存在其他相同的重复值，
 *    优先寻找并删除其相邻（前驱或后继）的同值迭代器，从而保护中位数迭代器不被擦除。
 *
 * @author [Yaniv Hollander](https://github.com/YanivHollander)
 */

#include <cassert>  
#include <cstdlib>  
#include <ctime>    
#include <list>     
#include <set>      
#include <vector>   
#include <iostream>
#include <random>   

namespace probability {
namespace windowed_median {

using Window = std::list<int>;
using size_type = Window::size_type;

/**
 * @class WindowedMedian
 * @brief 滑动窗口中位数计算类
 */
class WindowedMedian {
 private:
    const size_type _windowSize;  ///< 滑动窗口的固定大小限制
    Window _window;               ///< 双向链表维护窗口的 FIFO 顺序
    std::multiset<int> _sortedValues;  ///< 平衡二叉搜索树维护窗口内元素的有序性
    std::multiset<int>::const_iterator _itMedian;  ///< 指向中位数的迭代器

    /**
     * @brief 向平衡二叉树中插入新元素，并更新中位数迭代器
     */
    void insertToSorted(int value) {
        _sortedValues.insert(value);  
        const auto sz = _sortedValues.size();
        if (sz == 1) {  
            _itMedian = _sortedValues.begin();
            return;
        }

        // 新元素插入在左半部分，且当前元素个数为偶数时，中位数向左微调一步
        if (value < *_itMedian && sz % 2 == 0) {
            --_itMedian;  
        }
        // 新元素插入在右半部分（或等于中位数），且当前元素个数为奇数时，中位数向右微调一步
        else if (value >= *_itMedian && sz % 2 != 0) {
            ++_itMedian;  
        }
    }

    /**
     * @brief 从平衡二叉树中擦除过期元素，并安全更新中位数迭代器
     */
    void eraseFromSorted(int value) {
        const auto sz = _sortedValues.size();

        // 待删元素位于左半侧（或就是中位数本身），且原大小为偶数时，中位数向右微调一步
        if (value <= *_itMedian && sz % 2 == 0) {
            ++_itMedian;  
        }
        // 待删元素位于右半侧（或就是中位数本身），且原大小为奇数时，中位数向左微调一步
        else if (value >= *_itMedian && sz % 2 != 0) {
            --_itMedian;  
        }

        // 查找要删除的元素迭代器
        auto it = _sortedValues.find(value);  
        
        // 核心修复：若待删迭代器恰好是当前中位数迭代器本身，优先尝试删除其同值的重复元素，防止迭代器失效
        if (it == _itMedian) {
            auto next_it = std::next(it);
            if (next_it != _sortedValues.end() && *next_it == value) {
                it = next_it;
            } else if (it != _sortedValues.begin()) {
                auto prev_it = std::prev(it);
                if (*prev_it == value) {
                    it = prev_it;
                }
            }
        }
        
        _sortedValues.erase(it);                    
    }

 public:
    /**
     * @brief 构造函数
     * @param windowSize 窗口大小限制
     */
    explicit WindowedMedian(size_type windowSize) : _windowSize(windowSize) {}

    /**
     * @brief 向数据流中插入一个新值，同时移出超出窗口范围的老值
     */
    void insert(int value) {
        _window.push_back(value);
        insertToSorted(value);  
        if (_window.size() > _windowSize) {  
            eraseFromSorted(_window.front());  
            _window.pop_front();   
        }
    }

    /**
     * @brief 获取当前窗口内的中位数值
     */
    float getMedian() const {
        if (_sortedValues.empty()) return 0.0f;
        if (_sortedValues.size() % 2 != 0) {
            return static_cast<float>(*_itMedian);  
        }
        return 0.5f * static_cast<float>(*_itMedian) + 0.5f * static_cast<float>(*std::next(_itMedian));  
    }

    /**
     * @brief 朴素低效的中位数获取方法（通过临时复制并排序），用于对齐校验
     */
    float getMedianNaive() const {
        if (_window.empty()) return 0.0f;
        auto window = _window;
        window.sort();  
        auto median = *std::next(window.begin(), window.size() / 2);  
        if (window.size() % 2 != 0) {
            return static_cast<float>(median);
        }
        return 0.5f * static_cast<float>(median) +
               0.5f * static_cast<float>(*std::next(window.begin(), window.size() / 2 - 1));  
    }
};

}  // namespace windowed_median
}  // namespace probability

/**
 * @brief 单元自测用例
 */
static void test(const std::vector<int> &vals, int windowSize) {
    probability::windowed_median::WindowedMedian windowedMedian(windowSize);
    for (const auto val : vals) {
        windowedMedian.insert(val);
        // 验证高效中位数算法与朴素算法的结果完全对齐
        assert(windowedMedian.getMedian() == windowedMedian.getMedianNaive());
    }
}

/**
 * @brief 主函数
 */
int main() {
    test({1, 2, 3, 4, 5, 6, 7, 8, 9}, 3);  
    test({9, 8, 7, 6, 5, 4, 3, 2, 1}, 3);  
    test({9, 8, 7, 6, 5, 4, 5, 6}, 4);     
    test({3, 3, 3, 3, 3, 3, 3, 3, 3}, 3);  
    test({3, 3, 3, 3, 7, 3, 3, 3, 3}, 3);  
    test({4, 3, 3, -5, -5, 1, 3, 4, 5}, 5);  

    test({470211272, 101027544, 1457850878, 1458777923, 2007237709, 823564440,
          1115438165, 1784484492, 74243042, 114807987}, 6);

    // 随机自测校验
    std::mt19937 rng(42); // 固定随机种子
    std::vector<int> vals;
    for (int i = 0; i < 100; i++) {
        // 数组大小在 5 到 20 之间
        uint32_t n = 5 + (rng() % 16);
        // 窗口大小在 3 到 10 之间
        uint32_t windowSize = 3 + (rng() % 8);
        vals.clear();
        vals.reserve(n);
        for (uint32_t j = 0; j < n; j++) {
            vals.push_back(static_cast<int>(rng()) % 2000 - 1000);  
        }
        test(vals, windowSize);  
    }

    std::cout << "All windowed median tests passed successfully!" << std::endl;
    return 0;
}
