/**
 * @file
 * @brief Singly Linked List implementation using std::shared_ptr (基于 std::shared_ptr 的单链表实现)
 * @details
 * 单链表是一种通过节点指针连接的线性数据结构。支持添加、删除、搜索和打印。
 * 插入：在链表首部或尾部增加新节点。
 * 删除：遍历链表找到目标，将前驱节点的 next 指针指向当前节点的 next 指针，从而释放当前节点。
 */
#include <iostream>
#include <memory>
#include <string>

/**
 * @namespace data_structures
 * @brief 数据结构算法命名空间
 */
namespace data_structures {

/**
 * @namespace linked_list
 * @brief 单链表算法相关函数及类定义
 */
namespace linked_list {

/**
 * @brief 检查传入的字符串是否仅由数字字符组成
 * @param s 待检查的字符串
 * @return `true` 代表字符串只包含数字；`false` 否则
 */
bool isDigit(const std::string& s) {
    for (char i : s) {
        if (!isdigit(i)) {
            return false;
        }
    }
    return true;
}

/**
 * @brief 链表节点类 (Link)
 */
class link {
 private:
    int pvalue;                   ///< 节点存储的整数值
    std::shared_ptr<link> psucc;  ///< 指向下一个节点 (successor) 的共享智能指针

 public:
    /**
     * @brief 获取节点存储的值
     * @returns 整数值
     */
    int val() { return pvalue; }

    /**
     * @brief 获取下一个节点指针的引用
     * @returns 共享智能指针引用
     */
    std::shared_ptr<link>& succ() { return psucc; }

    /**
     * @brief 构造函数
     * @param value 节点保存的值，默认为 0
     */
    explicit link(int value = 0) : pvalue(value), psucc(nullptr) {}
};

/**
 * @brief 链表管理类 (List)
 */
class list {
 private:
    std::shared_ptr<link> first;  ///< 虚拟头节点 (Dummy Head)，不存储实际元素，其 succ() 指向第一个实际元素
    std::shared_ptr<link> last;   ///< 指向链表最后一个节点的指针
 public:
    /**
     * @brief 构造函数，初始化虚拟头节点
     */
    list() {
        first = std::make_shared<link>(); // 创建一个虚拟头节点
        last = nullptr;
    }

    bool isEmpty();                  ///< 检查链表是否为空
    void push_back(int new_elem);    ///< 尾插法插入元素
    void push_front(int new_elem);   ///< 头插法插入元素
    void erase(int old_elem);        ///< 删除指定值元素
    void display();                  ///< 显示链表所有元素
    std::shared_ptr<link> search(int find_elem); ///< 搜索指定值节点
    void reverse();                  ///< 反转链表（声明未实现）
};

/**
 * @brief 判断链表是否为空
 * @return `true` 为空；`false` 否则
 */
bool list::isEmpty() {
    if (last == nullptr) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief 在链表尾部追加元素
 * @param new_elem 待插入值
 */
void list::push_back(int new_elem) {
    if (isEmpty()) {
        // 如果链表为空，新建节点作为第一个实际节点，并让首尾指针指向它
        first->succ() = std::make_shared<link>(new_elem);
        last = first->succ();
    } else {
        // 挂载到 last 之后，并移动 last 标志
        last->succ() = std::make_shared<link>(new_elem);
        last = last->succ();
    }
}

/**
 * @brief 在链表头部插入元素
 * @param new_elem 待插入值
 */
void list::push_front(int new_elem) {
    if (isEmpty()) {
        first->succ() = std::make_shared<link>(new_elem);
        last = first->succ();
    } else {
        // 新建节点，使其 succ() 指向原第一个实际节点，然后再将虚拟头节点的 succ() 指向该新节点
        std::shared_ptr<link> t = std::make_shared<link>(new_elem);
        t->succ() = first->succ();
        first->succ() = t;
    }
}

/**
 * @brief 从链表中删除第一个匹配的旧元素值
 * @param old_elem 待删除的元素值
 */
void list::erase(int old_elem) {
    if (isEmpty()) {
        std::cout << "List is Empty!";
        return;
    }
    std::shared_ptr<link> t = first;
    std::shared_ptr<link> to_be_removed = nullptr;
    // 遍历寻找值为 old_elem 节点的父节点（前驱节点）
    while (t != last && t->succ()->val() != old_elem) {
        t = t->succ();
    }
    if (t == last) {
        std::cout << "Element not found\n";
        return;
    }
    to_be_removed = t->succ();
    t->succ() = t->succ()->succ(); // 从链表断开目标节点
    to_be_removed.reset();         // 释放共享指针计数引用
    
    // 如果删除的是最后一个节点，则需要更新 last 指针为前驱节点 t
    if (t->succ() == nullptr) {
        last = t;
    }
    // 如果链表删除完之后首尾重合（空链表状态），将 last 置空
    if (first == last){
        last = nullptr;
    }
}

/**
 * @brief 遍历打印链表所有元素
 */
void list::display() {
    if (isEmpty()) {
        std::cout << "List is Empty!";
        return;
    }
    std::shared_ptr<link> t = first;
    while (t->succ() != nullptr) {
        std::cout << t->succ()->val() << "\t";
        t = t->succ();
    }
}

/**
 * @brief 在链表中搜索指定值的节点
 * @param find_elem 待查找的键值
 * @return 匹配的节点指针，未找到则返回 nullptr
 */
std::shared_ptr<link> list::search(int find_elem) {
    if (isEmpty()) {
        std::cout << "List is Empty!";
        return nullptr;
    }
    std::shared_ptr<link> t = first;
    while (t != last && t->succ()->val() != find_elem) {
        t = t->succ();
    }
    if (t == last) {
        std::cout << "Element not found\n";
        return nullptr;
    }
    std::cout << "Element was found\n";
    return t->succ();
}
}  // namespace linked_list
}  // namespace data_structures

/**
 * @brief 主函数，提供交互式的链表操作菜单
 * @returns 0
 */
int main() {
    data_structures::linked_list::list l;
    int choice = 0;
    int x = 0;
    std::string s;
    do {
        std::cout << "\n1. Insert";
        std::cout << "\n2. Delete";
        std::cout << "\n3. Search";
        std::cout << "\n4. Print";
        std::cout << "\n0. Exit";
        std::cout << "\n\nEnter you choice : ";
        std::cin >> choice;
        switch (choice) {
            case 0:
                std::cout << "\nQuitting the program...\n";
                break;
            case 1:
                std::cout << "\nEnter the element to be inserted : ";
                std::cin >> s;

                if (data_structures::linked_list::isDigit(s)) {
                    x = std::stoi(s);
                    l.push_back(x);
                } else {
                    std::cout << "Wrong Input!\n";
                }
                break;
            case 2:
                std::cout << "\nEnter the element to be removed : ";
                std::cin >> s;
                if (data_structures::linked_list::isDigit(s)) {
                    x = std::stoi(s);
                    l.erase(x);
                } else {
                    std::cout << "Wrong Input!\n";
                }
                break;
            case 3:
                std::cout << "\nEnter the element to be searched : ";
                std::cin >> s;
                if (data_structures::linked_list::isDigit(s)) {
                    x = std::stoi(s);
                    std::shared_ptr<data_structures::linked_list::link> found =
                        l.search(x);
                } else {
                    std::cout << "Wrong Input!\n";
                }
                break;
            case 4:
                l.display();
                std::cout << "\n";
                break;
            default:
                std::cout << "Invalid Input\n" << std::endl;
                break;
        }
    } while (choice != 0);
    return 0;
}
