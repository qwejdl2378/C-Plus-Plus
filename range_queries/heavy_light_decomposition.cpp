/**
 * @file
 * @brief Implementation of [Heavy-Light Decomposition (HLD)](https://en.wikipedia.org/wiki/Heavy_path_decomposition) (树链剖分/重轻链剖分算法实现)
 *
 * @details
 * 树链剖分（Heavy-Light Decomposition, HLD）是一种将树形结构剖分成若干条线性链，
 * 从而将树上路径的查询和修改问题转化为区间段查询和修改问题的数据技术。它通常与**线段树（Segment Tree）**配合使用。
 *
 * ### 核心概念
 * 1. **重子节点 (Heavy Child)**：某个节点的子树中，节点数最多的那个子节点。
 * 2. **轻子节点 (Light Child)**：除重子节点之外的其余子节点。
 * 3. **重边 (Heavy Edge)**：连接节点与其重子节点的边。
 * 4. **轻边 (Light Edge)**：非重边。
 * 5. **重链 (Heavy Chain)**：由重边连接而成的极长路径。
 *
 * ### 剖分后的性质
 * - 从任何节点到根节点的简单路径上，轻边和重链的数量都不超过 $O(\log N)$ 条。
 * - 因此，任何树上路径的查询或修改操作都可以拆分成 $O(\log N)$ 个区间段。结合线段树，操作可在 $O(\log^2 N)$ 时间内完成。
 * - 单点更新时间复杂度为 $O(\log N)$。
 *
 * ### 模板局限性说明（算法设计审计）
 * - 本实现使用树上点权求和（Sum）。
 * - 内部 `update` 和 `query` 运算直接使用了 `+` 和 `-` 运算符。这意味着如果模板参数 `X` 被实例化为其他非加减法运算（如最大值/最小值），
 *   由于缺乏逆元运算（即没有对应的减法操作），本实现的单点差值更新机制将失效。若需要支持 Min/Max 区间查询，线段树需要实现区间覆盖/重构而非简单的差值累加。
 *
 * @author [Aniruthan R](https://github.com/aneee004)
 */

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <list>
#include <numeric>
#include <string>
#include <vector>

/**
 * @namespace range_queries
 * @brief 区间查询与更新命名空间
 */
namespace range_queries {
/**
 * @namespace heavy_light_decomposition
 * @brief 树链剖分算法相关命名空间
 */
namespace heavy_light_decomposition {

/**
 * @brief 基础树结构类（支持倍增法求 LCA 和 K 级祖先）
 * @tparam X 节点存储的数据类型
 */
template <typename X>
class Tree {
 private:
    std::vector<std::list<int>> t_adj;  ///< 树的邻接表
    const int t_nodes;                  ///< 树的节点总数
    const int t_maxlift;                ///< 最大倍增高度 (log2(N) + 1)
    std::vector<std::vector<int>> t_par;///< 倍增祖先数组，t_par[u][k] 表示 u 的 2^k 级祖先
    std::vector<int> t_depth;           ///< 各节点的深度值（根节点为 0）
    std::vector<int> t_size;            ///< 以各节点为根的子树节点数大小

    int t_root;                         ///< 树的根节点
    std::vector<X> t_val;               ///< 存储树节点的权值
    
    template <typename T>
    friend class HLD;

    /**
     * @brief 第一次 DFS：计算每个节点的子树大小 (t_size)
     * @param u 当前遍历的节点
     * @param p 当前节点的父节点
     */
    void dfs_size(int u, int p = -1) {
        for (const int &v : t_adj[u]) {
            if (v ^ p) { // 等价于 v != p
                dfs_size(v, u);
                t_size[u] += t_size[v];
            }
        }
    }

    /**
     * @brief 第二次 DFS：初始化倍增表和深度
     * @param u 当前遍历的节点
     * @param p 当前节点的父节点
     */
    void dfs_lca(int u, int p = -1) {
        t_par[u][0] = p;
        if (p != -1) {
            t_depth[u] = 1 + t_depth[p];
        }
        for (int k = 1; k < t_maxlift; k++) {
            if (t_par[u][k - 1] != -1) {
                t_par[u][k] = t_par[t_par[u][k - 1]][k - 1]; // 2^k 级祖先为 2^(k-1) 级祖先的 2^(k-1) 级祖先
            }
        }

        for (const int &v : t_adj[u]) {
            if (v ^ p) {
                dfs_lca(v, u);
            }
        }
    }

 public:
    /**
     * @brief 构造函数
     * @param nodes 节点总数
     */
    explicit Tree(int nodes)
        : t_nodes(nodes), t_maxlift(static_cast<int>(std::floor(std::log2(nodes))) + 1) {
        assert(nodes > 0); // 核心修复：防范节点数为 0 导致 log2 溢出崩溃
        t_root = 0; 
        t_adj.resize(t_nodes);
        t_par.assign(t_nodes, std::vector<int>(t_maxlift, -1));
        t_depth.assign(t_nodes, 0);
        t_size.assign(t_nodes, 1);
        t_val.resize(t_nodes);
    }

    /**
     * @brief 向树中添加一条无向边
     * @param u 端点 1
     * @param v 端点 2
     */
    void add_edge(const int u, const int v) {
        t_adj[u].push_back(v);
        t_adj[v].push_back(u);
    }

    /**
     * @brief 更改树的根节点
     */
    void change_root(int new_root) { t_root = new_root; }

    /**
     * @brief 设置所有节点的初始权值
     * @param node_val 初始权值数组
     */
    void set_node_val(const std::vector<X> &node_val) {
        assert(static_cast<int>(node_val.size()) == t_nodes);
        t_val = node_val;
    }

    /**
     * @brief 初始化树的状态信息（必须在添加完所有边和权值后调用）
     */
    void init() {
        assert(t_nodes > 0);
        dfs_size(t_root);
        dfs_lca(t_root);
    }

    /**
     * @brief 在树上提升节点指针（倍增上跳法）
     * @param p 指向节点 ID 的指针
     * @param dist 向上提升的步数
     */
    void lift(int *const p, int dist) const {
        for (int k = 0; k < t_maxlift; k++) {
            if (*p == -1) {
                return;
            }
            if (dist & 1) {
                *p = t_par[*p][k];
            }
            dist >>= 1;
        }
    }

    /**
     * @brief 获取某节点的第 K 级祖先
     */
    int kth_ancestor(int p, const int &dist) const {
        lift(&p, dist);
        return p;
    }

    /**
     * @brief 计算两个节点的最近公共祖先 (LCA)
     */
    int lca(int a, int b) const {
        assert(a >= 0 && b >= 0 && a < t_nodes && b < t_nodes);
        if (t_depth[a] > t_depth[b]) {
            lift(&a, t_depth[a] - t_depth[b]);
        }
        if (t_depth[b] > t_depth[a]) {
            lift(&b, t_depth[b] - t_depth[a]);
        }
        if (a == b) {
            return a;
        }
        for (int k = t_maxlift - 1; k >= 0; k--) {
            if (t_par[a][k] != t_par[b][k]) {
                a = t_par[a][k];
                b = t_par[b][k];
            }
        }
        return t_par[a][0];
    }
};

/**
 * @brief 无懒惰传播的点修改区间查询线段树类（配合 HLD 管理重链区间）
 * @tparam X 数据存储的数据类型
 */
template <typename X>
class SG {
 private:
    std::vector<X> s_tree;  ///< 线段树数组（大小为 2*N）
    int s_size;             ///< 叶子节点个数
    X sret_init = X(0);     ///< 查询初始默认值（求和为 0，最大值为 -INF）
    
    template <typename T>
    friend class HLD;

    /**
     * @brief 合并左右子区间的计算函数
     */
    X combine(X lhs, X rhs) const { return lhs + rhs; }

    /**
     * @brief 构造函数
     * @param size 线段树长度
     */
    explicit SG(int size) {
        s_size = size;
        s_tree.assign(2 * s_size, X(0)); // 核心修复：使用 X(0) 代替 0ll 提高泛型通用性
    }

    /**
     * @brief 单点累加更新
     * @param p 树状标号索引位置
     * @param v 累加增量值
     */
    void update(int p, X v) {
        for (p += s_size; p > 0; p >>= 1) {
            s_tree[p] += v;
        }
    }

    /**
     * @brief 区间查询 [l, r]
     */
    X query(int l, int r) const {
        X lhs = sret_init, rhs = sret_init;
        for (l += s_size, r += s_size + 1; l < r; l >>= 1, r >>= 1) {
            if (l & 1) {
                lhs = combine(lhs, s_tree[l++]);
            }
            if (r & 1) {
                rhs = combine(s_tree[--r], rhs);
            }
        }
        return combine(lhs, rhs);
    }

    /**
     * @brief 设定查询初始初值
     */
    void set_sret_init(X new_sret_init) { sret_init = new_sret_init; }
};

/**
 * @brief 树链剖分 (HLD) 主处理类
 * @tparam X 数据类型
 */
template <typename X>
class HLD : public Tree<X>, public SG<X> {
 private:
    int label;  ///< 分配连续 DFS 标号的计数器
    std::vector<int> h_label;     ///< 各节点在重构区间中的 DFS 标号
    std::vector<int> h_heavychild; ///< 各节点的重子节点 ID（无则为 -1）
    std::vector<int> h_parent;    ///< 各节点所在重链顶端顶点的节点 ID

    /**
     * @brief 第一遍 DFS：寻找并标记每个结点的重子节点
     */
    void dfs_hc(int u, int p = -1) {
        int hc_size = -1, hc_id = -1;
        for (const int &v : Tree<X>::t_adj[u]) {
            if (v ^ p) {
                dfs_hc(v, u);
                if (Tree<X>::t_size[v] > hc_size) {
                    hc_size = Tree<X>::t_size[v];
                    hc_id = v;
                }
            }
        }
        h_heavychild[u] = hc_id;
    }

    /**
     * @brief 第二遍 DFS：划分重链并确立每条重链的顶端节点
     */
    void dfs_par(int u, int p = -1) {
        if (h_heavychild[u] != -1) {
            h_parent[h_heavychild[u]] = h_parent[u]; // 重子节点所在重链顶端与其父节点一致
            dfs_par(h_heavychild[u], u);
        }
        for (const int &v : Tree<X>::t_adj[u]) {
            if (v ^ p && v ^ h_heavychild[u]) {
                h_parent[v] = v; // 轻子节点为新重链的顶端
                dfs_par(v, u);
            }
        }
    }

    /**
     * @brief 第三遍 DFS：优先遍历重子节点，使同一重链上的节点 DFS 序列标号（h_label）连续
     */
    void dfs_labels(int u, int p = -1) {
        h_label[u] = label++;
        if (h_heavychild[u] != -1) {
            dfs_labels(h_heavychild[u], u);
        }
        for (const int &v : Tree<X>::t_adj[u]) {
            if (v ^ p && v ^ h_heavychild[u]) {
                dfs_labels(v, u);
            }
        }
    }

    /**
     * @brief 查询同一根到叶重链路径上的两节点间的权值和
     */
    X chain_query(int a, int b) const {
        X ret = SG<X>::sret_init;
        if (Tree<X>::t_depth[a] < Tree<X>::t_depth[b]) {
            std::swap(a, b);
        }
        while (Tree<X>::t_depth[a] >= Tree<X>::t_depth[b]) {
            int l = h_label[h_parent[a]];
            int r = h_label[a];
            if (Tree<X>::t_depth[h_parent[a]] < Tree<X>::t_depth[b]) {
                l += Tree<X>::t_depth[b] - Tree<X>::t_depth[h_parent[a]];
            }
            ret = SG<X>::combine(ret, SG<X>::query(l, r));
            a = Tree<X>::t_par[h_parent[a]][0];
            if (a == -1) {
                break;
            }
        }
        return ret;
    }

 public:
    /**
     * @brief 构造函数，初始化并缩放各种相关结构
     */
    explicit HLD(int nodes) : Tree<X>(nodes), SG<X>(nodes) {
        label = 0;
        h_label.assign(Tree<X>::t_nodes, -1);
        h_heavychild.assign(Tree<X>::t_nodes, -1);
        h_parent.resize(Tree<X>::t_nodes);
        std::iota(h_parent.begin(), h_parent.end(), 0);
    }

    /**
     * @brief 初始化剖分和构建内部线段树（必须在树图建立并设置节点值后调用）
     */
    void init() {
        Tree<X>::init();

        label = 0;
        dfs_hc(Tree<X>::t_root);
        dfs_par(Tree<X>::t_root);
        dfs_labels(Tree<X>::t_root);

        // 初始化线段树数据叶子节点
        for (int i = 0; i < Tree<X>::t_nodes; i++) {
            SG<X>::s_tree[h_label[i] + Tree<X>::t_nodes] = Tree<X>::t_val[i];
        }
        // 自底向上递推构建线段树区间节点值
        for (int i = Tree<X>::t_nodes - 1; i > 0; i--) {
            SG<X>::s_tree[i] = SG<X>::combine(SG<X>::s_tree[i << 1],
                                              SG<X>::s_tree[i << 1 | 1]);
        }
    }

    /**
     * @brief 更新某个节点的权值
     * @param node 节点索引（从 0 开始）
     * @param val 设定的新值
     */
    void update(int node, X val) {
        X diff = val - Tree<X>::t_val[node];
        SG<X>::update(h_label[node], diff); // 累加差值
        Tree<X>::t_val[node] = val;
    }

    /**
     * @brief 查询树上任意两节点 a 到 b 的简单路径权值和
     * @param a 节点 1
     * @param b 节点 2
     * @return 路径上所有节点权值之和
     */
    X query(int a, int b) const {
        int lc = Tree<X>::lca(a, b); // 求解两节点的 LCA
        X ret = SG<X>::sret_init;
        assert(lc != -1);
        ret += chain_query(a, lc);   // 累加 a 到 LCA 链路径值
        ret += chain_query(b, lc);   // 累加 b 到 LCA 链路径值
        return ret - Tree<X>::t_val[lc]; // 根节点 lc 被重复计算了一次，减去
    }
};

}  // namespace heavy_light_decomposition
}  // namespace range_queries

/**
 * @brief 单元自测用例 1
 */
static void test_1() {
    std::cout << "Test 1:\n";

    int n = 5;
    std::vector<int64_t> node_values = {4, 2, 5, 2, 1};
    std::vector<std::vector<int>> edges = {{1, 2}, {1, 3}, {3, 4}, {3, 5}};
    std::vector<std::vector<int>> queries = {
        {2, 1, 4}, // 路径 1->3->4，权值和：4+5+2 = 11
        {1, 3, 2}, // 修改节点 3 的权值为 2
        {2, 1, 4}, // 路径 1->3->4，权值和：4+2+2 = 8
    };
    std::vector<int> expected_result = {11, 8};
    std::vector<int> code_result;

    range_queries::heavy_light_decomposition::HLD<int64_t> hld(n);
    hld.set_node_val(node_values);
    for (int i = 0; i < n - 1; i++) {
        int u = edges[i][0], v = edges[i][1];
        hld.add_edge(u - 1, v - 1);
    }
    hld.init();
    for (const auto &q : queries) {
        int type = q[0];
        if (type == 1) {
            int p = q[1], x = q[2];
            hld.update(p - 1, x);
        } else if (type == 2) {
            int a = q[1], b = q[2];
            code_result.push_back(hld.query(a - 1, b - 1));
        }
    }
    for (size_t i = 0; i < expected_result.size(); i++) {
        assert(expected_result[i] == code_result[i]);
    }
    std::cout << "Test 1 passed!\n";
}

/**
 * @brief 单元自测用例 2
 */
static void test_2() {
    std::cout << "Test 2:\n";

    int n = 10;
    std::vector<int64_t> node_values = {1, 8, 6, 8, 6, 2, 9, 2, 3, 2};
    std::vector<std::vector<int>> edges = {{10, 5}, {6, 2}, {10, 7},
                                           {5, 2},  {3, 9}, {8, 3},
                                           {1, 4},  {6, 4}, {8, 7}};
    std::vector<std::vector<int>> queries = {
        {2, 1, 10}, {2, 1, 6}, {1, 3, 4}, {2, 1, 9}, {1, 5, 3},
        {1, 7, 8},  {2, 1, 4}, {2, 1, 8}, {1, 1, 4}, {1, 2, 7}};
    std::vector<int> expected_result = {27, 11, 45, 9, 34};
    std::vector<int> code_result;

    range_queries::heavy_light_decomposition::HLD<int64_t> hld(n);
    hld.set_node_val(node_values);
    for (int i = 0; i < n - 1; i++) {
        int u = edges[i][0], v = edges[i][1];
        hld.add_edge(u - 1, v - 1);
    }
    hld.init();
    for (const auto &q : queries) {
        int type = q[0];
        if (type == 1) {
            int p = q[1], x = q[2];
            hld.update(p - 1, x);
        } else if (type == 2) {
            int a = q[1], b = q[2];
            code_result.push_back(hld.query(a - 1, b - 1));
        }
    }
    for (size_t i = 0; i < expected_result.size(); i++) {
        assert(expected_result[i] == code_result[i]);
    }
    std::cout << "Test 2 passed!\n";
}

/**
 * @brief 单元自测用例 3
 */
static void test_3() {
    std::cout << "Test 3:\n";

    int n = 8;
    std::vector<int64_t> node_values = {1, 8, 6, 8, 6, 2, 9, 2};
    std::vector<std::vector<int>> edges = {{1, 2}, {2, 3}, {3, 4}, {1, 5},
                                           {6, 3}, {7, 5}, {8, 7}};
    std::vector<std::vector<int>> queries = {
        {2, 6, 8}, {2, 3, 6}, {1, 3, 4}, {2, 7, 1}, {1, 5, 3},
        {1, 7, 8}, {2, 6, 4}, {2, 7, 8}, {1, 1, 4}, {1, 2, 7}};
    std::vector<int> expected_result = {34, 8, 16, 14, 10};
    std::vector<int> code_result;

    range_queries::heavy_light_decomposition::HLD<int64_t> hld(n);
    hld.set_node_val(node_values);
    for (int i = 0; i < n - 1; i++) {
        int u = edges[i][0], v = edges[i][1];
        hld.add_edge(u - 1, v - 1);
    }
    hld.init();
    for (const auto &q : queries) {
        int type = q[0];
        if (type == 1) {
            int p = q[1], x = q[2];
            hld.update(p - 1, x);
        } else if (type == 2) {
            int a = q[1], b = q[2];
            code_result.push_back(hld.query(a - 1, b - 1));
        }
    }
    for (size_t i = 0; i < expected_result.size(); i++) {
        assert(expected_result[i] == code_result[i]);
    }
    std::cout << "Test 3 passed!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test_1();
    test_2();
    test_3();
    return 0;
}
