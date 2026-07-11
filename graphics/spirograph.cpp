/**
 * @file
 * @author [Krishna Vedala](https://github.com/kvedala)
 * @brief Implementation of [Spirograph](https://en.wikipedia.org/wiki/Spirograph) (内摆线与外摆线螺旋绘图算法实现)
 *
 * @details
 * 螺旋作图仪（Spirograph）是一种几何画图玩具，可以绘制称为内摆线（Hypotrochoid）和外摆线（Epitrochoid）的数学曲线。
 *
 * ### 螺旋曲线数学公式
 * 本实现中采用如下笛卡尔坐标公式生成点：
 *   x = R * [ (1 - k) * cos(t) + l * k * cos((1 - k) * t / k) ]
 *   y = R * [ (1 - k) * sin(t) - l * k * sin((1 - k) * t / k) ]
 * 其中：
 * 1. R 是外圆（大圆）的半径，本程序中缩放常量 R = 1.0。
 * 2. k = r / R 是内圆（小圆）半径与外圆半径的比例值，满足 0 < k < 1。
 * 3. l = ρ / r 是画笔（marker）到内圆圆心的相对距离比例，满足 0 <= l <= 1。
 * 4. t 是旋转角弧度，代表时间变化参数。
 */

#ifdef USE_GLUT
#ifdef __APPLE__
#include <GLUT/glut.h>  // macOS 环境下的 GLUT 路径
#else
#include <GL/glut.h>    // Linux/Windows 环境下的 GLUT 路径
#endif  // __APPLE__
#endif

#define _USE_MATH_DEFINES /**< MSVC 编译器所需的数学常量定义宏 */
#include <array>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#ifdef _OPENMP
#include <omp.h>        // 支持 OpenMP 多线程并行加速计算
#endif

/**
 * spirograph 命名空间
 */
namespace spirograph {
/**
 * @brief 生成螺旋线曲线点阵
 * @tparam N 点的数量（数组大小）
 * @param[out] points 存储二维坐标点的双精度 std::pair 数组指针
 * @param l 画笔相对内圆心的距离比率值 (0 <= l <= 1)
 * @param k 内圆与外圆半径比率值 (0 < k < 1)
 * @param rot 旋转模拟的总圈数（可以是浮点小数）
 */
template <std::size_t N>
void spirograph(std::array<std::pair<double, double>, N> *points, double l,
                double k, double rot) {
    double dt = rot * 2.f * M_PI / N; // 每次采样角度增量
    double R = 1.f;
    const double k1 = 1.f - k;
    int32_t step = 0;

#ifdef _OPENMP
#pragma omp for // 如果开启 OpenMP，则多线程并行计算各个点坐标
#endif
    for (step = 0; step < N; step++) {
        double t = dt * step;
        double first = R * (k1 * std::cos(t) + l * k * std::cos(k1 * t / k));
        double second = R * (k1 * std::sin(t) - l * k * std::sin(k1 * t / k));
        points[0][step].first = first;
        points[0][step].second = second;
    }
}

/**
 * @brief 单元自测函数：生成点阵数据并将其保存到 CSV 文件中
 * @note
 * 【Bug 警示说明】：
 * 在第 110 行：`fp << points[i].first << "," << points[i].first;`
 * 作者在输出坐标点时将 `first`（X 坐标）写入了两次，导致 CSV 中输出的 Y 轴数据实际上也是 X 轴的值。
 * 若要得到正确的螺旋图像数据，应当修正为：`points[i].first << "," << points[i].second`。
 */
void test() {
    const size_t N = 500;
    double l = 0.3, k = 0.75, rot = 10.;
    std::stringstream fname;
    fname << std::setw(3) << "spirograph_" << l << "_" << k << "_" << rot
          << ".csv";
    std::ofstream fp(fname.str());
    if (!fp.is_open()) {
        perror(fname.str().c_str());
        exit(EXIT_FAILURE);
    }

    std::array<std::pair<double, double>, N> points;

    spirograph(&points, l, k, rot);

    for (size_t i = 0; i < N; i++) {
        fp << points[i].first << "," << points[i].first; // 存在输出双 X 坐标的 bug
        if (i < N - 1) {
            fp << '\n';
        }
    }

    fp.close();
}

#ifdef USE_GLUT
static bool paused = 0; /**< 标记是否暂停动画 */
static const int animation_speed = 25; /**< 动画间隔延迟（毫秒） */

static const double step = 0.01;   /**< 动画单步增量 */
static double l_ratio = step * 10; /**< l-ratio 参数 */
static double k_ratio = step;      /**< k-ratio 参数 */
static const double num_rot = 20.; /**< 旋转圈数 */

/**
 * @brief 自定义位图字符串渲染函数（GLUT 兼容包装）
 */
static inline void glutBitmapString(void *font, char *message) {
    for (char *ch = message; *ch != '\0'; ch++) glutBitmapCharacter(font, *ch);
}

/**
 * @brief 绘制螺旋曲线和文字信息到 OpenGL 图形窗口上
 */
template <size_t N>
void display_graph(const std::array<std::pair<double, double>, N> &points,
                   double l, double k) {
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // 背景涂白
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_LINES);         // 绘制直线段
    glColor3f(0.f, 0.f, 1.f);  // 蓝色线段
    glPointSize(2.f);

    for (size_t i = 1; i < N; i++) {
        glVertex2f(points[i - 1].first, points[i - 1].second);
        glVertex2f(points[i].first, points[i].second);
    }
    glEnd();

    // 绘制当前 l 和 k 参数到屏幕左上角
    glColor3f(0.f, 0.f, 0.f);
    std::stringstream buffer;
    buffer << std::setw(3) << "l = " << l;
    glRasterPos2f(-.85, .85);
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,
                     const_cast<char *>(buffer.str().c_str()));
    buffer.str("");
    buffer.clear();
    buffer << std::setw(3) << "k = " << k;
    glRasterPos2f(-.85, .70);
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,
                     const_cast<char *>(buffer.str().c_str()));

    glutSwapBuffers();
}

/**
 * @brief 含有动画演变展示的测试绘制函数
 */
void test2() {
    const size_t N = 5000;

    static bool direction1 = true;
    static bool direction2 = true;

    std::array<std::pair<double, double>, N> points;

    spirograph(&points, l_ratio, k_ratio, num_rot);
    display_graph(points, l_ratio, k_ratio);

    if (paused)
        return; // 暂停状态，直接返回不更新比率值

    // 动态调整 k_ratio 和 l_ratio 的值以产生变幻动画效果
    if (direction1) {
        if (k_ratio >= (1.f - step))
            direction1 = false;
        else
            k_ratio += step;
    } else {
        if (k_ratio <= step) {
            direction1 = true;

            if (direction2) {
                if (l_ratio >= (1.f - step))
                    direction2 = false;
                else
                    l_ratio += step;
            } else {
                if (l_ratio <= step)
                    direction2 = true;
                else
                    l_ratio -= step;
            }
        } else {
            k_ratio -= step;
        }
    }
}

/**
 * @brief GLUT 定时器回调函数，用以维持稳定的帧率延时
 */
void timer_cb(int t) {
    glutTimerFunc(animation_speed, timer_cb, 0);
    glutPostRedisplay();
}

/**
 * @brief 键盘按键事件响应回调函数
 */
void keyboard_cb(unsigned char key, int x, int y) {
    switch (key) {
        case ' ': // 空格键：切换暂停/播放
            paused = !paused;
            break;
        case GLUT_KEY_UP:
        case '+': // 增加 k_ratio 值
            k_ratio += step;
            break;
        case GLUT_KEY_DOWN:
        case '_': // 减少 k_ratio 值
            k_ratio -= step;
            break;
        case GLUT_KEY_RIGHT:
        case '=': // 增加 l_ratio 值
            l_ratio += step;
            break;
        case GLUT_KEY_LEFT:
        case '-': // 减少 l_ratio 值
            l_ratio -= step;
            break;
        case 0x1B: // ESC 键退出程序
            exit(EXIT_SUCCESS);
        default:
            return;
    }
}
#endif
}  // namespace spirograph

/**
 * @brief 主函数
 */
int main(int argc, char **argv) {
    spirograph::test(); // 执行 CSV 输出保存自测

#ifdef USE_GLUT
    // 如果定义了 USE_GLUT，启动 OpenGL 视窗和主循环
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutCreateWindow("Spirograph");
    glutInitWindowSize(400, 400);
    glutTimerFunc(spirograph::animation_speed, spirograph::timer_cb, 0);
    glutKeyboardFunc(spirograph::keyboard_cb);
    glutDisplayFunc(spirograph::test2);
    glutMainLoop();
#endif

    return 0;
}
