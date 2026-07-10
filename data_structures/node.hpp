/**
 * @file
 * @brief Provides Node class and related utilities (提供节点类及相关的链表辅助工具函数)
 **/
#ifndef DATA_STRUCTURES_NODE_HPP_
#define DATA_STRUCTURES_NODE_HPP_

#include <iostream>  /// 用于 std::cout
#include <memory>    /// 用于 std::shared_ptr 智能指针
#include <vector>    /// 用于 std::vector 数组

/** 
 * @brief 链表节点结构体定义
 * \tparam ValueType 节点所存储的数据类型
 */
template <class ValueType>
struct Node {
    using value_type = ValueType;
    ValueType data = {};                        ///< 节点保存的值
    std::shared_ptr<Node<ValueType>> next = {}; ///< 指向下一个节点的共享智能指针
};

/**
 * @brief 泛型链表遍历函数
 * @tparam Node 节点类型
 * @tparam Action 要对每个节点执行的函数动作类型
 * @param inNode 起始遍历的节点指针
 * @param action 函数回调对象，每次访问节点时调用
 */
template <typename Node, typename Action>
void traverse(const Node* const inNode, const Action& action) {
    if (inNode) {
        action(*inNode); // 访问当前节点
        traverse(inNode->next.get(), action); // 递归遍历下一个节点
    }
}

/**
 * @brief 打印链表中的所有节点数据
 * @tparam Node 节点类型
 * @param inNode 起始遍历的节点指针
 */
template <typename Node>
void display_all(const Node* const inNode) {
    traverse(inNode,
             [](const Node& curNode) { std::cout << curNode.data << " "; });
}

/**
 * @brief 将链表中的所有元素全部存入 std::vector 数组中
 * @tparam Node 节点类型
 * @param inNode 起始节点指针
 * @param expected_size 预期的元素数量（用于提高 vector 开辟空间的性能）
 * @return 包含所有节点数值的 std::vector
 */
template <typename Node>
std::vector<typename Node::value_type> push_all_to_vector(
    const Node* const inNode, const std::size_t expected_size = 0) {
    std::vector<typename Node::value_type> res;
    res.reserve(expected_size);
    traverse(inNode,
             [&res](const Node& curNode) { res.push_back(curNode.data); });
    return res;
}

#endif  // DATA_STRUCTURES_NODE_HPP_
