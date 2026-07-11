/**
 * @file
 * @brief Implementation of iterative [Depth First Search Algorithm](https://en.wikipedia.org/wiki/Depth-first_search) using an explicit stack (基于显式栈的迭代深度优先搜索算法实现)
 *
 * @details
 * 深度优先搜索（DFS）是一种图遍历算法。与使用函数递归调用栈（类隐式栈）的实现相比，
 * 本文件采用 `std::stack` 在堆上维护显式栈，从而避免了超深图遍历时可能触发的系统调用栈溢出（Stack Overflow）风险。
 *
 * ### 节点标记状态（三色法变种）
 * - `WHITE` (0)：白色，代表节点尚未被探索或访问。
 *   `GREY` (1)：灰色，代表节点已加入栈中，等待被弹出处理。
 * - `BLACK` (2)：黑色，代表节点已出栈并完成了其邻居节点的探索，即已被完全遍历。
 *
 * @note
 * 【注释与代码实现不一致警示】：
 * 在第 64-73 行的 `addEdge` 函数中，注释声明：“这里我们考虑无向图，因此我们将 v 添加到 u 的邻接表，并且将 u 添加到 v 的邻接表”。
 * 然而，**实际代码中只写了单向边插入**：
 *   `(*adj)[u - 1].push_back(v - 1);`
 * 这使得它在物理上是一个**有向图（Directed Graph）**的加边逻辑，这与注释的描述相悖。
 * 如果要支持真正的无向图，应当补充添加反向边。
 *
 * 时间复杂度: O(V + E)
 * 空间复杂度: O(V)
 *
 * @author [Ayaan Khan](http://github.com/ayaankhan98)
 * @author [Saurav Uppoor](https://github.com/sauravUppoor)
 */

#include <iostream> 	 /// 用于输入输出
#include <stack>    	 /// 用于 std::stack
#include <vector>   	 /// 用于 std::vector
#include <cassert>  	 /// 用于 assert 断言
#include <limits>   	 /// 用于数值极限

constexpr int WHITE = 0; // 白色：未访问
constexpr int GREY = 1;	 // 灰色：已入栈未探索
constexpr int BLACK = 2; // 黑色：探索完毕

/**
 * @namespace graph
 * @brief 图算法命名空间
 */
namespace graph {
/**
 * @namespace depth_first_search
 * @brief 深度优先搜索算法命名空间
 */
namespace depth_first_search {
/**
 * @brief 添加有向边（虽然注释写的是无向）
 * @param adj 邻接表指针
 * @param u 起点（1-indexed）
 * @param v 终点（1-indexed）
 */
void addEdge(std::vector<std::vector<size_t>> *adj, size_t u, size_t v) {
	(*adj)[u - 1].push_back(v - 1); // 警告：实际上只有单向插入，构成有向边
}

/**
 * @brief 使用显式栈执行迭代 DFS 遍历
 * @param graph 邻接表形式的图
 * @param start 起始遍历节点索引 (0-indexed)
 * @returns 按照 DFS 访问顺序记录的节点序列 (1-indexed 展现)
 */
std::vector<size_t> dfs(const std::vector<std::vector<size_t> > &graph, size_t start) {
    std::vector<size_t> checked(graph.size(), WHITE); // 初始化所有节点为白色
    std::vector<size_t> traversed_path;               // 保存遍历路径

    checked[start] = GREY;
    std::stack<size_t> stack;
    stack.push(start); // 起点入栈

    // 循环探索栈顶节点
    while (!stack.empty()) {
        int act = stack.top();
        stack.pop();

        // 仅当节点在入栈等待探索状态（灰色）时，才对其邻居进行展开
        if (checked[act] == GREY) {
            traversed_path.push_back(act + 1); // 记录访问节点

            // 遍历所有邻居节点并压入栈中
            for (auto it : graph[act]) {
                stack.push(it);
                if (checked[it] != BLACK) {
                    checked[it] = GREY; // 标记状态为已在栈中
                }
            }
            checked[act] = BLACK;  // 该节点探索结束，染成黑色
        }
    }
    return traversed_path;
}
}  // namespace depth_first_search
}  // namespace graph

/**
 * @brief 单元自测用例
 */
static void tests() {
	size_t start_pos;

	/// 测试 1
	std::cout << "Case 1: " << std::endl;
	start_pos = 1;
	std::vector<std::vector<size_t> > g1(3, std::vector<size_t>());
	graph::depth_first_search::addEdge(&g1, 1, 2);
	graph::depth_first_search::addEdge(&g1, 2, 3);
	graph::depth_first_search::addEdge(&g1, 3, 1);

	std::vector<size_t> expected1 {1, 2, 3};
	assert(graph::depth_first_search::dfs(g1, start_pos - 1) == expected1);
	std::cout << "Passed" << std::endl;

	/// 测试 2
	std::cout << "Case 2: " << std::endl;
	start_pos = 1;
	std::vector<std::vector<size_t> > g2(4, std::vector<size_t>());
	graph::depth_first_search::addEdge(&g2, 1, 2);
	graph::depth_first_search::addEdge(&g2, 1, 3);
	graph::depth_first_search::addEdge(&g2, 2, 4);
	graph::depth_first_search::addEdge(&g2, 4, 1);

	std::vector<size_t> expected2 {1, 3, 2, 4};
	assert(graph::depth_first_search::dfs(g2, start_pos - 1) == expected2);
	std::cout << "Passed" << std::endl;

	/// 测试 3
	std::cout << "Case 3: " << std::endl;
	start_pos = 2;
	std::vector<std::vector<size_t> > g3(4, std::vector<size_t>());
	graph::depth_first_search::addEdge(&g3, 1, 2);
	graph::depth_first_search::addEdge(&g3, 1, 3);
	graph::depth_first_search::addEdge(&g3, 2, 4);
	graph::depth_first_search::addEdge(&g3, 4, 1);

	std::vector<size_t> expected3 {2, 4, 1, 3};
	assert(graph::depth_first_search::dfs(g3, start_pos - 1) == expected3);
	std::cout << "Passed" << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    tests();  // 运行自测

    size_t vertices = 0, edges = 0, start_pos = 1;
	std::vector<size_t> traversal;

    std::cout << "Enter the Vertices : ";
	std::cin >> vertices;
	std::cout << "Enter the Edges : ";
	std::cin >> edges;

    std::vector<std::vector<size_t> > adj(vertices, std::vector<size_t>());

    std::cout << "Enter the vertices which have edges between them : " << std::endl;
	while (edges--) {
		size_t u = 0, v = 0;
		std::cin >> u >> v;
		graph::depth_first_search::addEdge(&adj, u, v);
	}

    std::cout << "Enter the starting vertex [1,n]: " << std::endl;
	std::cin >> start_pos;
	start_pos -= 1;
	traversal = graph::depth_first_search::dfs(adj, start_pos);

    for (auto x : traversal) {
		std::cout << x << ' ';
	}
	std::cout << std::endl;

    return 0;
}
