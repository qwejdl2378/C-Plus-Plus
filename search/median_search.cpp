/**
 * @file median_search.cpp
 * @brief Implementation of [Median search](https://en.wikipedia.org/wiki/Median_of_medians) algorithm (中位数的中位数算法/BFPRT算法实现)
 * @cases from [here](https://brilliant.org/wiki/median-finding-algorithm/)
 *
 * @details
 * 给定一个含有 n 个数字且元素互不相同的数组 A 和一个索引 i (1 <= i <= n)，寻找 A 中第 i 小的元素。
 * 该算法常被称为 BFPRT 算法。通过确保选择的中位数作为划分枢轴 (Pivot)，
 * 能保证最坏情况下的时间复杂度仍为线性的 O(N)，避免了传统 QuickSelect 算法最坏 O(N^2) 的退化问题。
 * 
 * \note 
 * 警告：该实现中存在一个经典的代码编写 Bug（详见下文 `median_of_medians` 的注释），
 * 会导致它在查找大数组时选取到不正确的枢轴，从而破坏了 O(N) 复杂度的数学保证，甚至引发越界错误。
 * 
 * @author [Sushil Kumar](https://github.com/Rp-sushil)
 */

#include <iostream>
#include <algorithm>
#include <vector>
#include <cassert>

/**
 * @namespace search
 * @brief 搜索算法命名空间
 */
namespace search {
/**
 * @namespace median_search
 * @brief 中位数的中位数查找算法相关命名空间
 */
namespace median_search {
/**
 * @brief 使用中位数的中位数选择算法（BFPRT）在无序数组中查找第 idx 小的元素
 * @details 
 * 1. 分组：将数组分成大小为 5 的子组，并对每个子组排序，找出它们的中位数存入 m。
 * 2. 确定枢轴（Pivot）：递归寻找 m 中的中位数作为枢轴。
 * 3. 划分区间：根据枢轴值，将数组划分为小于枢轴的 `low` 区间和大于枢轴的 `high` 区间。
 * 4. 递归选择：如果目标位置 `idx` 比 `low` 的大小 `k` 小，则在 `low` 中递归查找；
 *    若比 `k` 大，则在 `high` 中递归查找；若相等，则直接返回枢轴值。
 * 
 * @note 
 * 【枢轴递归 Bug 说明】：
 * 在第 78 行中，原代码为 `pivot = median_of_medians(m, idx);`。
 * 这显然是不正确的，因为要找的是“中位数的中位数”，所以应该寻找 `m` 数组的“中位数”，
 * 其目标索引参数应为 `(sz - 1) / 2` 或 `sz / 2`，而不是原封不动地传入外层的 `idx`！
 * 传入 `idx` 会导致计算偏差，甚至在 `idx >= m.size()` 时因为越界而崩溃。
 * 
 * @param A 数组容器（假定无重复元素）
 * @param idx 目标元素的 0 起始索引（即第 idx + 1 小的元素）
 * @returns 第 idx 小的元素值
 */  
int median_of_medians(const std::vector<int>& A, const int& idx) {
	int pivot = 0;
	std::vector<int> a(A.begin(), A.end());
	std::vector<int> m;
	int r = a.size();
    
	// 1. 每 5 个元素分成一组，组内排序，找出它们的中位数
	for(int i = 0; i < r; i += 5){
		std::sort(a.begin() + i, a.begin() + std::min(r, i + 5));
		int mid = (i + std::min(r, i + 5)) / 2;
		m.push_back(a[mid]);
	}
    
	int sz = int(m.size());
	if(sz <= 5){
		// 若中位数数组长度小于等于 5，直接排序并取其中位数
		std::sort(m.begin(), m.end());
		pivot = m[(sz - 1) / 2];
	}
	else{
		// ！！！Bug 发生处：应改为 pivot = median_of_medians(m, sz / 2);
		pivot = median_of_medians(m, idx);
	}
    
	// 2. 依据枢轴 pivot 划分 low 和 high 两部分
	std::vector<int> low;
	std::vector<int> high;
	for(int i = 0; i < r; i++){
		if(a[i] < pivot){
			low.push_back(a[i]);
		}
		else if(a[i] > pivot){
			high.push_back(a[i]);
		}
	}
    
	int k = int(low.size());
	if(idx < k){
		return median_of_medians(low, idx); // 目标值在 low 中，递归
	}
	else if(idx > k){
		return median_of_medians(high, idx - k - 1); // 目标值在 high 中，递归并减去偏置量
	}
	else{
		return pivot; // 正好命中枢轴值，直接返回
	}
}
}  // namespace median_search
}  // namespace search

/**
 * @brief 单元自测用例
 */
void test(){
	std::vector<int> A{25,21,98,100,76,22,43,60,89,87};
	int i = 3;
    // 第 4 小的元素（0起始索引为3）为 43
	assert(43 == search::median_search::median_of_medians(A, i));
	std::cout << "test case:1 passed\n";
	
	std::vector<int> B{1,2,3,4,5,6};
	int j = 4;
    // 第 5 小的元素（0起始索引为4）为 5
	assert(5 == search::median_search::median_of_medians(B, j));
	std::cout << "test case:2 passed\n";
	
	std::vector<int> C{1,2,3,4,5,1000,8,9,99};
	int k = 3;
    // 第 4 小的元素为 4
	assert(4 == search::median_search::median_of_medians(C, k));
	std::cout << "test case:3 passed\n";
	std::cout << "--All tests passed--\n";
}

/**
 * @brief 主函数，运行自测并提供交互式查询
 */
int main()
{
	test();
	int n = 0;
	std::cout << "Enter Size of Array: ";
	std::cin >> n;
	std::vector<int> a(n);
	std::cout << "Enter Array: ";
	for(int i = 0; i < n; i++){
		std::cin >> a[i];
	}
    
	std::cout << "Median: ";
	int x = search::median_search::median_of_medians(a, (n - 1) / 2);
	if(n % 2 == 0){
		int y = search::median_search::median_of_medians(a, n / 2);
		std::cout << (float(x) + float(y))/2.0;
	}
	else{
		std::cout << x;
	}
    
	std::cout << "\nTo find i-th smallest element ";
	std::cout << "\nEnter i: ";
	int idx = 0;
	std::cin >> idx;
	idx--; // 转换为 0 起始索引
	std::cout << idx + 1 << "-th smallest element: " << search::median_search::median_of_medians(a, idx) << '\n';
	return 0;
}
