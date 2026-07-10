/*
 * This program reads a data file consisting of students' GPAs
 * followed by their names. The program then prints the highest
 * GPA and the names of the students with the highest GPA.
 * It uses stack to store the names of the students
 * Run:
 *     make all
 *     ./main student.txt
 * ************************************************************
 *
 * 该程序从文件中读取一组学生的 GPA 和对应的名字。
 * 随后，程序会计算并输出最高的 GPA 值，以及所有获得该最高 GPA 的学生姓名。
 * 它使用自定义的 `stack` (栈) 结构来存储并过滤这些获得最高成绩的学生名字。
 * */
#include <cassert>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "./stack.hpp"

/**
 * @brief 主函数，演示使用自定义泛型栈解决实际数据筛选问题
 * @param argc 命令行参数计数（要求为 2，即带一个数据文件名参数）
 * @param argv 命令行参数数组，其中 argv[1] 应为数据文件路径
 * @returns 0
 */
int main(int argc, char* argv[]) {
    double GPA = NAN;
    double highestGPA = NAN;
    std::string name;

    assert(argc == 2); // 确保传入了学生数据文件参数
    std::ifstream infile;
    stack<std::string> stk; // 实例化一个存储字符串类型的栈

    infile.open(argv[1]);
    std::cout << std::fixed << std::showpoint;
    std::cout << std::setprecision(2); // 浮点输出格式精度控制为两位小数
    infile >> GPA >> name;
    highestGPA = GPA;

    // 循环迭代读取文件中的每一条学生 GPA 与姓名数据
    while (infile) {
        if (GPA > highestGPA) {
            // 发现比当前记录还要高的 GPA：
            stk.clear();      // 清空栈内原有的所有学生姓名（因为他们都不符合新最高条件了）
            stk.push(name);   // 压入符合新最高 GPA 的当前学生名字
            highestGPA = GPA; // 更新最高 GPA 的值
        } else if (GPA == highestGPA) {
            // 发现与当前最高 GPA 相同的学生：
            stk.push(name);   // 并列最高，直接将名字也压入栈中保存
        }
        infile >> GPA >> name;
    }
    
    // 输出计算结果
    std::cout << "Highest GPA: " << highestGPA << std::endl;
    std::cout << "Students with the highest GPA are: " << std::endl;
    
    // 通过出栈操作，依次打印所有并列获得最高 GPA 的学生姓名（打印顺序与输入文件顺序相反，即后进先出）
    while (!stk.isEmptyStack()) {
        std::cout << stk.top() << std::endl;
        stk.pop();
    }
    std::cout << std::endl;
    return 0;
}
