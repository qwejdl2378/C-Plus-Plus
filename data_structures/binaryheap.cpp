/**
 * \file
 * \brief A C++ program to demonstrate common Binary Heap Operations (二叉最小堆的常见操作演示)
 */
#include <climits>
#include <iostream>
#include <utility>

/** 
 * @brief 最小堆（Min Heap）类定义
 */
class MinHeap {
    int *harr;      ///< 指向堆中元素数组的指针
    int capacity;   ///< 最小堆的最大物理容量
    int heap_size;  ///< 最小堆中当前的有效元素个数

 public:
    /** 
     * @brief 构造函数：初始化最小堆空间
     * \param[in] capacity 初始堆容量限制
     */
    explicit MinHeap(int cap) {
        heap_size = 0;
        capacity = cap;
        harr = new int[cap];
    }

    /** 
     * @brief 对以指定索引为根的子树进行下滤调整 (Heapify)
     */
    void MinHeapify(int);

    /**
     * @brief 获取当前节点 i 的父节点索引
     */
    int parent(int i) { return (i - 1) / 2; }

    /** 
     * @brief 获取当前节点 i 的左孩子节点索引
     */
    int left(int i) { return (2 * i + 1); }

    /** 
     * @brief 获取当前节点 i 的右孩子节点索引
     */
    int right(int i) { return (2 * i + 2); }

    /** 
     * @brief 弹出并返回堆顶的最小值节点 (Extract Min)
     */
    int extractMin();

    /** 
     * @brief 将索引 i 处的键值降低到更小的值 new_val
     */
    void decreaseKey(int i, int new_val);

    /** 
     * @brief 返回堆顶的最小值（不弹出）
     */
    int getMin() { return harr[0]; }

    /** 
     * @brief 删除堆中指定索引 i 处的键值
     */
    void deleteKey(int i);

    /** 
     * @brief 向最小堆中插入一个新键值 'k'
     */
    void insertKey(int k);

    /**
     * @brief 析构函数，释放物理数组的动态内存
     */
    ~MinHeap() { delete[] harr; }
};

/**
 * @brief 插入新键值 k
 * @details 
 * 1. 首先将新元素 k 插入到堆数组的末尾。
 * 2. 从下往上（与父节点比较），若当前值比父节点小，则执行上滤 (up-heapify) 交换，直至满足最小堆性质。
 * @param k 待插入的值
 */
void MinHeap::insertKey(int k) {
    if (heap_size == capacity) {
        std::cout << "\nOverflow: Could not insertKey\n";
        return;
    }

    // 插入末尾
    heap_size++;
    int i = heap_size - 1;
    harr[i] = k;

    // 自底向上执行上滤交换
    while (i != 0 && harr[parent(i)] > harr[i]) {
        std::swap(harr[i], harr[parent(i)]);
        i = parent(i);
    }
}

/** 
 * @brief 将索引 i 处的键值减小至更小的 new_val。假定 new_val 必定小于当前的 harr[i]。
 * @param i 节点索引
 * @param new_val 新值
 */
void MinHeap::decreaseKey(int i, int new_val) {
    harr[i] = new_val;
    // 值变小了，可能违背堆性质，需要自底向上执行上滤
    while (i != 0 && harr[parent(i)] > harr[i]) {
        std::swap(harr[i], harr[parent(i)]);
        i = parent(i);
    }
}

/** 
 * @brief 弹出并返回堆顶的最小值
 * @details 
 * 1. 将堆尾的元素移动并覆盖到堆顶（harr[0]）。
 * 2. 堆元素总数减 1。
 * 3. 对新的堆顶（索引 0）进行自顶向下下滤 (MinHeapify) 调整，维护最小堆平衡。
 */
int MinHeap::extractMin() {
    if (heap_size <= 0)
        return INT_MAX;
    if (heap_size == 1) {
        heap_size--;
        return harr[0];
    }

    int root = harr[0];
    harr[0] = harr[heap_size - 1]; // 尾部元素移至堆顶
    heap_size--;
    MinHeapify(0); // 对堆顶执行下滤调整

    return root;
}

/** 
 * @brief 删除堆中指定索引 i 处的键值
 * @details 
 * 巧妙地将索引 i 处的值利用 decreaseKey 强行降低为负无穷大 (INT_MIN)，
 * 此时该值会自动上滤交换到堆顶，接着再调用 extractMin() 弹出即可。
 */
void MinHeap::deleteKey(int i) {
    decreaseKey(i, INT_MIN);
    extractMin();
}

/** 
 * @brief 递归实现的下滤调整（自顶向下 Heapify）
 * @details 比较当前节点 i 与其左右孩子，找出三者中的最小值。
 * 若最小值不是 i 自身，则与最小值节点进行交换，并递归下滤被交换的子树。
 * @param i 待下滤的节点索引
 */
void MinHeap::MinHeapify(int i) {
    int l = left(i);
    int r = right(i);
    int smallest = i;
    if (l < heap_size && harr[l] < harr[i])
        smallest = l;
    if (r < heap_size && harr[r] < harr[smallest])
        smallest = r;
    if (smallest != i) {
        std::swap(harr[i], harr[smallest]);
        MinHeapify(smallest); // 递归下滤
    }
}

/**
 * @brief 主函数测试最小堆的各项功能
 * @returns 0
 */
int main() {
    MinHeap h(11);
    h.insertKey(3);
    h.insertKey(2);
    h.deleteKey(1);
    h.insertKey(15);
    h.insertKey(5);
    h.insertKey(4);
    h.insertKey(45);
    std::cout << h.extractMin() << " ";  // 应当输出最小的元素值
    std::cout << h.getMin() << " ";      // 获取堆顶当前最小值
    h.decreaseKey(2, 1);
    std::cout << h.getMin();
    return 0;
}
