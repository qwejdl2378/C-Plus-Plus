#include "cll.h"
using namespace std;

/**
 * @brief 主函数，演示并测试循环单链表（CLL）的各项基本操作
 * @returns 0
 */
int main() {
    /* 测试循环单链表 (CLL) */
    cout << "----------- Test construct -----------" << endl;
    cll list1;
    list1.display(); // 测试空链表构造
    
    cout << "----------- Test insert front -----------" << endl;
    list1.insert_front(5);
    cout << "After insert 5 at front: " << endl;
    list1.display();
    cout << "After insert 10 3 7 at front: " << endl;
    list1.insert_front(10);
    list1.insert_front(3);
    list1.insert_front(7);
    list1.display(); // 打印链表，验证头插顺序
    
    cout << "----------- Test insert tail -----------" << endl;
    cout << "After insert 18 19 20 at tail: " << endl;
    list1.insert_tail(18);
    list1.insert_tail(19);
    list1.insert_tail(20);
    list1.display(); // 验证尾插顺序
    
    cout << "----------- Test find item -----------" << endl;
    // 验证查找元素功能
    if (list1.find_item(10))
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
    
    // 验证查找不存在的元素功能
    if (!list1.find_item(30))
        cout << "PASS" << endl;
    else
        cout << "FAIL" << endl;
        
    cout << "----------- Test * operator -----------" << endl;
    int value = *list1; // 通过重载解引用获取当前头节点的数据值
    cout << "Value at *list1: " << value << endl;
    
    cout << "----------- Test ++ operator -----------" << endl;
    list1.display();
    ++list1; // 执行前置递增重载，实际弹出了头节点
    cout << "After ++list1: " << endl;
    list1.display();

    return 0;
}
