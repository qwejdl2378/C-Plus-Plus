/**
 * @file
 * @brief Implementation of [Prim's Minimum Spanning Tree](https://en.wikipedia.org/wiki/Prim%27s_algorithm) algorithm (Prim 最小生成树算法实现)
 *
 * @details
 * Prim 算法是一种典型的图论贪心算法，用于在加权连通图中寻找最小生成树。
 *
 * ### 贪心决策策略 (Greedy Choice)
 * 1. 初始时，选择任意一个顶点加入最小生成树的顶点集合中。
 * 2. 局部贪心选择：在所有连接“已在树中的顶点”与“尚未在树中的顶点”的边中，选择权重最小的一条边，将该边对应的未访问顶点并入树中。
 * 3. 距离松弛与更新：新顶点加入后，以它为桥梁，更新所有其余未加入顶点的最小连接权值（`key`）。
 * 4. 迭代直到所有顶点均已并入树中。
 *
 * 时间复杂度: $O(V^2)$ (邻接矩阵遍历)
 * 空间复杂度: $O(V)$ (存储生成树节点的状态信息)
 *
 * @note
 * 【C++ 全局宏名冲突与不连通图逻辑漏洞审计与修复】：
 * 1. **全局宏命名冲突风险**：原代码使用 `#define INFINITY 99999`。
 *    在 C++ 中，`INFINITY` 是 `<cmath>` 头文件定义的标准浮点数常量。宏定义会强行劫持并重写它，如果工程后续引入数学库会造成**编译冲突错误**。
 *    **修复**：改用 `constexpr int INF = 99999;`。
 * 2. **非连通图死循环/错乱 Bug**：在非连通图或大权值图下，若剩下节点不可达，
 *    内部寻找最小 key 节点的循环会找不到有效节点，导致 `minIndex` 停留在 `0`。
 *    进而重复将已访问的第 0 号节点拿出来处理，造成树结构错乱。
 *    **修复**：若最小权重 `min` 仍为初始无穷大，说明剩余未访问的节点与当前树不连通，应当直接退出循环以防错乱。
 *
 * @author Unknown author
 */

#include <iostream>
#include <vector>

// 使用 constexpr 代替 #define，避免命名污染与宏冲突
constexpr int V_SIZE = 4;
constexpr int INF = 99999;

// 邻接矩阵表示的图结构
int graph[V_SIZE][V_SIZE] = {
    {0, 5, 1, 2}, 
    {5, 0, 3, 3}, 
    {1, 3, 0, 4}, 
    {2, 3, 4, 0}
};

/**
 * @brief 记录最小生成树节点状态的结构体
 */
struct mst {
    bool visited; ///< 该顶点是否已加入生成树
    int key;      ///< 该顶点与当前生成树连接的最小边权重
    int near;     ///< 该顶点与生成树中哪个邻接顶点相连
};

// 存储最小生成树状态的全局数组
mst MST_Array[V_SIZE];

/**
 * @brief 初始化生成树节点状态
 */
void initialize() {
    for (int i = 0; i < V_SIZE; i++) {
        MST_Array[i].visited = false;
        MST_Array[i].key = INF; 
        MST_Array[i].near = i;
    }

    // 默认选择 0 号节点作为起点，其初始连接权重为 0
    MST_Array[0].key = 0;
}

/**
 * @brief 贪心寻找最小生成树结构
 */
void updateNear() {
    for (int v = 0; v < V_SIZE; v++) {
        int min_key = INF;
        int minIndex = -1; // 默认设为 -1 标识未找到

        // 贪心选择当前未入树且连接权值最小的顶点
        for (int i = 0; i < V_SIZE; i++) {
            if (!MST_Array[i].visited && MST_Array[i].key < min_key) {
                min_key = MST_Array[i].key;
                minIndex = i;
            }
        }

        // 核心修复：若 minIndex == -1，说明剩余未加入的顶点与当前生成树不连通
        if (minIndex == -1) {
            break; 
        }

        // 将该顶点标记为已并入生成树
        MST_Array[minIndex].visited = true;

        // 根据新加入的顶点 minIndex 更新未入树顶点的连接权值 key
        for (int i = 0; i < V_SIZE; i++) {
            if (graph[minIndex][i] != 0 && graph[minIndex][i] < INF) {
                // 如果通过新顶点连接的边比当前的 key 更小，则更新 key 和父边 near
                if (!MST_Array[i].visited && graph[minIndex][i] < MST_Array[i].key) {
                    MST_Array[i].key = graph[minIndex][i];
                    MST_Array[i].near = minIndex;
                }
            }
        }
    }
}

/**
 * @brief 打印生成树每条边的连接情况
 */
void show() {
    std::cout << "Edge   Weight\n";
    for (int i = 1; i < V_SIZE; i++) { // 从 1 开始打印，跳过根节点 0
        std::cout << MST_Array[i].near << " - " << i << "\t"
                  << graph[i][MST_Array[i].near] << "\n";
    }
}

/**
 * @brief 主函数
 */
int main() {
    initialize();  // 初始化
    updateNear();  // 贪心扩展
    show();        // 打印结果
    return 0;
}
