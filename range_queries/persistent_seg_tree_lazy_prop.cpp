/**
 * @file
 * @brief Implementation of [Persistent Segment Tree with Lazy Propagation](https://en.wikipedia.org/wiki/Persistent_data_structure) (带懒惰传播的持久化线段树/可持久化线段树算法实现)
 *
 * @details
 * 可持久化数据结构（Persistent Data Structure）是指在修改时能够保留历史版本的数据结构。
 * 本实现为**带区间修改（懒惰传播）的可持久化线段树**。
 *
 * ### 核心原理（单点复制 - Path Copying）
 * 1. 当对线段树进行更新操作时，不直接在原树节点上进行就地修改。
 * 2. 仅复制从根节点到更新叶子节点路径上的所有受影响节点（数量为 $O(\log N)$），并使新版本的祖先节点指向这些新复制的节点。
 * 3. 对于未受更新影响的子树，新版本直接共享旧版本的子树节点指针。
 * 4. 每次更新产生一个新根节点（`ptrs[version]`），从而允许在 $O(\log N)$ 时间内查询任何历史版本的数据。
 *
 * ### 懒惰传播与持久化的冲突与解决策略
 * - **冲突点**：在传统线段树中，懒惰下传（Lazy Pushdown）会直接修改子节点的权值和懒惰标记。
 *   但在可持久化结构中，直接修改子节点会污染历史版本。
 * - **本实现方案（按需复制）**：在执行 `lazy` 下传时，如果子节点需要被写入（即被赋予新的 `prop` 值），
 *   程序会立即通过 `newKid` 拷贝一份该子节点，再对其进行修改。
 * - **物理修改与逻辑持久化**：需要注意的是，本实现中，在 `query` 和 `update` 时调用的 `lazy` 操作会就地更新当前节点的 `val` 并将 `prop` 清零。
 *   由于这是单线程执行，虽然物理节点的内容和子节点指针发生了改变，但由于改变后的子节点包含了下传后的等价数据，
 *   因此其所代表的历史版本的**逻辑查询结果**仍然是绝对正确且等价的。对于多线程并发读取，这可能会引入竞态风险。
 *
 * 时间复杂度: 更新 $O(\log N)$，查询 $O(\log N)$
 * 空间复杂度: 每次更新新增 $O(\log N)$ 个节点空间
 *
 * @author [Magdy Sedra](https://github.com/MSedra)
 */

#include <cassert>   /// 用于 assert 断言
#include <cstdint>   /// 用于 std::uint32_t, int64_t
#include <iostream>  /// 用于输入输出
#include <memory>    /// 用于智能指针 std::shared_ptr, std::make_shared
#include <vector>    /// 用于 std::vector

/**
 * @namespace range_queries
 * @brief 区间查询算法命名空间
 */
namespace range_queries {

/**
 * @brief 可持久化线段树类
 */
class perSegTree {
 private:
    /**
     * @brief 线段树节点定义
     */
    class Node {
     public:
        std::shared_ptr<Node> left = nullptr;   ///< 指向左子节点的智能指针
        std::shared_ptr<Node> right = nullptr;  ///< 指向右子节点的智能指针
        int64_t val = 0;   ///< 当前节点区间和的值
        int64_t prop = 0;  ///< 延迟标记值（Lazy Tag）
    };

    uint32_t n = 0;  ///< 原数组中叶子节点的数量
    std::vector<std::shared_ptr<Node>> ptrs{};  ///< 存放每个版本根节点指针的数组，ptrs[v] 代表第 v 版本根节点
    std::vector<int64_t> vec{};  ///< 临时存储原数组的初始值

    /**
     * @brief 复制当前节点，生成一个完全一样的新节点（用于持久化路径拷贝）
     * @param curr 待拷贝的当前节点指针
     * @returns 新拷贝生成的节点指针
     */
    std::shared_ptr<Node> newKid(std::shared_ptr<Node> const &curr) {
        auto newNode = std::make_shared<Node>();
        newNode->left = curr->left;
        newNode->right = curr->right;
        newNode->prop = curr->prop;
        newNode->val = curr->val;
        return newNode;
    }

    /**
     * @brief 延迟标记下传函数（Lazy Propagation）
     * @param i 当前节点区间的左边界
     * @param j 当前节点区间的右边界
     * @param curr 当前节点指针（如果其含有延迟标记，会按需拷贝子节点并下传）
     */
    void lazy(const uint32_t &i, const uint32_t &j,
              std::shared_ptr<Node> const &curr) {
        if (!curr->prop) {
            return; // 没有延迟标记，直接返回
        }
        
        // 累加延迟标记对当前节点区间和的贡献
        curr->val += static_cast<int64_t>(j - i + 1) * curr->prop;
        
        // 如果不是叶子节点，则将延迟标记下传给左右子节点
        if (i != j) {
            // 核心安全持久化：在修改子节点前，必须拷贝子节点，防止破坏其他版本的结构
            curr->left = newKid(curr->left);
            curr->right = newKid(curr->right);
            
            curr->left->prop += curr->prop;
            curr->right->prop += curr->prop;
        }
        curr->prop = 0; // 清空当前节点的延迟标记
    }

    /**
     * @brief 递归构建初始线段树（版本 0）
     * @param i 区间左端点
     * @param j 区间右端点
     * @returns 构建好的子树根节点指针
     */
    std::shared_ptr<Node> construct(const uint32_t &i, const uint32_t &j) {
        auto newNode = std::make_shared<Node>();
        if (i == j) {
            newNode->val = vec[i];
        } else {
            uint32_t mid = i + (j - i) / 2;
            auto leftt = construct(i, mid);
            auto right = construct(mid + 1, j);
            newNode->val = leftt->val + right->val;
            newNode->left = leftt;
            newNode->right = right;
        }
        return newNode;
    }

    /**
     * @brief 递归区间更新（在当前区间 [i, j] 中，对修改目标区间 [l, r] 累加值 value）
     * @param i 当前节点左边界
     * @param j 当前节点右边界
     * @param l 目标修改左边界
     * @param r 目标修改右边界
     * @param value 累加的值
     * @param curr 当前版本中对应的节点指针
     * @returns 新版本中对应的节点指针
     */
    std::shared_ptr<Node> update(const uint32_t &i, const uint32_t &j,
                                 const uint32_t &l, const uint32_t &r,
                                 const int64_t &value,
                                 std::shared_ptr<Node> const &curr) {
        lazy(i, j, curr);
        
        // 目标区间完全覆盖当前区间
        if (i >= l && j <= r) {
            std::shared_ptr<Node> newNode = newKid(curr); // 拷贝当前节点
            newNode->prop += value;                       // 累加延迟标记
            lazy(i, j, newNode);                          // 立即下传确保 val 正确
            return newNode;
        }
        
        // 当前区间与目标区间完全无交集
        if (i > r || j < l) {
            return curr; // 直接复用旧版本的节点指针，无需拷贝
        }
        
        // 部分覆盖：需要新建节点，并递归更新左右子树
        auto newNode = std::make_shared<Node>();
        uint32_t mid = i + (j - i) / 2;
        newNode->left = update(i, mid, l, r, value, curr->left);
        newNode->right = update(mid + 1, j, l, r, value, curr->right);
        newNode->val = newNode->left->val + newNode->right->val;
        return newNode;
    }

    /**
     * @brief 递归区间和查询
     * @param i 当前节点左边界
     * @param j 当前节点右边界
     * @param l 查询目标左边界
     * @param r 查询目标右边界
     * @param curr 当前版本中的节点指针
     * @returns 目标区间的累加和
     */
    int64_t query(const uint32_t &i, const uint32_t &j, const uint32_t &l,
                  const uint32_t &r, std::shared_ptr<Node> const &curr) {
        lazy(i, j, curr);
        if (j < l || r < i) {
            return 0; // 无交集返回 0
        }
        if (i >= l && j <= r) {
            return curr->val; // 完全覆盖，返回当前区间的和
        }
        uint32_t mid = i + (j - i) / 2;
        return query(i, mid, l, r, curr->left) +
               query(mid + 1, j, l, r, curr->right);
    }

 public:
    perSegTree() = default;

    /**
     * @brief 外部构造入口：从给定初始 vector 构建持久化线段树（产生版本 0）
     * @param vec 初始值 vector
     */
    void construct(const std::vector<int64_t> &vec) {
        if (vec.empty()) {
            return;
        }
        n = vec.size();
        this->vec = vec;
        auto root = construct(0, n - 1);
        ptrs.push_back(root); // 版本 0
    }

    /**
     * @brief 外部更新接口：在区间 [l, r] 上每个元素累加 value (产生一个新版本)
     */
    void update(const uint32_t &l, const uint32_t &r, const int64_t &value) {
        if (ptrs.empty()) {
            return;
        }
        // 基于上一个版本的根节点，计算并产生新一版的根节点，存入 ptrs 数组中
        ptrs.push_back(update(0, n - 1, l, r, value, ptrs[ptrs.size() - 1]));
    }

    /**
     * @brief 外部查询接口：查询指定历史版本 version 中区间 [l, r] 的累加和
     * @param l 查询左边界
     * @param r 查询右边界
     * @param version 查询的目标版本号（0 代表初始版本）
     */
    int64_t query(const uint32_t &l, const uint32_t &r, const uint32_t &version) {
        if (version >= ptrs.size()) {
            return 0;
        }
        return query(0, n - 1, l, r, ptrs[version]);
    }

    /**
     * @brief 获取当前已存有的版本总数量
     */
    uint32_t size() const {
        return ptrs.size();
    }
};

}  // namespace range_queries

/**
 * @brief 单元自测用例
 */
static void test() {
    std::vector<int64_t> arr = {-5, 2, 3, 11, -2, 7, 0, 1};
    range_queries::perSegTree tree;
    
    std::cout << "Elements before any updates are {";
    for (uint32_t i = 0; i < arr.size(); ++i) {
        std::cout << arr[i];
        if (i != arr.size() - 1) {
            std::cout << ",";
        }
    }
    std::cout << "}\n";

    tree.construct(arr); // 版本 0
    
    // 验证版本 0 的区间查询 [2, 4] 结果是否等于 3 + 11 - 2 = 12
    int64_t q1 = tree.query(2, 4, 0);
    std::cout << "Querying range sum on version 0 from index 2 to 4 = 12? Result: " << q1 << '\n';
    assert(q1 == 12);

    std::cout << "Subtract 7 from all elements from index 1 to index 5 inclusive (creates version 1)\n";
    tree.update(1, 5, -7); // 区间更新

    // 验证版本 1 的各个值是否正确：{-5, -5, -4, 4, -9, 0, 0, 1}
    std::cout << "Elements of the segment tree whose version = 1 are {";
    for (uint32_t i = 0; i < arr.size(); ++i) {
        std::cout << tree.query(i, i, 1);
        if (i != arr.size() - 1) {
            std::cout << ",";
        }
    }
    std::cout << "}\n";
    assert(tree.query(1, 1, 1) == -5);
    assert(tree.query(2, 2, 1) == -4);
    assert(tree.query(3, 3, 1) == 4);
    assert(tree.query(4, 4, 1) == -9);
    assert(tree.query(5, 5, 1) == 0);

    std::cout << "Add 10 to all elements from index 0 to index 7 inclusive (creates version 2)\n";
    tree.update(0, 7, 10); // 区间更新

    std::cout << "Elements of the segment tree whose version = 2 are {";
    for (uint32_t i = 0; i < arr.size(); ++i) {
        std::cout << tree.query(i, i, 2);
        if (i != arr.size() - 1) {
            std::cout << ",";
        }
    }
    std::cout << "}\n";

    // 验证版本 0 仍然保留且完全未受后面更新的影响
    int64_t q_old = tree.query(3, 5, 0);
    std::cout << "Querying range sum on version 0 from index 3 to 5 = 16? Result: " << q_old << '\n';
    assert(q_old == 16); // 11 - 2 + 7 = 16

    // 验证版本 1
    int64_t q_v1 = tree.query(3, 5, 1);
    std::cout << "Querying range sum on version 1 from index 3 to 5 = -5? Result: " << q_v1 << '\n';
    assert(q_v1 == -5); // 4 - 9 + 0 = -5

    std::cout << "All Persistent Segment Tree tests passed successfully!" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行自测
    return 0;
}
