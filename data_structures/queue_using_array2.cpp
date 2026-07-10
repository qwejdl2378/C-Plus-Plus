#include <iostream>

int queue[10];      ///< 全局数组表示的队列，容量为 10
int front = 0;      ///< 队首索引指针
int rear = 0;       ///< 队尾（下一个插入位置）索引指针

/**
 * @brief 元素入队
 * @param x 待插入的元素值
 */
void Enque(int x) {
    if (rear == 10) {
        std::cout << "\nOverflow"; // 队满溢出
    } else {
        queue[rear++] = x;
    }
}

/**
 * @brief 元素出队
 * @details 该版本在出队后，为了防止空间浪费和指针右移漂移，会执行一次数组平移：
 * 将从 front 到 rear 的所有剩余元素左移到数组开头（0 起始），然后将 front 重置为 0，
 * 并重新计算 rear 的偏移量。
 */
void Deque() {
    if (front == rear) {
        std::cout << "\nUnderflow"; // 队空下溢
    } else {
        std::cout << "\n" << queue[front++] << " deleted";
        // 将剩余的数据元素向前移动，填补被删除的位置以节省物理空间
        for (int i = front; i < rear; i++) {
            queue[i - front] = queue[i];
        }
        rear = rear - front; // 更新队尾的下一个可用索引
        front = 0;           // 队首指针重置为 0
    }
}

/**
 * @brief 打印队列中当前的所有有效元素
 */
void show() {
    for (int i = front; i < rear; i++) {
        std::cout << queue[i] << "\t";
    }
}

/**
 * @brief 主函数，提供交互菜单
 * @returns 0
 */
int main() {
    int ch, x;
    do {
        std::cout << "\n1. Enque";
        std::cout << "\n2. Deque";
        std::cout << "\n3. Print"
                  << "\n0. Exit";
        std::cout << "\nEnter Your Choice : ";
        std::cin >> ch;
        if (ch == 1) {
            std::cout << "\nInsert : ";
            std::cin >> x;
            Enque(x);
        } else if (ch == 2) {
            Deque();
        } else if (ch == 3) {
            show();
        }
    } while (ch != 0);

    return 0;
}
