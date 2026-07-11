/**
 * @file
 * @brief Implementation of the [A* Search Algorithm](https://en.wikipedia.org/wiki/A*_search_algorithm) for the EightPuzzle/15-Puzzle game (A* 启发式搜索算法实现，用于解决八数码/十五数码游戏)
 *
 * @details
 * A* 搜索算法是一种在图形平面上，有多个节点的路径，求出最低通过成本的算法。
 * 它结合了 Dijkstra 算法的节点路径距离（G 值）与最佳优先搜索算法的启发估算距离（H 值）。
 *
 * ### 评估函数
 * $F(n) = G(n) + H(n)$
 * - $G(n)$：从起始状态到当前状态实际所耗费的步数（深度）。
 * - $H(n)$：从当前状态到目标状态的估计成本（启发值，本实现中使用曼哈顿距离 Manhattan Distance）。
 * - $F(n)$：当前节点的估算总成本。算法每次均优先拓展 $F(n)$ 值最小的节点。
 *
 * 时间复杂度: 最坏 $O(b^d)$，其中 $b$ 是分支因子，$d$ 是解的深度。
 * 空间复杂度: 最坏 $O(b^d)$（由于要存储所有生成的节点以避免重复访问）。
 *
 * @note
 * 【潜在实例化编译与安全防卫性 Bug 审计与修复】：
 * 1. **模板类中未调用的拷贝构造函数类型不匹配编译 Bug**：在 `Info` 的拷贝构造函数中，原先写为：
 *    `state(std::make_shared<Puzzle>(A.state))`
 *    其中 `A.state` 是一个 `std::shared_ptr<Puzzle>`。由于 `EightPuzzle` 没有接受 `std::shared_ptr` 的构造函数，
 *    一旦该拷贝构造函数在外部被实例化，会导致直接的编译报错。
 *    **修复**：将其解引用以正确传入底层对象的引用，满足拷贝语义：
 *    `state(std::make_shared<Puzzle>(*A.state))`
 * 2. **无解零查找越界防卫**：在 `find_zero` 中，原先找不到零时返回 `{-1, -1}`。
 *    由于返回类型为无符号的 `uint32_t`，这会产生下溢并映射到极大数值 `4294967295`。
 *    若此时计算移动发生溢出回绕到 0，会在 `in_range` 验证时误判为合法移动并导致越界数组写。
 *    **修复**：在 `find_zero` 尾部添加断言验证，若棋盘损坏没有包含 0 元素则直接触发断言中断。
 *
 * @author [Ashish Daulatabad](https://github.com/AshishYUO)
 */

#include <algorithm>   
#include <array>       
#include <cassert>     
#include <cstdint>     
#include <functional>  
#include <iostream>    
#include <map>         
#include <memory>      
#include <set>         
#include <vector>      

namespace machine_learning {
namespace aystar_search {

/**
 * @class EightPuzzle
 * @brief 八数码/十五数码游戏状态棋盘类
 * @tparam N 棋盘大小，默认 3 (八数码为 3x3，十五数码为 4x4)
 */
template <size_t N = 3>
class EightPuzzle {
    std::array<std::array<uint32_t, N>, N> board;  ///< 二维数组存储棋盘当前的数字分布

    std::vector<std::pair<int8_t, int8_t>> moves = {
        {0, 1},   // 右移
        {1, 0},   // 下移
        {0, -1},  // 左移
        {-1, 0}   // 上移
    };  ///< 可行移动向量组

    /**
     * @brief 寻找棋盘中空格 '0' 所在的位置
     * @return 包含行、列索引的 pair
     */
    std::pair<uint32_t, uint32_t> find_zero() const {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                if (!board[i][j]) {
                    return {i, j};
                }
            }
        }
        // 核心修复：添加安全防卫断言，一旦棋盘数据受损缺少 '0' 则立即抛出中断
        assert(false && "Invalid board state: Space '0' not found!");
        return {0, 0};
    }

    /**
     * @brief 检查越界范围
     */
    inline bool in_range(const uint32_t value) const { return value < N; }

 public:
    /**
     * @brief 获取棋盘中指定行列的数字
     */
    uint32_t get(size_t i, size_t j) const {
        if (in_range(i) && in_range(j)) {
            return board[i][j];
        }
        return -1;
    }

    /**
     * @brief 获取当前棋盘状态
     */
    std::array<std::array<uint32_t, N>, N> get_state() const { return board; }

    /**
     * @brief 获取棋盘尺寸
     */
    inline size_t get_size() const { return N; }

    /**
     * @brief 默认构造函数，生成一个正序的目标棋盘配置
     */
    EightPuzzle() {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                board[i][j] = ((i * N + j + 1) % (N * N));
            }
        }
    }

    /**
     * @brief 参数化构造函数
     * @param init 初始状态配置
     */
    explicit EightPuzzle(const std::array<std::array<uint32_t, N>, N> &init)
        : board(init) {}

    /**
     * @brief 拷贝构造函数
     */
    EightPuzzle(const EightPuzzle<N> &A) : board(A.board) {}

    /**
     * @brief 移动构造函数
     */
    EightPuzzle(EightPuzzle<N> &&A) noexcept
        : board(std::move(A.board)) {}

    ~EightPuzzle() = default;

    /**
     * @brief 拷贝赋值运算符
     */
    EightPuzzle &operator=(const EightPuzzle &A) {
        if (this != &A) {
            board = A.board;
        }
        return *this;
    }

    /**
     * @brief 移动赋值运算符
     */
    EightPuzzle &operator=(EightPuzzle &&A) noexcept {
        if (this != &A) {
            board = std::move(A.board);
        }
        return *this;
    }

    /**
     * @brief 基于空格的四个方向移动，生成所有可能的后续棋盘状态列表
     */
    std::vector<EightPuzzle<N>> generate_possible_moves() const {
        auto zero_pos = find_zero();
        std::vector<EightPuzzle<N>> NewStates;
        for (auto &move : moves) {
            uint32_t next_i = zero_pos.first + move.first;
            uint32_t next_j = zero_pos.second + move.second;
            if (in_range(next_i) && in_range(next_j)) {
                std::array<std::array<uint32_t, N>, N> new_config = board;
                std::swap(new_config[zero_pos.first][zero_pos.second],
                          new_config[next_i][next_j]);
                NewStates.emplace_back(EightPuzzle<N>(new_config));
            }
        }
        return NewStates;
    }

    /**
     * @brief 运算符重载：相等比较
     */
    bool operator==(const EightPuzzle<N> &check) const {
        return board == check.board;
    }

    /**
     * @brief 运算符重载：小于比较（以便放入 std::set 或 std::map）
     */
    bool operator<(const EightPuzzle<N> &check) const {
        return board < check.board;
    }

    /**
     * @brief 运算符重载：小于或等于
     */
    bool operator<=(const EightPuzzle<N> &check) const {
        return board <= check.board;
    }

    /**
     * @brief 友元输出函数，方便直接打印当前棋盘
     */
    friend std::ostream &operator<<(std::ostream &op,
                                    const EightPuzzle<N> &SomeState) {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                op << SomeState.board[i][j] << " ";
            }
            op << "\n";
        }
        return op;
    }
};

/**
 * @class AyStarSearch
 * @brief A* 搜索算法类
 * @tparam Puzzle 泛型表示状态，必须提供 generate_possible_moves() 及 `<` 等运算符
 */
template <typename Puzzle>
class AyStarSearch {
    /**
     * @struct Info
     * @brief 内部结构，封装了搜索节点的状态、G值、H值等信息
     */
    typedef struct Info {
        std::shared_ptr<Puzzle> state;  ///< 当前棋盘状态指针
        size_t heuristic_value = 0;     ///< H 值：到目标状态的估算开销
        size_t depth = 0;               ///< G 值：从初始点到该处的实际路径深度

        Info() = default;

        explicit Info(const Puzzle &A) : state(std::make_shared<Puzzle>(A)) {}

        Info(const Puzzle &A, size_t h_value, size_t d)
            : state(std::make_shared<Puzzle>(A)),
              heuristic_value(h_value),
              depth(d) {}

        /**
         * @brief 拷贝构造函数
         * @note 核心修复：修改为解引用 *A.state，避免 std::shared_ptr 隐式转换类型不匹配编译错误
         */
        Info(const Info &A)
            : state(std::make_shared<Puzzle>(*A.state)),
              heuristic_value(A.heuristic_value),
              depth(A.depth) {}

        Info(Info &&A) noexcept
            : state(std::move(A.state)),
              heuristic_value(A.heuristic_value),
              depth(A.depth) {}

        Info &operator=(const Info &A) {
            if (this != &A) {
                state = std::make_shared<Puzzle>(*A.state);
                heuristic_value = A.heuristic_value;
                depth = A.depth;
            }
            return *this;
        }

        Info &operator=(Info &&A) noexcept {
            if (this != &A) {
                state = std::move(A.state);
                heuristic_value = A.heuristic_value;
                depth = A.depth;
            }
            return *this;
        }

        ~Info() = default;
    } Info;

    std::shared_ptr<Info> Initial;  ///< 起始节点
    std::shared_ptr<Info> Final;    ///< 目标节点

    /**
     * @brief std::set / std::map 自定义排序器
     */
    struct comparison_operator {
        bool operator()(const std::shared_ptr<Info> &a,
                        const std::shared_ptr<Info> &b) const {
            return *(a->state) < *(b->state);
        }
    };

 public:
    using MapOfPuzzleInfoWithPuzzleInfo =
        std::map<std::shared_ptr<Info>, std::shared_ptr<Info>,
                 comparison_operator>;

    using MapOfPuzzleInfoWithInteger =
        std::map<std::shared_ptr<Info>, uint32_t, comparison_operator>;

    using SetOfPuzzleInfo =
        std::set<std::shared_ptr<Info>, comparison_operator>;

    /**
     * @brief 构造函数
     * @param initial 初始棋盘状态
     * @param final 目标期望状态
     */
    AyStarSearch(const Puzzle &initial, const Puzzle &final) {
        Initial = std::make_shared<Info>(initial);
        Final = std::make_shared<Info>(final);
    }

    /**
     * @brief 构造出从起始状态到解目标状态的路径向量
     */
    std::vector<Puzzle> Solution(
        std::shared_ptr<Info> FinalState,
        const MapOfPuzzleInfoWithPuzzleInfo &parent_of) {
        auto current_state = FinalState;
        std::vector<Puzzle> answer;
        while (current_state != nullptr) {
            answer.emplace_back(*current_state->state);
            auto it = parent_of.find(current_state);
            if (it != parent_of.end()) {
                current_state = it->second;
            } else {
                current_state = nullptr;
            }
        }
        return answer;
    }

    /**
     * @brief A* 启发式搜索算法核心逻辑
     * @param dist 启发估算函数 H(n) 的计算逻辑
     * @param permissible_depth 最大允许的最大搜索深度，防止爆栈
     * @return 返回还原步数列表（逆向，自 Final 回溯到 Initial）
     */
    std::vector<Puzzle> a_star_search(
        const std::function<uint32_t(const Puzzle &, const Puzzle &)> &dist,
        const uint32_t permissible_depth = 30) {
        MapOfPuzzleInfoWithPuzzleInfo parent_of;       
        MapOfPuzzleInfoWithInteger g_score;  
        SetOfPuzzleInfo open_list;           ///< 开启列表：待探查扩展的节点
        SetOfPuzzleInfo closed_list;         ///< 关闭列表：已处理的节点

        open_list.emplace(Initial);
        parent_of[Initial] = nullptr;
        g_score[Initial] = 0;

        while (!open_list.empty()) {
            typename SetOfPuzzleInfo::iterator it_low_f_score;
            uint32_t min_f_score = 1e9;

            // 寻找开启列表中 F 值最小的节点进行处理
            for (auto iter = open_list.begin(); iter != open_list.end(); ++iter) {
                uint32_t f_score = (*iter)->heuristic_value + (*iter)->depth;
                if (f_score < min_f_score) {
                    min_f_score = f_score;
                    it_low_f_score = iter;
                }
            }

            std::shared_ptr<Info> current_state = *it_low_f_score;

            // 达到目标状态，构建并返回解路径
            if (*(current_state->state) == *(Final->state)) {
                return Solution(current_state, parent_of);
            }

            open_list.erase(it_low_f_score);

            if (current_state->depth >= permissible_depth) {
                continue;
            }

            // 寻找邻居节点（当前所有合法移动组合）
            std::vector<Puzzle> total_possible_moves =
                current_state->state->generate_possible_moves();

            for (Puzzle &neighbor : total_possible_moves) {
                std::shared_ptr<Info> Neighbor = std::make_shared<Info>(
                    neighbor, dist(neighbor, *(Final->state)),
                    current_state->depth + 1U);
                uint32_t temp_g_score = Neighbor->depth;

                // 若该邻居存在于关闭列表中，且新的深度较浅，则从关闭列表抹除以便二次探索
                auto closed_list_iter = closed_list.find(Neighbor);
                if (closed_list_iter != closed_list.end()) {
                    if (Neighbor->depth < (*closed_list_iter)->depth) {
                        closed_list.erase(closed_list_iter);
                    } else {
                        continue;
                    }
                }

                // 维护 G 值记录
                auto neighbor_g_score_iter = g_score.find(Neighbor);
                if (neighbor_g_score_iter != g_score.end()) {
                    if (neighbor_g_score_iter->second > temp_g_score) {
                        neighbor_g_score_iter->second = temp_g_score;
                        parent_of[Neighbor] = current_state;
                    }
                } else {
                    g_score[Neighbor] = temp_g_score;
                    parent_of[Neighbor] = current_state;
                }

                // 维护开启列表
                auto iter = open_list.find(Neighbor);
                if (iter == open_list.end()) {
                    open_list.emplace(Neighbor);
                } else if ((*iter)->depth > Neighbor->depth) {
                    (*iter)->depth = Neighbor->depth;
                }
            }
            closed_list.emplace(current_state);
        }
        return std::vector<Puzzle>(0); // 无解返回空
    }
};

}  // namespace aystar_search
}  // namespace machine_learning

/**
 * @brief 单元自测用例
 */
static void test() {
    using matrix3 = std::array<std::array<uint32_t, 3>, 3>;
    using row3 = std::array<uint32_t, 3>;
    using matrix4 = std::array<std::array<uint32_t, 4>, 4>;
    using row4 = std::array<uint32_t, 4>;

    // 用例 1：简单的 8-Puzzle 探查还原测试
    matrix3 puzzle;
    puzzle[0] = row3({0, 2, 3});
    puzzle[1] = row3({1, 5, 6});
    puzzle[2] = row3({4, 7, 8});

    matrix3 ideal;
    ideal[0] = row3({1, 2, 3});
    ideal[1] = row3({4, 5, 6});
    ideal[2] = row3({7, 8, 0});

    // 曼哈顿启发式评价函数定义
    auto manhattan_distance =
        [](const machine_learning::aystar_search::EightPuzzle<3> &first,
           const machine_learning::aystar_search::EightPuzzle<3> &second) {
            uint32_t ret = 0;
            for (size_t i = 0; i < first.get_size(); ++i) {
                for (size_t j = 0; j < first.get_size(); ++j) {
                    uint32_t find = first.get(i, j);
                    size_t m = first.get_size(), n = first.get_size();
                    for (size_t k = 0; k < second.get_size(); ++k) {
                        for (size_t l = 0; l < second.get_size(); ++l) {
                            if (find == second.get(k, l)) {
                                std::tie(m, n) = std::make_pair(k, l);
                                break;
                            }
                        }
                        if (m != first.get_size()) {
                            break;
                        }
                    }
                    if (m != first.get_size()) {
                        ret += (std::max(m, i) - std::min(m, i)) +
                               (std::max(n, j) - std::min(n, j));
                    }
                }
            }
            return ret;
        };

    machine_learning::aystar_search::EightPuzzle<3> Puzzle(puzzle);
    machine_learning::aystar_search::EightPuzzle<3> Ideal(ideal);
    machine_learning::aystar_search::AyStarSearch<
        machine_learning::aystar_search::EightPuzzle<3>>
        search(Puzzle, Ideal);

    std::vector<matrix3> answer;
    answer.push_back(matrix3({row3({0, 2, 3}), row3({1, 5, 6}), row3({4, 7, 8})}));
    answer.push_back(matrix3({row3({1, 2, 3}), row3({0, 5, 6}), row3({4, 7, 8})}));
    answer.push_back(matrix3({row3({1, 2, 3}), row3({4, 5, 6}), row3({0, 7, 8})}));
    answer.push_back(matrix3({row3({1, 2, 3}), row3({4, 5, 6}), row3({7, 0, 8})}));
    answer.push_back(matrix3({row3({1, 2, 3}), row3({4, 5, 6}), row3({7, 8, 0})}));

    auto Solution = search.a_star_search(manhattan_distance);
    std::cout << "Solution 1 steps: " << Solution.size() << std::endl;
    assert(Solution.size() == answer.size());

    uint32_t idx = 0;
    for (auto it = Solution.rbegin(); it != Solution.rend(); ++it) {
        assert(it->get_state() == answer[idx]);
        ++idx;
    }

    // 用例 2：复杂的 8-Puzzle 探查还原测试
    puzzle[0] = row3({5, 7, 3});
    puzzle[1] = row3({2, 0, 6});
    puzzle[2] = row3({1, 4, 8});
    ideal[0] = row3({1, 2, 3});
    ideal[1] = row3({4, 5, 6});
    ideal[2] = row3({7, 8, 0});

    Puzzle = machine_learning::aystar_search::EightPuzzle<3>(puzzle);
    Ideal = machine_learning::aystar_search::EightPuzzle<3>(ideal);
    search = machine_learning::aystar_search::AyStarSearch<
        machine_learning::aystar_search::EightPuzzle<3>>(Puzzle, Ideal);

    Solution = search.a_star_search(manhattan_distance);
    std::cout << "Solution 2 steps: " << Solution.size() << std::endl;
    assert(13 == Solution.size());
    assert(Solution[0].get_state() == ideal);

    // 用例 3：15-Puzzle (4x4) 探查还原测试
    matrix4 puzzle2;
    puzzle2[0] = row4({10, 1, 6, 2});
    puzzle2[1] = row4({5, 8, 4, 3});
    puzzle2[2] = row4({13, 0, 7, 11});
    puzzle2[3] = row4({14, 9, 15, 12});
    matrix4 ideal2;
    ideal2[0] = row4({1, 2, 3, 4});
    ideal2[1] = row4({5, 6, 7, 8});
    ideal2[2] = row4({9, 10, 11, 12});
    ideal2[3] = row4({13, 14, 15, 0});

    machine_learning::aystar_search::EightPuzzle<4> Puzzle2(puzzle2), Ideal2(ideal2);
    machine_learning::aystar_search::AyStarSearch<
        machine_learning::aystar_search::EightPuzzle<4>>
        search2(Puzzle2, Ideal2);

    auto manhattan_distance2 =
        [](const machine_learning::aystar_search::EightPuzzle<4> &first,
           const machine_learning::aystar_search::EightPuzzle<4> &second) {
            uint32_t ret = 0;
            for (size_t i = 0; i < first.get_size(); ++i) {
                for (size_t j = 0; j < first.get_size(); ++j) {
                    uint32_t find = first.get(i, j);
                    size_t m = first.get_size(), n = first.get_size();
                    for (size_t k = 0; k < second.get_size(); ++k) {
                        for (size_t l = 0; l < second.get_size(); ++l) {
                            if (find == second.get(k, l)) {
                                std::tie(m, n) = std::make_pair(k, l);
                                break;
                            }
                        }
                        if (m != first.get_size()) {
                            break;
                        }
                    }
                    if (m != first.get_size()) {
                        ret += (std::max(m, i) - std::min(m, i)) +
                               (std::max(n, j) - std::min(n, j));
                    }
                }
            }
            return ret;
        };

    auto sol2 = search2.a_star_search(manhattan_distance2);
    std::cout << "Solution 3 steps: " << sol2.size() << std::endl;
    assert(24 == sol2.size());
    assert(sol2[0].get_state() == ideal2);
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试用例确认正确性
    return 0;
}
