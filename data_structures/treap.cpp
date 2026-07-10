/**
 * @file
 * @brief A balanced binary search tree (BST) on the basis of binary search tree
 * and heap: the [Treap](https://en.wikipedia.org/wiki/Treap) algorithm
 * implementation (基于BST和二叉堆特性的平衡树：Treap算法实现)
 *
 * @details
 * Treap（树堆）数据结构实现。
 * 支持与普通 BST 相同的操作，包括插入 (insert)、删除 (erase) 和查询（如查询指定元素的排名，或者排名为 x 的元素）。
 * 由于 Treap 在维持 BST 特性的同时，利用随机分配的优先级 (priority) 并通过旋转 (rotate) 操作来维持堆的特性，
 * 这使得树的期望深度保持在 O(log N)。因此，这些操作的时间复杂度都为 O(log N)，退化成链的概率极低。
 *
 * @author [Kairao ZHENG](https://github.com/fgmn)
 */

#include <array>     /// 用于 std::array
#include <cassert>   /// 用于 assert 断言
#include <cstdint>
#include <iostream>  /// 用于输入输出

/**
 * @namespace data_structures
 * @brief 数据结构命名空间
 */
namespace data_structures {
/**
 * @namespace treap
 * @brief 树堆 (Treap) 平衡树算法实现命名空间
 */
namespace treap {
const int maxNode = 1e5 + 5;  ///< 树堆节点的最大上限容量

/**
 * @brief Treap 结构体定义，采用静态扁平数组风格实现（竞赛编程中常用）
 */
struct Treap {
    int root = 0;      ///< 树根的节点编号
    int treapCnt = 0;  ///< 树中当前已分配节点的总计数器

    std::array<int, maxNode> key = {};       ///< 各节点存储的键值（Key）
    std::array<int, maxNode> priority = {};  ///< 各节点拥有的随机堆优先级（Priority）

    std::array<std::array<int, 2>, maxNode> childs = {
        {}};  ///< childs[i][0] 表示节点 i 的左子节点，childs[i][1] 表示节点 i 的右子节点

    std::array<int, maxNode> cnt =
        {};  ///< 存储每个节点的自身复本个数（允许重复元素）
    std::array<int, maxNode> size = {};  ///< 存储以该节点为根的子树的大小（用于排名查询）

    /**
     * @brief 树堆构造函数，初始化辅助根节点信息
     */
    Treap() : treapCnt(1) {
        priority[0] = INT32_MAX; // 0 号虚节点优先级设为无穷大，做边界处理
        size[0] = 0;
    }

    /**
     * @brief 重新计算并更新节点 x 的子树大小
     * @param x 待更新的节点索引
     */
    void update(int x) {
        size[x] = size[childs[x][0]] + cnt[x] + size[childs[x][1]];
    }

    /**
     * @brief 树旋转操作，在不破坏 BST 顺序的前提下调整堆优先级
     * @param x 待旋转的节点索引的引用（旋转后会被子节点替换）
     * @param t 旋转方向：0 代表左旋转；1 代表右旋转
     */
    void rotate(int &x, int t) {
        int y = childs[x][t];
        childs[x][t] = childs[y][1 - t];
        childs[y][1 - t] = x;
        // 旋转操作只会改变当前节点与其子节点之间的子树大小，需要重新更新
        update(x);
        update(y);
        x = y; // 替换当前节点的指针指向
    }

    /**
     * @brief 向子树中插入一个键值 k（内部递归方法）
     * @param x 指向当前子树根节点索引的引用（可能通过旋转发生改变）
     * @param k 待插入的键值
     */
    void _insert(int &x, int k) {
        if (x) {
            if (key[x] == k) {
                cnt[x]++; // 如果节点已存在，只需将该键值的复本数加一
            } else {
                int t = (key[x] < k); // 根据 BST 特性判断插入方向 (t=0往左，t=1往右)
                _insert(childs[x][t], k); // 递归插入子节点
                
                // 插入后，若子节点的随机优先级小于当前节点的优先级，则通过旋转调整以维持小顶堆特性
                if (priority[childs[x][t]] < priority[x]) {
                    rotate(x, t);
                }
            }
        } else {  // 当前位置为空，创建并分配一个新节点
            x = treapCnt++;
            key[x] = k;
            cnt[x] = 1;
            priority[x] = rand();  // 随机分配优先级
            childs[x][0] = childs[x][1] = 0;
        }
        update(x); // 递归向上更新子树大小
    }

    /**
     * @brief 从子树中删除一个键值 k（内部递归方法）
     * @param x 指向当前子树根节点索引的引用
     * @param k 待删除的键值
     */
    void _erase(int &x, int k) {
        if (key[x] == k) {
            if (cnt[x] > 1) {
                cnt[x]--; // 若当前键值的复本数大于一，只需递减复本数
            } else {
                if (childs[x][0] == 0 && childs[x][1] == 0) {
                    x = 0; // 若为叶子节点，直接清空返回
                    return;
                }
                // 若节点非叶子节点，根据左右子节点的优先级高低进行旋转，将当前节点旋转到下方去，然后递归删除
                int t = (priority[childs[x][0]] > priority[childs[x][1]]);
                rotate(x, t);
                _erase(x, k);
            }
        } else {  // 依据 BST 键值大小决定向左或向右寻找目标节点
            _erase(childs[x][key[x] < k], k);
        }
        update(x); // 更新子树大小
    }

    /**
     * @brief 寻找当前子树中第 k 小的元素（内部递归方法）
     * @param x 当前子树根节点的引用
     * @param k 查询的排名目标
     * @return 排名为第 k 小的元素的键值
     */
    int _get_k_th(int &x, int k) {
        if (k <= size[childs[x][0]]) {
            // 如果 k 小于左子树的大小，说明目标必然在左子树中
            return _get_k_th(childs[x][0], k);
        }
        k -= size[childs[x][0]] + cnt[x]; // 减去左子树大小以及当前节点自身复本数
        if (k <= 0) {
            return key[x]; // k <= 0 说明刚好命中当前节点键值
        }
        // 否则目标在右子树中，继续递归查找
        return _get_k_th(childs[x][1], k);
    }

    /**
     * @brief 查询指定元素 k 在整棵树堆中的排名（内部递归方法）
     * @param x 当前子树根节点
     * @param k 待查询排名的键值
     * @return 元素 k 的起始排名位置（从 1 开始）
     */
    int _get_rank(int x, int k) {
        if (!x) {
            return 0; // 空节点，返回 0
        }
        if (k == key[x]) {
            // 如果命中，返回左子树的全部节点数加上 1（代表当前元素的第一个排名）
            return size[childs[x][0]] + 1;
        } else if (k < key[x]) {
            // 目标更小，往左子树寻找
            return _get_rank(childs[x][0], k);
        } else {
            // 目标更大，往右子树寻找。返回：左子树大小 + 当前节点复本数 + 右子树查找排名
            return size[childs[x][0]] + cnt[x] + _get_rank(childs[x][1], k);
        }
    }

    /**
     * @brief 获取元素 k 在树堆中的前驱元素（即小于 k 的最大值）
     * @param k 目标值
     * @return 前驱元素值，若不存在返回 -1
     */
    int get_predecessor(int k) {
        int x = root, pre = -1;
        while (x) {
            if (key[x] < k) {
                pre = key[x]; // 记录可能的前驱，并尝试向右子树寻找更大的可能值
                x = childs[x][1];
            } else {
                x = childs[x][0]; // 往左子树收缩边界
            }
        }
        return pre;
    }

    /**
     * @brief 获取元素 k 在树堆中的后继元素（即大于 k 的最小值）
     * @param k 目标值
     * @return 后继元素值，若不存在返回 -1
     */
    int get_next(int k) {
        int x = root, next = -1;
        while (x) {
            if (key[x] > k) {
                next = key[x]; // 记录可能的后继，并尝试向左子树寻找更小的可能值
                x = childs[x][0];
            } else {
                x = childs[x][1]; // 往右子树收缩边界
            }
        }
        return next;
    }

    /**
     * @brief 外部公开接口：插入一个键值 k
     * @param k 待插入值
     */
    void insert(int k) { _insert(root, k); }

    /**
     * @brief 外部公开接口：删除一个键值 k
     * @param k 待删除值
     */
    void erase(int k) { _erase(root, k); }

    /**
     * @brief 外部公开接口：获取第 k 小的值
     * @param k 排名位置
     * @return 该排名的元素值
     */
    int get_k_th(int k) { return _get_k_th(root, k); }

    /**
     * @brief 外部公开接口：获取指定元素 k 的排名
     * @param k 待查询值
     * @return 排名（从 1 开始）
     */
    int get_rank(int k) { return _get_rank(root, k); }
};
}  // namespace treap
}  // namespace data_structures

/**
 * @brief 单元自测试用例集
 */
static void test() {
    data_structures::treap::Treap mTreap;  ///< 创建 Treap 对象实例

    mTreap.insert(1);
    mTreap.insert(2);
    mTreap.insert(3);
    assert(mTreap.get_k_th(2) == 2);
    mTreap.insert(4);
    mTreap.insert(5);
    mTreap.insert(6);
    assert(mTreap.get_next(4) == 5);
    mTreap.insert(7);
    assert(mTreap.get_predecessor(7) == 6);
    mTreap.erase(4);
    assert(mTreap.get_k_th(4) == 5);
    assert(mTreap.get_rank(5) == 4);
    mTreap.insert(10);
    assert(mTreap.get_rank(10) == 7);
    assert(mTreap.get_predecessor(10) == 7);

    std::cout << "All tests have successfully passed!\n";
}

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    test();  // 运行单元测试
    return 0;
}
