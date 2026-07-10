#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

// 定义并查集数组的最大节点数范围
const int mx = 1e6 + 5;
using ll = int64_t;

// parent 数组用于并查集，记录每个顶点的父节点
std::array<ll, mx> parent;
ll node, edge; // 顶点数和边数

// 存储所有的边，格式为：<边权值, <起点, 终点>>
std::vector<std::pair<ll, std::pair<ll, ll>>> edges;

/**
 * @brief 初始化并查集
 * 将每个节点的父节点指向其自身，表示各节点自成一个连通分量
 */
void initial() {
    for (int i = 0; i < node + edge; ++i) {
        parent[i] = i;
    }
}

/**
 * @brief 并查集的寻根操作（带有路径压缩）
 * 沿父节点指针向上查找，直到找到该连通分量的根节点，
 * 并在回溯时将当前节点及其所有祖先直接连接到根节点上以提高后续查找效率。
 * 
 * @param i 待查找的顶点索引
 * @return 连通分量的根节点
 */
int root(int i) {
    while (parent[i] != i) {
        parent[i] = parent[parent[i]]; // 路径压缩，将双亲指向祖父
        i = parent[i];
    }
    return i;
}

/**
 * @brief 并查集的合并操作
 * 将两个属于不同连通分量的子集进行合并。
 * 
 * @param x 顶点 x
 * @param y 顶点 y
 */
void join(int x, int y) {
    int root_x = root(x);
    int root_y = root(y);
    parent[root_x] = root_y; // 将 x 分量的根节点链接到 y 分量的根节点上
}

/**
 * @brief Kruskal 算法求最小生成树 (MST) 的最小权重总和
 * 
 * @return 最小生成树的总边权之和
 */
ll kruskal() {
    ll mincost = 0; // 最小生成树的总权值
    
    // 遍历已排好序的各条边（边已经按权值从小到大排序）
    for (int i = 0; i < edge; ++i) {
        ll x = edges[i].second.first;  // 边的起点
        ll y = edges[i].second.second; // 边的终点
        
        // 如果起点和终点不在同一个连通分量中（不会形成环）
        if (root(x) != root(y)) {
            mincost += edges[i].first; // 累加该边权值到最小生成树中
            join(x, y);                // 合并这两个连通分量
        }
    }
    return mincost;
}

int main() {
    // 循环处理多组测试输入，直到输入 0 0 为止
    while (true) {
        int from = 0, to = 0, cost = 0, totalcost = 0;
        std::cin >> node >> edge;  // 输入顶点数和边数
        if (node == 0 && edge == 0) {
            break;  // 输入 0 0 代表测试结束
        }
        
        initial();  // 初始化并查集
        
        // 输入每条边的信息：起点、终点和权值
        for (int i = 0; i < edge; ++i) {
            std::cin >> from >> to >> cost;
            edges.emplace_back(make_pair(cost, std::make_pair(from, to)));
            totalcost += cost;
        }
        
        // 关键步骤 1：按照边权值从小到大进行排序
        sort(edges.begin(), edges.end());
        
        // 关键步骤 2：执行 Kruskal 算法并输出最小生成树的总花费
        std::cout << kruskal() << std::endl;
        
        // 清理当前测试用例的边列表，为下一轮输入做准备
        edges.clear();
    }
    return 0;
}
