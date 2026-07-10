/**
 * \file
 * \brief [Disjoint Sets Data Structure](https://en.wikipedia.org/wiki/Disjoint-set_data_structure) (并查集数据结构实现)
 *
 * \author [leoyang429](https://github.com/leoyang429)
 *
 * \details
 * 并查集（Disjoint Set Union, DSU）是一种用于管理一系列不相交集合的数据结构。
 * 典型应用场景包括：寻找无向图的连通分量、Kruskal 最小生成树算法等。
 * 主要包含两种基本操作：
 * 1) Find：查询元素属于哪个集合（即找到它的根节点代表）。
 * 2) Union：将两个不同的集合合并为一个集合。
 */

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

vector<int> root; ///< 存储各节点的双亲节点引用 (root[i] 表示节点 i 的父节点)
vector<int> rank; ///< 存储各集合树的高度级排 (用来做按秩合并优化)

/**
 * @brief 初始化并查集，创建 n 个单元素集合
 * @param n 并查集中元素的上限个数
 */
void CreateSet(int n) {
    root = vector<int>(n + 1);
    rank = vector<int>(n + 1, 1);
    for (int i = 1; i <= n; ++i) {
        root[i] = i; // 初始状态下，每个节点的父节点都是其自身
    }
}

/**
 * @brief 查找元素 x 所在集合的代表元（根节点）
 * @details 采用了**路径压缩**（Path Compression）技术。
 * 在寻找根节点的过程中，将路径上的所有节点直接挂载到根节点下面，
 * 使得后续查询的时间复杂度均摊为接近 O(1) 的反阿克曼函数值。
 * 
 * @param x 查询的目标节点
 * @return 目标节点所属集合的根节点索引
 */
int Find(int x) {
    if (root[x] == x) {
        return x;
    }
    // 递归查找根节点，并在回溯时直接更新 root 指针，实现路径压缩
    return root[x] = Find(root[x]);
}

/**
 * @brief 检查两个元素 x 和 y 是否属于同一个集合
 * @param x 元素一
 * @param y 元素二
 * @return `true` 代表属于同一个集合；`false` 否则
 */
bool InSameUnion(int x, int y) { return Find(x) == Find(y); }

/**
 * @brief 合并元素 x 和 y 所在的两个不同集合
 * @details 采用了**按秩合并**（Union by Rank）技术。
 * 将秩（树的高度）较低的根节点连接到较深的根节点下面，避免树退化为链表，维持树高在 O(log n)。
 * 
 * @param x 元素一
 * @param y 元素二
 */
void Union(int x, int y) {
    int a = Find(x);
    int b = Find(y);
    if (a != b) {
        if (rank[a] < rank[b]) {
            root[a] = b; // a 树较矮，挂在 b 树下
        } else if (rank[a] > rank[b]) {
            root[b] = a; // b 树较矮，挂在 a 树下
        } else {
            root[a] = b; // 高度相同，挂载后 b 树高度加 1
            ++rank[b];
        }
    }
}

/**
 * @brief 主函数，运行基本的并查集功能测试
 * @returns 0
 */
int main() {
    // 测试初始化与 Find 状态
    int n = 100;
    CreateSet(n);
    for (int i = 1; i <= 100; ++i) {
        if (root[i] != i) {
            cout << "Fail" << endl;
            break;
        }
    }
    // 测试合并操作与同集判断
    cout << "1 and 2 are initially not in the same subset" << endl;
    if (InSameUnion(1, 2)) {
        cout << "Fail" << endl;
    }
    Union(1, 2);
    cout << "1 and 2 are now in the same subset" << endl;
    if (!InSameUnion(1, 2)) {
        cout << "Fail" << endl;
    }
    return 0;
}
