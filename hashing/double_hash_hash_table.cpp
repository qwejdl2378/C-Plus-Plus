/**
 * @file
 * @brief Implementation of a Hash Table using [Double Hashing](https://en.wikipedia.org/wiki/Double_hashing) (双重哈希法/双重散列哈希表实现)
 *
 * @details
 * 双重哈希是开放寻址法（Open Addressing）中解决哈希冲突的优良方法。
 * 当发生哈希冲突时，它不使用简单的线性或二次增量，而是通过第二个独立的哈希函数计算步长。
 *
 * ### 寻址探查公式
 * $Index_i = (Hash_1(key) + i \cdot Hash_2(key)) \bmod Size$
 * 其中 $i$ 是冲突发生时的探查次数（$i = 0, 1, 2, \dots$）。
 * 这种方法有效避免了线性探查中的“聚集（Clustering）”问题。
 *
 * 时间复杂度: 插入/查询/删除平均 $O(1)$，最坏 $O(N)$。
 * 空间复杂度: $O(N)$。
 *
 * @note
 * 【未匹配键误删导致槽位 0 被非法篡改与内存泄漏 Bug 审计与修复】：
 * 1. **键未找到时仍执行删除覆盖 Bug**：在 `remove` 函数中，
 *    代码调用 `int index = doubleHash(key, true);`。如果键在表中不存在，
 *    `doubleHash` 会返回空槽位的标识符 `notPresent`（其全局默认值为 `0`）。
 *    原有逻辑虽然检测到 `index == notPresent` 并输出了 "key not found"，
 *    **但却未执行 return 中断，仍然无条件执行了 `table[index].key = tomb;`（即覆盖了槽位 0）并减少了哈希表 size 计数**！
 *    这导致每次删除不存在的键，都会把槽位 0 抹除为墓碑 `-1`，并错误减少哈希表有效长度，最终彻底破坏哈希表状态。
 *    **修复**：在 `remove` 函数中增加安全拦截，确认查找到的槽位值等于要删除的 `key`，否则直接返回：
 *    `if (table[index].key != key) { std::cout << "key not found" << std::endl; return; }`
 * 2. **零元素误判限制**：本程序中 `notPresent` 默认初始化为 `0`，这意味着键值 `0` 被保留用作空槽哨兵，不可作为有效数据插入。
 *    已在注释中对此设计限制做了明确说明。
 *
 * @author [achance6](https://github.com/achance6)
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include <functional>

namespace double_hashing {

// 前置声明
struct Entry;
bool putProber(const Entry& entry, int key);
bool searchingProber(const Entry& entry, int key);
void add(int key);

// 全局变量定义
int notPresent = 0;              ///< 哨兵值，表示当前位置没有键（0 无法作为有效键插入）
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
 * @brief 第二个哈希函数（计算冲突解决步长）
 */
size_t otherHashFxn(int key) {
    std::hash<int> hash;
    return 1 + (7 - (hash(key) % 7)); // 步长范围 [1, 7]
}

/**
 * @brief 双重哈希寻址探查核心函数
 * @param key 目标键
 * @param searching true 表示当前处于查找/删除状态，false 表示当前处于插入状态
 * @return 目标键所在槽位索引，或未找到时返回空槽位哨兵 (notPresent)
 */
int doubleHash(int key, bool searching) {
    int hash = static_cast<int>(hashFxn(key) % totalSize);
    int i = 0;
    Entry entry;
    do {
        // 计算双重哈希寻址索引
        int index = static_cast<int>((hash + (i * otherHashFxn(key))) % totalSize);
        if (index < 0) {
            index += totalSize;
        }
        entry = table[index];

        if (searching) {
            // 查找模式：如果遇到完全空的槽位，说明目标键必定不存在
            if (entry.key == notPresent) {
                return notPresent;
            }
            // 遇到匹配的键
            if (searchingProber(entry, key)) {
                return index;
            }
            i++;
        } else {
            // 插入模式：寻找空槽位或墓碑位置插入
            if (putProber(entry, key)) {
                return index;
            }
            i++;
        }

        // 避免死循环防护
        if (i == totalSize * 100) {
            std::cout << "DoubleHash probe failed" << std::endl;
            return notPresent;
        }
    } while (entry.key != notPresent);
    return notPresent;
}

/**
 * @brief 判断插入槽位是否可用（空槽或墓碑）
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
 * @brief 再哈希扩容（当负载因子 >= 0.5 时触发）
 */
void rehash() {
    rehashing = true;
    int oldSize = totalSize;
    std::vector<Entry> oldTable = table;

    // 容量翻倍
    table = std::vector<Entry>(totalSize * 2, Entry(notPresent));
    totalSize *= 2;
    size = 0; // 重置有效元素计数，因为 add 会递增它

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
    int index = doubleHash(key, false);
    table[index].key = key;
    size++;
    // 负载因子超过 0.5 时进行扩容
    if (static_cast<double>(size) / totalSize >= 0.5) {
        rehash();
    }
}

/**
 * @brief 删除指定元素
 */
void remove(int key) {
    int index = doubleHash(key, true);
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

}  // namespace double_hashing

/**
 * @brief 单元自测用例
 */
static void test() {
    using namespace double_hashing;
    totalSize = 7;
    table = std::vector<Entry>(totalSize, Entry(notPresent));
    size = 0;

    // 1. 插入测试
    add(10);
    add(17); // 17 % 7 = 3，若有冲突会通过双重哈希解决
    add(24);

    // 2. 查找测试
    int idx1 = doubleHash(17, true);
    assert(table[idx1].key == 17);

    // 3. 删除测试（并测试误删不存在元素不影响槽位 0 的 Bug 修复效果）
    remove(999); // 不存在的键，应输出 "key not found"，且不发生任何修改
    assert(table[doubleHash(10, true)].key == 10); // 槽位上的 10 应完好无损

    remove(17);
    int idx_removed = doubleHash(17, true);
    assert(idx_removed == notPresent); // 应当找不到

    std::cout << "All Double Hash Table tests passed successfully!\n";
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
    std::cin >> double_hashing::totalSize;
    if (double_hashing::totalSize <= 0) {
        double_hashing::totalSize = 7;
    }
    double_hashing::table = std::vector<double_hashing::Entry>(double_hashing::totalSize);
    double_hashing::size = 0;

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
                double_hashing::addInfo(key);
                break;
            case 2:
                std::cout << "Enter key to remove = ";
                std::cin >> key;
                double_hashing::removalInfo(key);
                break;
            case 3: {
                std::cout << "Enter key to search = ";
                std::cin >> key;
                int index = double_hashing::doubleHash(key, true);
                if (double_hashing::table[index].key == key) {
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
                          << " is = " << double_hashing::hashFxn(key) << std::endl;
                break;
            case 5:
                double_hashing::display();
                break;
            default:
                loop = false;
                break;
        }
        std::cout << std::endl;
    }
    return 0;
}
