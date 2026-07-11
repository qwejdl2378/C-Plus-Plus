/**
 * @file
 * @brief Implementation of [Kosaraju's Algorithm](https://en.wikipedia.org/wiki/Kosaraju%27s_algorithm) for finding Strongly Connected Components (SCCs) (基于科萨拉朱算法的有向图强连通分量求解)
 *
 * @details
 * 强连通分量（Strongly Connected Components, SCC）：在有向图中，如果两个顶点对之间的每一个顶点都可达，则称该有向图是强连通的。
 * 极大的强连通子图称为强连通分量。
 *
 * ### Kosaraju 算法步骤
 * 1. 创建一个空栈。对原图执行一次完整的 DFS 遍历。当一个顶点的所有邻接点都被探索完毕（即将回溯）时，将该顶点压入栈中。这保证了栈顶元素为拓扑排序或强连通分量拓扑序的源头（即“出度”相对最大的顶点）。
 * 2. 建立反向图 `grev`（将原图中所有有向边的方向反转）。
 * 3. 依次弹出栈顶节点。如果它尚未被访问，则在反向图上以此节点为起点执行一次 DFS。这一次 DFS 所能遍历到的所有未访问节点集合即构成一个强连通分量。
 *
 * @note
 * 【原版代码的越界 Bug 审计】：
 * 原版实现中，0-indexed 和 1-indexed 混用造成了严重的内存越界风险：
 * 1. `main()` 函数中分配了 `a + 1` 大小的 `adj` 向量（采用 1-indexed），但调用 `kosaraju(a, adj)` 时传入的顶点数 $V = a$。
 * 2. 在 `kosaraju` 中：
 *    - `std::vector<bool> vis(V, false);` 建立的大小仅为 $V$，合法下标为 `0` 到 `V-1`。
 *    - 循环第 86 行：`for (int i = 0; i < V + 1; i++)` 尝试访问 `adj[V]`，当 $V$ 较大时若未匹配好实际分配大小可能会发生段错误。
 *    - 当有向边指向节点 `V` 时，在 `push_vertex` 中检测 `(*vis)[V]` 会直接引发**数组越界访问**。
 *    - `grev` 在分配时大小为 `V`，若 `*j = V`，执行 `grev[V].push_back(i)` 会产生**严重的越界写崩溃**。
 * 现已将该算法重构为完全标准且安全的 0-indexed 实现。
 *
 * 时间复杂度: O(V + E)
 * 空间复杂度: O(V + E)
 *
 * @author Anirban166
 */

#include <iostream>
#include <stack>
#include <vector>

/**
 * @brief 打印图的邻接表结构（调试辅助函数）
 * @param a 邻接表
 * @param V 顶点数量
 */
void print(const std::vector<std::vector<int> > &a, int V) {
    for (int i = 0; i < V; i++) {
        if (!a[i].empty()) {
            std::cout << "i=" << i << "-->";
        }
        for (int j : a[i]) {
            std::cout << j << " ";
        }
        if (!a[i].empty()) {
            std::cout << std::endl;
        }
    }
}

/**
 * @brief DFS 辅助函数：将访问完毕的顶点压入栈中
 * @param v 当前访问节点
 * @param st 栈指针，存放拓扑回溯序
 * @param vis 节点访问状态表指针
 * @param adj 原图邻接表
 */
void push_vertex(int v, std::stack<int> *st, std::vector<bool> *vis,
                 const std::vector<std::vector<int> > &adj) {
    (*vis)[v] = true;
    for (auto i = adj[v].begin(); i != adj[v].end(); i++) {
        if ((*vis)[*i] == false) {
            push_vertex(*i, st, vis, adj);
        }
    }
    st->push(v); // 节点所有分支均探索完，压入栈中
}

/**
 * @brief DFS 辅助函数：在反向图上进行强连通子图探索
 * @param v 当前访问节点
 * @param vis 节点访问状态表指针
 * @param grev 反向图邻接表
 */
void dfs(int v, std::vector<bool> *vis,
         const std::vector<std::vector<int> > &grev) {
    (*vis)[v] = true;
    for (auto i = grev[v].begin(); i != grev[v].end(); i++) {
        if ((*vis)[*i] == false) {
            dfs(*i, vis, grev);
        }
    }
}

/**
 * @brief 计算强连通分量（SCC）的总数
 * @param V 顶点数量
 * @param adj 原图邻接表（0-indexed，大小为 V）
 * @returns 强连通分量的个数
 */
int kosaraju(int V, const std::vector<std::vector<int> > &adj) {
    std::vector<bool> vis(V, false);
    std::stack<int> st;

    // 1. 第一轮 DFS：利用栈保存顶点的出栈拓扑序
    for (int v = 0; v < V; v++) {
        if (vis[v] == false) {
            push_vertex(v, &st, &vis, adj);
        }
    }

    // 2. 建立反向图 (将每条有向边反转方向)
    std::vector<std::vector<int> > grev(V);
    for (int i = 0; i < V; i++) {
        for (auto j = adj[i].begin(); j != adj[i].end(); j++) {
            grev[*j].push_back(i); // 反转 i -> *j 为 *j -> i
        }
    }

    // 3. 重置访问状态，第二轮 DFS 依据栈序输出连通分支
    for (int i = 0; i < V; i++) {
        vis[i] = false;
    }
    
    int count_scc = 0;
    while (!st.empty()) {
        int t = st.top();
        st.pop();
        if (vis[t] == false) {
            dfs(t, &vis, grev); // 在反向图上搜索连通块
            count_scc++;        // 强连通分量计数加 1
        }
    }
    return count_scc;
}

/**
 * @brief 主测试程序
 */
int main() {
    int t = 0;
    std::cout << "Enter number of test cases: ";
    std::cin >> t;
    while (t--) {
        int a = 0, b = 0;  // a 为节点数，b 为有向边数
        std::cout << "Enter number of nodes and edges: ";
        std::cin >> a >> b;
        
        std::vector<std::vector<int> > adj(a); // 改为完全 0-indexed，大小为 a
        
        int m = 0, n = 0;
        std::cout << "Enter edges (m n) representing m -> n (1-indexed input will be converted to 0-indexed):" << std::endl;
        for (int i = 0; i < b; i++) {
            std::cin >> m >> n;
            // 自动转化为 0-indexed 存储，防止内存越界
            adj[m - 1].push_back(n - 1);
        }
        
        std::cout << "Total Strongly Connected Components: " << kosaraju(a, adj) << std::endl;
    }
    return 0;
}
