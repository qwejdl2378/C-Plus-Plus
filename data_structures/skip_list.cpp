/**
 * @file skip_list.cpp
 * @brief Data structure for fast searching and insertion in \f$O(\log n)\f$ time (跳表数据结构实现)
 * @details
 * 跳表（Skip List）是一种可以用来代替平衡树的数据结构。
 * 它通过维护一个多层级的链表结构，使得查询、插入和删除操作均可以在期望 O(log n) 的时间复杂度内完成。
 * 每一层链表都是下一层链表的子集，越往上层越稀疏。
 *
 * References used: [GeeksForGeek](https://www.geeksforgeeks.org/skip-list/),
 * [OpenGenus](https://iq.opengenus.org/skip-list) for PseudoCode and Code
 * @author [enqidu](https://github.com/enqidu)
 * @author [Krishna Vedala](https://github.com/kvedala)
 */

#include <array>
#include <cstring>
#include <ctime>
#include <iostream>
#include <memory>
#include <vector>

/** \namespace data_structures
 * \brief 数据结构算法命名空间
 */
namespace data_structures {
constexpr int MAX_LEVEL = 2;        ///< 跳表的最大层数限制
constexpr float PROBABILITY = 0.5;  ///< 晋升到更高层级的随机概率（相当于抛硬币）

/**
 * @brief 跳表的节点结构体
 */
struct Node {
    int key;      ///< 节点的键值（用于排序和比对）
    void* value;  ///< 节点存储的指针数据，可以是任何类型

    // forward 数组记录该节点在每一层指向的下一个节点。
    // forward[i] 代表在第 i 层中，当前节点的下一个节点指针。
    std::vector<std::shared_ptr<Node>> forward;

    /**
     * @brief 节点构造函数
     * @param key 比对用的键值
     * @param level 当前节点拥有的最高层级，决定了 forward 数组的大小
     * @param value 附加的值指针
     */
    Node(int key, int level, void* value = nullptr) : key(key), value(value) {
        // 初始化 forward 数组，填充为空指针
        for (int i = 0; i < (level + 1); i++) {
            forward.push_back(nullptr);
        }
    }
};

/**
 * @brief 包含基本方法的跳表类定义
 */
class SkipList {
    int level;                     ///< 当前跳表实际达到的最大层级
    std::shared_ptr<Node> header;  ///< 指向跳表头节点的指针（哨兵节点）

 public:
    /**
     * @brief 构造函数，初始化哨兵头节点并设置初始层级为 0
     */
    SkipList() {
        level = 0;
        // 头节点的 key 初始化为 -1，拥有满额的最大层数
        header = std::make_shared<Node>(-1, MAX_LEVEL);
    }

    /**
     * @brief 生成一个随机的节点层级。
     * @details 根据抛硬币概率 PROBABILITY，每高一层的概率都是前一层的一半。
     * @return 节点应当被分配的随机层级
     */
    int randomLevel() {
        int lvl = 0;
        while (static_cast<float>(std::rand()) / RAND_MAX < PROBABILITY &&
               lvl < MAX_LEVEL) {
            lvl++;
        }
        return lvl;
    }

    /**
     * @brief 插入一个含有键值 key 和指针 value 的新节点
     * @details 自上而下搜索目标插入位置，利用 update 数组记录每一层中新节点的前驱节点，
     * 然后创建新节点，并随机确定其层数，最后修改前后指针完成插入。
     *
     * @param key 用于排序比对的键值
     * @param value 伴随的数值指针
     */
    void insertElement(int key, void* value) {
        std::cout << "Inserting " << key << "...";
        std::shared_ptr<Node> x = header;
        
        // update 数组用来保存每一层中最后一个键值小于 key 的节点（即插入位置的前驱节点）
        std::array<std::shared_ptr<Node>, MAX_LEVEL + 1> update;
        update.fill(nullptr);

        // 自上而下在每一层查找前驱节点并记录
        for (int i = level; i >= 0; i--) {
            while (x->forward[i] != nullptr && x->forward[i]->key < key) {
                x = x->forward[i];
            }
            update[i] = x;
        }

        // x 移动到第 0 层的下一个节点，这就是可能重复的冲突节点位置
        x = x->forward[0];

        // 如果节点不存在，则开始执行插入操作
        bool doesnt_exist = (x == nullptr || x->key != key);
        if (doesnt_exist) {
            int rlevel = randomLevel(); // 生成随机层数

            // 如果生成的随机层数大于当前跳表的最大层数，需要将高出部分的 update 记录更新指向哨兵头节点
            if (rlevel > level) {
                for (int i = level + 1; i < rlevel + 1; i++) update[i] = header;

                // 更新当前跳表的最大层数
                level = rlevel;
            }

            // 创建新节点
            std::shared_ptr<Node> n =
                std::make_shared<Node>(key, rlevel, value);
            
            // 逐层将新节点链入链表中
            for (int i = 0; i <= rlevel; i++) {
                n->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = n;
            }
            std::cout << "Inserted" << std::endl;

        } else {
            std::cout << "Exists" << std::endl; // 键值已存在，跳过插入
        }
    }

    /**
     * @brief 根据键值删除节点，并打印是否删除成功
     * @param key 待删除的键值
     */
    void deleteElement(int key) {
        std::shared_ptr<Node> x = header;

        // update 数组用于记录每一层的前驱节点
        std::array<std::shared_ptr<Node>, MAX_LEVEL + 1> update;
        update.fill(nullptr);

        // 自上而下寻找每一层的前驱节点并记录
        for (int i = level; i >= 0; i--) {
            while (x->forward[i] != nullptr && x->forward[i]->key < key) {
                x = x->forward[i];
            }
            update[i] = x;
        }

        x = x->forward[0]; // 移动到第 0 层被删除节点处

        bool doesnt_exist = (x == nullptr || x->key != key);

        if (!doesnt_exist) {
            // 逐层修改前驱节点的指针，将节点 x 从各层链表中断开
            for (int i = 0; i <= level; i++) {
                if (update[i]->forward[i] != x) {
                    break;
                }
                update[i]->forward[i] = x->forward[i];
            }
            
            // 自动缩减因为节点被删除而变空出来的顶部层级
            while (level > 0 && header->forward[level] == nullptr) level--;
            std::cout << "Deleted" << std::endl;
        } else {
            std::cout << "Doesn't exist" << std::endl;
        }
    }

    /**
     * @brief 在跳表中查找指定键值的节点值指针
     * @param key 待查找的键值
     * @return 节点存储的原始数据指针，未找到则返回 nullptr
     */
    void* searchElement(int key) {
        std::shared_ptr<Node> x = header;
        std::cout << "Searching for " << key << std::endl;

        // 从高层往底层快速跳跃搜寻
        for (int i = level; i >= 0; i--) {
            while (x->forward[i] && x->forward[i]->key < key) x = x->forward[i];
        }

        x = x->forward[0]; // 移至第 0 层目标位置
        if (x && x->key == key) {
            std::cout << "Found" << std::endl;
            return x->value;
        } else {
            std::cout << "Not Found" << std::endl;
            return nullptr;
        }
    }

    /**
     * @brief 逐层打印跳表的层级分布状态
     */
    void displayList() {
        std::cout << "Displaying list:\n";
        for (int i = 0; i <= level; i++) {
            std::shared_ptr<Node> node = header->forward[i];
            std::cout << "Level " << (i) << ": ";
            while (node != nullptr) {
                std::cout << node->key << " ";
                node = node->forward[i];
            }
            std::cout << std::endl;
        }
    }
};

}  // namespace data_structures

/**
 * @brief 主函数，初始化随机种子，生成并向跳表插入元素，最后可视化输出
 * @returns 0
 */
int main() {
    std::srand(std::time(nullptr));

    data_structures::SkipList lst;

    // 随机向跳表中插入若干个数字并进行展示
    for (int j = 0; j < (1 << (data_structures::MAX_LEVEL + 1)); j++) {
        int k = (std::rand() % (1 << (data_structures::MAX_LEVEL + 2)) + 1);
        lst.insertElement(k, &j);
    }

    lst.displayList();
    return 0;
}
