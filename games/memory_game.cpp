/**
 * @file
 * @brief A simple [Memory Game](https://en.wikipedia.org/wiki/Matching_game) (控制台卡片记忆配对小游戏)
 * @details
 * 记忆翻牌配对游戏：
 * 1. 游戏提供了 3 种不同尺寸的关卡：4x2 (8张卡)、5x2 (10张卡)、7x2 (14张卡)。
 * 2. 系统会在后台随机打乱字母对（如 'A'、'E'、'Z'、'D' 等），玩家需要逐次输入两个位置的索引进行翻牌。
 * 3. 若翻开的两张卡片字母相同，则保持翻开状态；若不同，则在短暂停留后重新盖上。
 * 4. 当全部卡片完成配对后，提示通关并询问是否重新开始。
 *
 * @author [David Leal](https://github.com/Panquesito7)
 */

#include <algorithm>  /// 用于 std::shuffle()
#include <cstdlib>    /// 用于 std::srand()
#include <ctime>      /// 用于 std::time()
#include <iostream>   /// 用于标准输入输出
#include <random>     /// 用于 std::mt19937 生成器
#include <vector>     /// 用于 std::vector 容器

// 根据操作系统编译环境，实现跨平台的休眠延时函数 SLEEP
#ifdef _WIN32
#include <Windows.h>  /// 包含 Windows.h 用于 Sleep()
template <typename T>
constexpr typename std::enable_if<std::is_integral<T>::value, void>::type SLEEP(
    T milliseconds) {
    Sleep(milliseconds * 1000);
}
#else
#include <unistd.h>  /// 包含 unistd.h 用于 sleep()
template <typename T>
constexpr T SLEEP(T seconds) {
    return sleep(seconds);
}
#endif

namespace games {
/**
 * @namespace memory_game
 * @brief 卡片记忆配对游戏逻辑命名空间
 */
namespace memory_game {
/**
 * @brief 输入流合法性校验函数
 * @details 当检测到输入非法（例如要求输入数字却输入了字母），重置输入流状态以防止死循环。
 * @tparam T 输入数据类型
 * @param input 输入变量
 * @returns `false` 输入失败；`true` 输入成功合法
 */
template <typename T>
bool is_number(const T &input) {
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(256, '\n');
        return false;
    }
    return true;
}

/**
 * @brief 初始化游戏卡片矩阵
 * @details 根据卡片总量（size）选择对应的字母集，双份推入，打乱后填充到卡片表中。
 * @tparam T 元素类型
 * @param table 卡片矩阵的 vector 指针
 */
template <typename T>
void init(std::vector<T> *table) {
    std::vector<char> letters(7);

    // 根据不同表格尺寸设定不同的记忆卡片组合
    if ((*table).size() == 10) {  // 5x2 模式
        letters = {'A', 'E', 'Z', 'P', 'D'};
    } else if ((*table).size() == 8) {  // 4x2 模式
        letters = {'A', 'E', 'Z', 'D'};
    } else if ((*table).size() == 14) {  // 7x2 模式
        letters = {'A', 'E', 'Z', 'P', 'D', 'B', 'M'};
    }

    std::vector<char> pairs;
    for (char letter : letters) {
        pairs.push_back(letter); // 每种卡片推入两张以组成配对
        pairs.push_back(letter);
    }

    // 采用洗牌算法打乱卡片顺序
    std::shuffle(pairs.begin(), pairs.end(),
                 std::mt19937(std::random_device()()));

    for (int i = 0; i < (*table).size(); i++) {
        (*table)[i] = pairs[i];
    }

    std::cout << "All available types are: ";
    for (int i = 0; i < letters.size(); i++) {
        if (i == letters.size() - 1) {
            std::cout << "and " << letters[i] << ".\n\n";
        } else {
            std::cout << letters[i] << ", ";
        }
    }
}

/**
 * @brief 打印当前卡片显示矩阵到控制台上
 * @param table 存储当前卡片状态的 vector
 */
template <typename T>
void print_table(const std::vector<T> &table) {
    std::cout << "| ";
    std::vector<T> table_print(table.size());

    for (int i = 0; i < table.size(); i++) {
        table_print[i] = ' ';
        if (table[i] != 0) {
            table_print[i] = table[i]; // 已翻开或配对成功的展示其值，未配对的隐藏
        }
    }

    for (int i = 0; i < table.size(); i++) {
        if (i % 5 == 0 && i != 0) {
            std::cout << "\n| ";
        }
        std::cout << table_print[i] << " | ";
    }
}

// 函数前置声明
template <typename T>
void reset_data(const std::vector<T> &, int *, int *, int *);

/**
 * @brief 请求用户输入要翻开的卡片索引并进行有效性校验
 * @param table 当前游戏状态卡片表
 * @param answer 当前翻开卡片的索引
 * @param old_answer 上一次翻开卡片的索引
 * @param memory_count 翻牌计数器（一次翻两张）
 */
template <typename T>
void ask_data(const std::vector<T> &table, int *answer, int *old_answer,
              int *memory_count) {
    (*old_answer) = (*answer);
    print_table(table);

    std::cout << "\n\nType your response here (number index):\n";
    std::cin >> (*answer);

    // 输入格式校验
    if (!is_number((*answer))) {
        std::cout << "\nYou must enter a valid number.\n\n";
        reset_data(table, answer, old_answer, memory_count);
    }

    (*memory_count)++; // 翻开次数加 1

    // 索引越界校验
    if (((*answer) > table.size()) || ((*answer) < 1)) {
        std::cout << "\nYou can't check a value that doesn't exist (or an invalid number).\n\n";
        reset_data(table, answer, old_answer, memory_count);
    }

    // 重复选择同一张卡片校验
    if ((*old_answer) == (*answer)) {
        std::cout << "\nYou can't check the same value twice.\n\n";
        reset_data(table, answer, old_answer, memory_count);
    }

    // 重复翻开已经配对成功的卡片校验
    if ((table[(*answer) - 1] != 0) &&
        ((table[(*old_answer)] == 0) || (table[(*old_answer)] != 0))) {
        std::cout << "\nYou can't check the same value twice.\n\n";
        reset_data(table, answer, old_answer, memory_count);
    }
}

/**
 * @brief 当发生错误时重置输入状态并重新输入
 */
template <typename T>
void reset_data(const std::vector<T> &table, int *answer, int *old_answer,
                int *memory_count) {
    (*answer) = (*old_answer);
    (*memory_count)--;
    ask_data(table, answer, old_answer, memory_count);
}

/**
 * @brief 判断翻开的两张卡片是否匹配
 * @param table 原始存储全字母排列的卡片表
 * @param table_empty 当前游戏可视化展示表（已匹配完的卡片显示字母）
 * @param answer 当前翻开卡片的位置索引
 * @param first_time 翻牌阶段标志位，若为第一个卡片则等待第二张；若为第二张则进行匹配校验
 * @param old_answer 第一张翻开卡片的位置索引
 * @param memory_count 翻牌计数器
 * @returns `true` 匹配成功或处于第一张翻牌阶段；`false` 两张卡不匹配
 */
template <typename T>
bool match(const std::vector<T> &table, std::vector<T> *table_empty,
           const int &answer, bool *first_time, int *old_answer,
           int *memory_count) {
    if ((*first_time) == true) {
        return true; // 翻开第一张，直接保留显示
    }

    // 翻开第二张，进行匹配比对
    for (int i = 0; i < table.size() + 1; i++) {
        if (i == answer) {
            if (table[i - 1] == table[(*old_answer) - 1]) {
                // 两张牌的值相同 -> 配对成功
                (*first_time) = true;
                (*memory_count) = 0;
                (*old_answer) = 0;
                return true;
            } else {
                // 两张牌的值不相同 -> 配对失败，清除这两张卡片的显示
                std::cout << "\nNo match (value was " << table[i - 1]
                          << ", index is " << i << ").\n\n";

                (*table_empty)[(*old_answer) - 1] = 0;
                (*table_empty)[answer - 1] = 0;

                (*first_time) = true;
                (*memory_count) = 0;
                (*old_answer) = 0;
                return false;
            }
        }
    }

    return false;
}

/**
 * @brief 记录分配翻卡结果并递归控制游戏进程
 */
template <typename T>
void assign_results(std::vector<T> *table_empty, std::vector<T> *table,
                    int *answer, bool *first_time, int *old_answer,
                    int *memory_count) {
    
    for (int i = 0; i < (*table).size() + 1; i++) {
        if (i == (*answer)) {
            if (match((*table), table_empty, (*answer), first_time, old_answer,
                      memory_count) == true) {
                (*table_empty)[i - 1] = (*table)[i - 1]; // 记录正确卡片展示
                (*first_time) = true;
            }
        }
    }

    if ((*memory_count) == 1) {
        (*first_time) = false;
        (*memory_count) = 0;
    }

    char try_again = 'n';

    // 检查游戏是否已经全部通关（table_empty 表被填满）
    for (int i = 0; i < (*table).size() + 1; i++) {
        if ((*table_empty)[i] == 0) {
            break; // 仍有未配对项，跳出循环，继续游戏
        } else if (i == (*table).size() - 1) {
            print_table((*table));

            std::cout << "\n\nYou won. Congratulations! Do you want to play again? (y/n)\n";
            std::cin >> try_again;
            if (try_again == 'y') {
                // 重新开始游戏，清空桌牌数据并重新初始化
                for (int i = 0; i < (*table_empty).size(); i++) {
                    (*table_empty)[i] = 0;
                }
                init(table);
            } else {
                std::cout << "\nThanks for playing the game!\n";
                SLEEP(3);
                exit(0);
            }
        }
    }

    // 未完成，继续递归读入输入并分配比对
    ask_data((*table_empty), answer, old_answer, memory_count);
    assign_results(table_empty, table, answer, first_time, old_answer,
                   memory_count);
}
}  // namespace memory_game
}  // namespace games

/**
 * @brief 主函数
 * @returns 0
 */
int main() {
    std::srand(std::time(nullptr)); // 随机数种子初始化

    int size = 0;
    int selection = 0;

    int response = 0;
    int old_answer = 0;

    int memory_count = 0;
    bool first_time = true;

    std::cout << "\tMEMORY GAME\n";

    // 读入并校验玩家选择的表格尺寸
    do {
        std::cout << "\n1. 4x2 (1)";
        std::cout << "\n2. 5x2 (2)";
        std::cout << "\n3. 7x2 (3)\n";

        std::cout << "\nChoose table size: ";
        std::cin >> selection;
    } while ((selection < 1 || selection > 3) &&
             (!games::memory_game::is_number(selection)));

    switch (selection) {
        case 1:
            size = 8;
            break;
        case 2:
            size = 10;
            break;
        case 3:
            size = 14;
            break;
        default:
            size = 10;
            break;
    }

    std::vector<char> table(size);
    std::vector<char> table_empty(size);

    std::cout << "\n";

    games::memory_game::init(&table);
    games::memory_game::ask_data(table_empty, &response, &old_answer, &memory_count);
    games::memory_game::assign_results(&table_empty, &table, &response,
                                       &first_time, &old_answer, &memory_count);

    return 0;
}
