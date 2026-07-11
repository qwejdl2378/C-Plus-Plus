/**
 * @file
 * @brief Implementation of a Hash Table using [Quadratic Probing](https://en.wikipedia.org/wiki/Quadratic_probing) (二次探查法散列表实现)
 *
 * @details
 * 二次探查法是开放寻址（Open Addressing）解决散列冲突的一种改进方法。
 * 与线性探查不同，二次探查的步长随着探查次数的平方增长，从而有效减少了线性探查中的一次聚集现象，
 * 但会产生“二次聚集（Secondary Clustering）”问题。
 *
 * ### 寻址探查公式
 * $Index_i = (Hash(key) + i^2) \bmod Size$
 * 其中 $i$ 是冲突发生时的探查次数。
 *
 * 时间复杂度: 插入/查询/删除平均 $O(1)$，最坏 $O(N)$。
 * 空间复杂度: $O(N)$。
 *
 * @note
 * 【未匹配键误删导致槽位 0 被非法篡改与内存散列计数损坏 Bug 审计与修复】：
 * 1. **键未找到时仍执行删除覆盖 Bug**：在 `remove` 函数中，
 *    如果键在散列表中不存在，`quadraticProbe` 返回空槽哨兵 `notPresent`（即 `0`）。
 *    原有逻辑只输出了 "key not found" 却依然执行了 `table[index].key = tomb;`，导致覆盖槽位 0 且破坏散列表大小计数。
 *    **修复**：在 `remove` 中增加安全边界条件拦截：
 *    `if (table[index].key != key) { std::cout << "key not found" << std::endl; return; }`
 * 2. **浮点运算优化**：原算法中使用 `std::round(std::pow(i, 2))` 来计算 $i^2$，不仅慢而且在极高冲突下存在潜在浮点精度误差。
 *    **修复**：直接使用纯整型乘法 `i * i`，更加高效且结果 100% 精确。
 * 3. **零元素误判限制**：`notPresent` 默认初始化为 `0`，零被保留用作空槽哨兵。
 *
 * @author [achance6](https://github.com/achance6)
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <cmath>
#include <iostream>
#include <vector>
#include <cassert>
#include <functional>

namespace quadratic_probing {

// 前置声明
struct Entry;
bool putProber(const Entry& entry, int key);
bool searchingProber(const Entry& entry, int key);
void add(int key);

// 全局变量定义
int notPresent = 0;              ///< 哨兵值，表示当前位置没有键
std::vector<Entry> table;        ///< 哈希表数组
int totalSize = 0;               ///< 哈希表总容量
int tomb = -1;                   ///< 墓碑值，表示该位置的键已被删除
int size = 0;                    ///< 当前有效键个数
bool rehashing = false;          ///< 标识当前是否正在执行再哈希扩容

/**
 * @brief 哈希表项结构体
 */
struct Entry {
    explicit Entry(int key = notPresent) : key(key) {}
    int key;
};

/**
 * @brief 第一个哈希函数（计算初始槽位）
 */
size_t hashFxn(int key) {
    std::hash<int> hash;
    return hash(key);
}

/**
 * @brief 二次寻址探查核心函数
 * @param key 目标键
 * @param searching true 表示当前处于查找/删除状态，false 表示当前处于插入状态
 * @return 目标键所在槽位索引，或未找到时返回空槽位哨兵 (notPresent)
 */
int quadraticProbe(int key, bool searching) {
    int hash = static_cast<int>(hashFxn(key) % totalSize);
    int i = 0;
    Entry entry;
    do {
        // 核心优化：直接使用 i * i 替代原有的 std::round(std::pow(i, 2)) 浮点数计算
        int index = static_cast<int>((hash + (i * i)) % totalSize);
        if (index < 0) {
            index += totalSize;
        }
        entry = table[index];

        if (searching) {
            if (entry.key == notPresent) {
                return notPresent;
            }
            if (searchingProber(entry, key)) {
                return index;
            }
            i++;
        } else {
            if (putProber(entry, key)) {
                return index;
            }
            i++;
        }

        if (i == totalSize * 100) {
            std::cout << "Quadratic probe failed (infinite loop)" << std::endl;
            return notPresent;
        }
    } while (entry.key != notPresent);
    return notPresent;
}

/**
 * @brief 判断插入槽位是否可用
 */
bool putProber(const Entry& entry, int key) {
    return (entry.key == notPresent || entry.key == tomb);
}

/**
 * @brief 判断当前槽位的键是否匹配
 */
bool searchingProber(const Entry& entry, int key) {
    return (entry.key == key);
}

/**
 * @brief 在表中查找特定键对应项
 */
Entry find(int key) {
    int index = quadraticProbe(key, true);
    if (table[index].key != key) {
        return Entry(notPresent);
    }
    return table[index];
}

/**
 * @brief 打印显示哈希表内容
 */
void display() {
    for (int i = 0; i < totalSize; i++) {
        if (table[i].key == notPresent) {
            std::cout << " Empty ";
        } else if (table[i].key == tomb) {
            std::cout << " Tomb ";
        } else {
            std::cout << " " << table[i].key << " ";
        }
    }
    std::cout << std::endl;
}

/**
 * @brief 再哈希扩容
 */
void rehash() {
    rehashing = true;
    int oldSize = totalSize;
    std::vector<Entry> oldTable = table;

    // 容量翻倍
    table = std::vector<Entry>(totalSize * 2, Entry(notPresent));
    totalSize *= 2;
    size = 0; // 重置有效元素计数

    for (int i = 0; i < oldSize; i++) {
        if (oldTable[i].key != tomb && oldTable[i].key != notPresent) {
            add(oldTable[i].key);
        }
    }

    rehashing = false;
    std::cout << "Table was rehashed, new size is: " << totalSize << std::endl;
}

/**
 * @brief 插入新元素
 */
void add(int key) {
    int index = quadraticProbe(key, false);
    table[index].key = key;
    size++;
    if (static_cast<double>(size) / totalSize >= 0.5) {
        rehash();
    }
}

/**
 * @brief 删除指定元素
 */
void remove(int key) {
    int index = quadraticProbe(key, true);
    // 核心修复：确认查找到的 index 处确实存放着对应的 key，防止误删覆盖槽位 0
    if (table[index].key != key) {
        std::cout << "key not found" << std::endl;
        return;
    }
    table[index].key = tomb;
    std::cout << "Removal successful, leaving tombstone" << std::endl;
    size--;
}

/**
 * @brief 带有描述信息的插入包装函数
 */
void addInfo(int key) {
    std::cout << "Initial table: ";
    display();
    add(key);
    std::cout << "New table: ";
    display();
}

/**
 * @brief 带有描述信息的删除包装函数
 */
void removalInfo(int key) {
    std::cout << "Initial table: ";
    display();
    remove(key);
    std::cout << "New table: ";
    display();
}

}  // namespace quadratic_probing

/**
 * @brief 单元自测用例
 */
static void test() {
    using namespace quadratic_probing;
    totalSize = 7;
    table = std::vector<Entry>(totalSize, Entry(notPresent));
    size = 0;

    // 1. 插入测试
    add(3);
    add(10); // 冲突，使用二次步长探查寻址 10 % 7 = 3
    add(17);

    // 2. 查找测试
    int idx1 = quadraticProbe(10, true);
    assert(table[idx1].key == 10);

    // 3. 删除测试
    remove(999); // 不存在的元素
    assert(table[quadraticProbe(3, true)].key == 3); // 槽位 3 元素不应受影响

    remove(10);
    int idx_removed = quadraticProbe(10, true);
    assert(idx_removed == notPresent);

    std::cout << "All Quadratic Probing Table tests passed successfully!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行单元测试

    std::cout << "Would you like to run interactive mode? (1 for Yes, 0 for No): ";
    int run_interactive = 0;
    if (!(std::cin >> run_interactive) || run_interactive == 0) {
        return 0;
    }

    int cmd = 0, key = 0;
    std::cout << "Enter the initial size of Hash Table. = ";
    std::cin >> quadratic_probing::totalSize;
    if (quadratic_probing::totalSize <= 0) {
        quadratic_probing::totalSize = 7;
    }
    quadratic_probing::table = std::vector<quadratic_probing::Entry>(quadratic_probing::totalSize);
    quadratic_probing::size = 0;

    bool loop = true;
    while (loop) {
        std::cout << std::endl;
        std::cout << "PLEASE CHOOSE -" << std::endl;
        std::cout << "1. Add key. (Numeric only)" << std::endl;
        std::cout << "2. Remove key." << std::endl;
        std::cout << "3. Find key." << std::endl;
        std::cout << "4. Generate Hash. (Numeric only)" << std::endl;
        std::cout << "5. Display Hash table." << std::endl;
        std::cout << "6. Exit." << std::endl;
        if (!(std::cin >> cmd)) {
            break;
        }
        switch (cmd) {
            case 1:
                std::cout << "Enter key to add = ";
                std::cin >> key;
                quadratic_probing::addInfo(key);
                break;
            case 2:
                std::cout << "Enter key to remove = ";
                std::cin >> key;
                quadratic_probing::removalInfo(key);
                break;
            case 3: {
                std::cout << "Enter key to search = ";
                std::cin >> key;
                int index = quadratic_probing::quadraticProbe(key, true);
                if (quadratic_probing::table[index].key == key) {
                    std::cout << "Key " << key << " found at index " << index << std::endl;
                } else {
                    std::cout << "Key not present" << std::endl;
                }
                break;
            }
            case 4:
                std::cout << "Enter element to generate hash = ";
                std::cin >> key;
                std::cout << "Hash of " << key
                          << " is = " << quadratic_probing::hashFxn(key) << std::endl;
                break;
            case 5:
                quadratic_probing::display();
                break;
            default:
                loop = false;
                break;
        }
        std::cout << std::endl;
    }
    return 0;
}
