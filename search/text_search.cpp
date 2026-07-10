/**
 * \file
 * \brief Search for words in a long textual paragraph (在长文本段落中检索单词)
 */
#include <cassert>
#include <cstdlib>
#include <iostream>
#ifdef _MSC_VER
#include <string>  // MS Visual C++ 编译兼容
#else
#include <cstring>
#endif

/**
 * @brief 将字符串中的所有大写英文字母转换为小写字母
 * @param word 输入的 std::string 字符串
 * @returns 转换成全小写的新字符串
 */
std::string lower(std::string word) {
    int length = word.length();
    std::string lc = "";

    for (int i = 0; i < length; i++) {
        lc += tolower(word[i]); // 逐字符转为小写
    }

    return lc;
}

/**
 * @brief 单元自测用例
 */
static void test() {
    assert(lower("abcd").compare("abcd") == 0);
    assert(lower("abc").compare("abcd") == -1);
    assert(lower("abcd").compare("abc") == 1);
}

/**
 * @brief 主函数
 * @details 
 * 提供一个交互式的文本检索控制台。用户输入一段落，随后可以通过输入目标词进行检索。
 * 支持“大小写敏感”或“大小写不敏感”两种检索模式，底层使用 `std::string::find` 进行定位。
 * 
 * \note
 * 提示：由于在第 62 行使用了 `std::cin >> ch;` 读取整数，这会在输入缓冲区遗留下一个换行符 `\n`，
 * 导致下一次循环在 `std::getline(std::cin, word)` 时会读入一个空字符串。建议在实际使用中清除输入流缓冲区。
 * 
 * @returns 0
 */
int main() {
    test();  // 运行自测
    std::string paragraph;
    std::cout << "Please enter your paragraph: \n";
    std::getline(std::cin, paragraph); // 读入整个段落文本
    std::cout << "\nHello, your paragraph is:\n " << paragraph << "!\n";
    std::cout << "\nThe size of your paragraph = " << paragraph.size()
              << " characters. \n\n";

    if (paragraph.empty()) {
        std::cout << "\nThe paragraph is empty" << std::endl;
    } else {
        int ch = 0;
        while (true) {
            std::string word;
            std::cout << "Please enter the word you are searching for: ";
            std::getline(std::cin, word);
            std::cout << "Ignore case-sensitive? 1 = Yes, 0 = No" << std::endl;
            std::cin >> ch;
            if (ch == 1) {
                // 模式一：大小写不敏感（统一转为小写后查找）
                std::string lowerCase = lower(paragraph);
                std::string lowerCaseWord = lower(word);

                std::cout << "Hello, your word is " << word << "!\n";
                if (lowerCase.find(lowerCaseWord) == std::string::npos) {
                    std::cout << word << " does not exist in the sentence" << std::endl;
                } else {
                    std::cout << "The word " << word
                              << " is now found at location "
                              << lowerCase.find(lowerCaseWord) << std::endl
                              << std::endl;
                }
            } else {
                // 模式二：大小写敏感（直接查找）
                std::cout << "Hello, your word is " << word << "!\n";
                if (paragraph.find(word) == std::string::npos) {
                    std::cout << word << " does not exist in the sentence" << std::endl;
                } else {
                    std::cout << "The word " << word
                              << " is now found at location "
                              << paragraph.find(word) << std::endl
                              << std::endl;
                }
            }
            std::cout << "\nPress Ctrl + C to exit the program.\n\n";
            std::cin.get(); // 吞掉缓冲区换行符
        }
    }
    return 0;
}
