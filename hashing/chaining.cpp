/**
 * @file
 * @brief Implementation of a Hash Table using [Chaining (Linked Lists)](https://en.wikipedia.org/wiki/Hash_chain) (拉链法/链地址法哈希表实现)
 *
 * @details
 * 链地址法（Chaining）是解决哈希冲突（Hash Collision）的一种主流方法。
 * 哈希表内部维护一个链表数组（在 C++ 中由 `std::vector` 和 `std::shared_ptr` 组成）。
 * 当多个不同的键映射到同一个哈希槽（Bucket）时，它们会被以链表结点的形式依次追加链接在该槽位后。
 *
 * 时间复杂度: 插入 $O(1)$，查询/删除平均 $O(1)$，最坏 $O(N)$（所有元素均冲突到同一链表）。
 * 空间复杂度: $O(N + M)$，其中 $N$ 是插入的元素个数，$M$ 是哈希表容量（`_mod`）。
 *
 * @note
 * 【负键越界访问导致内存崩溃与空指针解引用 Bug 审计与修复】：
 * 1. **负键导致向量越界读取崩溃 Bug**：在原有交互逻辑中，
 *    查找元素（`case 2`）时直接调用 `h = mychain.hash(x);` 并传入 `find(x, h)`。
 *    在 C++ 中，对负数取模 `x % _mod` 会返回负的哈希值 `h`。
 *    若此时直接执行 `head[h]`（如 `head[-1]`），会触发 **Vector 越界访问段错误** 导致程序崩溃。
 *    **修复**：在 `hash` 函数内部进行数学标准化，若余数为负则加上模数，确保哈希值恒在 `[0, _mod - 1]` 区间内：
 *    `int h = x % _mod; if (h < 0) h += _mod;`
 * 2. **元素不存在时 find 函数尾部空指针解引用 Bug**：原 `find` 函数在链表中扫描元素时：
 *    `while (temp->data != x && temp->next) temp = temp->next;`
 *    随后执行：
 *    `if (temp->next) { return true; }`
 *    若链表中只有一个节点且不等于 `x`，`temp->next` 为 `nullptr`，跳出循环。
 *    此时满足最后一个 `if (temp->data == x)` 条件，若不等于 `x`，会返回 `false`。
 *    但如果链表为空（`temp == nullptr`），虽然有首部判断，但防御不够彻底。
 *    **修复**：重构 `find` 扫描逻辑，使其优雅、安全且语义清晰，避免任何可能的空指针异常。
 *
 * @author [vasutomar](https://github.com/vasutomar)
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <cmath>
#include <iostream>
#include <memory>
#include <vector>
#include <cassert>

/**
 * @brief 拉链法哈希表类
 */
class hash_chain {
 private:
    /**
     * @brief 链表结点定义
     */
    using Node = struct Node {
        int data{};                         ///< 节点中存储的数据值
        std::shared_ptr<struct Node> next;  ///< 指向下一个节点的共享指针
    };

    std::vector<std::shared_ptr<Node>> head;  ///< 槽位数组，存放每个链表的头节点
    int _mod;                                 ///< 哈希表容量（模数）

 public:
    /**
     * @brief 构造函数
     * @param mod 哈希表槽位数，强制防御小于 1 的输入
     */
    explicit hash_chain(int mod) : _mod(mod <= 0 ? 10 : mod) {
        head.assign(_mod, nullptr);
    }

    /**
     * @brief 向哈希表指定槽位中插入一个新元素（尾插法）
     * @param x 待插入的元素值
     * @param h 已经计算好的哈希槽位索引
     */
    void add(int x, int h) {
        // 边界防护：防止外部传入非法越界槽位
        if (h < 0 || h >= _mod) {
            return;
        }

        std::shared_ptr<Node> temp = std::make_shared<Node>();
        temp->data = x;
        temp->next = nullptr;

        if (!head[h]) {
            head[h] = temp;
        } else {
            std::shared_ptr<Node> curr = head[h];
            while (curr->next) {
                curr = curr->next;
            }
            curr->next = temp;
        }
    }

    /**
     * @brief 在控制台输出当前哈希表的完整状态
     */
    void display() const {
        for (int i = 0; i < _mod; i++) {
            if (!head[i]) {
                std::cout << "Key " << i << " is empty" << std::endl;
            } else {
                std::cout << "Key " << i << " has values = ";
                std::shared_ptr<Node> temp = head[i];
                while (temp) {
                    std::cout << temp->data << " -> ";
                    temp = temp->next;
                }
                std::cout << "nullptr" << std::endl;
            }
        }
    }

    /**
     * @brief 哈希函数（计算槽位索引）
     * @param x 待计算的数值
     * @return 映射后的槽位 [0, _mod - 1]
     */
    virtual int hash(int x) const {
        // 核心修复：对取模结果进行标准化，防止负数引起负索引越界崩溃
        int h = x % _mod;
        if (h < 0) {
            h += _mod;
        }
        return h;
    }

    /**
     * @brief 在哈希表中查找指定元素
     * @param x 待查找的数值
     * @param h 对应的哈希槽位索引
     * @return true 找到元素, false 未找到
     */
    bool find(int x, int h) const {
        if (h < 0 || h >= _mod || !head[h]) {
            std::cout << "Element not found" << std::endl;
            return false;
        }

        std::shared_ptr<Node> temp = head[h];
        // 核心修复：采用标准清晰的链表遍历查找
        while (temp) {
            if (temp->data == x) {
                std::cout << "Element found" << std::endl;
                return true;
            }
            temp = temp->next;
        }

        std::cout << "Element not found" << std::endl;
        return false;
    }
};

/**
 * @brief 单元自测用例
 */
static void test() {
    hash_chain table(5);

    // 1. 插入正数与负数测试
    table.add(10, table.hash(10)); // 映射到 0
    table.add(15, table.hash(15)); // 发生冲突，挂在 0 链表的尾部
    table.add(-3, table.hash(-3)); // 负数，-3 % 5 = -3 -> 映射到 2，验证负余数标准化修复

    // 2. 验证查找
    assert(table.find(10, table.hash(10)) == true);
    assert(table.find(15, table.hash(15)) == true);
    assert(table.find(-3, table.hash(-3)) == true);
    assert(table.find(100, table.hash(100)) == false); // 不存在

    std::cout << "All Hash Chaining tests passed successfully!\n";
}

/**
 * @brief 主函数
 */
int main() {
    test(); // 运行自动化测试确保代码稳健性

    // 依然支持交互式模式输入
    std::cout << "Would you like to run interactive mode? (1 for Yes, 0 for No): ";
    int run_interactive = 0;
    if (!(std::cin >> run_interactive) || run_interactive == 0) {
        return 0;
    }

    int c = 0, x = 0, mod = 0, h = 0;
    std::cout << "Enter the size of Hash Table. = " << std::endl;
    std::cin >> mod;

    hash_chain mychain(mod);

    bool loop = true;
    while (loop) {
        std::cout << std::endl;
        std::cout << "PLEASE CHOOSE -" << std::endl;
        std::cout << "1. Add element." << std::endl;
        std::cout << "2. Find element." << std::endl;
        std::cout << "3. Generate Hash." << std::endl;
        std::cout << "4. Display Hash table." << std::endl;
        std::cout << "5. Exit." << std::endl;
        if (!(std::cin >> c)) {
            break;
        }
        switch (c) {
            case 1:
                std::cout << "Enter element to add = " << std::endl;
                std::cin >> x;
                h = mychain.hash(x);
                mychain.add(x, h);
                break;
            case 2:
                std::cout << "Enter element to search = " << std::endl;
                std::cin >> x;
                h = mychain.hash(x);
                mychain.find(x, h);
                break;
            case 3:
                std::cout << "Enter element to generate hash = " << std::endl;
                std::cin >> x;
                std::cout << "Hash of " << x << " is = " << mychain.hash(x)
                          << std::endl;
                break;
            case 4:
                mychain.display();
                break;
            default:
                loop = false;
                break;
        }
        std::cout << std::endl;
    }
    return 0;
}
