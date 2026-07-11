/**
 * @file
 * @brief Ground to ground [projectile motion](https://en.wikipedia.org/wiki/Projectile_motion) equation implementations (地面斜抛运动方程实现)
 *
 * @details
 * 地面斜抛运动指弹射物在地面发射，到达顶点（Apex）后落回水平地面的抛体运动轨迹。
 * 在不计空气阻力的情况下，其水平方向为匀速直线运动，竖直方向为受到重力加速度影响的匀变速直线运动。
 *
 * ### 运动公式
 * 1. **飞行时间 (Time of Flight)**:
 *    $t = \frac{2 \cdot v_i \cdot \sin(\theta)}{g}$
 * 2. **水平射程 (Horizontal Range)**:
 *    $R = v_i \cdot \cos(\theta) \cdot t$
 * 3. **最大高度 (Maximum Height)**:
 *    $H = \frac{(v_i \cdot \sin(\theta))^2}{2g}$
 *
 * 时间复杂度: $O(1)$
 * 空间复杂度: $O(1)$
 *
 * @note
 * 【零除风险防卫性审计与修复】：
 * 1. **除零崩溃防御**：如果外部传入 `gravity = 0` 或负值，会导致飞行时间与最大高度计算发生除以零错误（除以 $2g$）
 *    从而产生 `NaN` 或正负无穷大溢出。
 *    **修复**：在 `time_of_flight` 与 `max_height` 中添加 `assert(gravity > 0.0)` 对重力加速度进行非零防卫。
 *
 * @author [Focusucof](https://github.com/Focusucof)
 */

#include <cassert>   
#define _USE_MATH_DEFINES
#include <cmath>     
#include <iostream>  

/** 标准重力加速度常量 (m/s^2) */
constexpr double GRAVITY = 9.80665; 

namespace physics {
namespace ground_to_ground_projectile_motion {

/**
 * @brief 角度转弧度计算
 * @param degrees 角度值 (0-360)
 * @return 弧度值
 */
inline double degrees_to_radians(double degrees) {
    return degrees * (M_PI / 180.0);
}

/**
 * @brief 计算抛体在空中的飞行时间
 * @tparam T 浮点数值类型 (float/double)
 * @param initial_velocity 初速度 (m/s)
 * @param angle 发射仰角 (度)
 * @param gravity 重力加速度
 * @return 飞行总时间 (s)
 */
template <typename T>
T time_of_flight(T initial_velocity, T angle, double gravity = GRAVITY) {
    // 核心修复：防止重力加速度为 0 导致除零异常
    assert(gravity > 0.0 && "Gravity must be strictly positive!");
    double Viy = initial_velocity * (std::sin(degrees_to_radians(angle))); 
    return static_cast<T>(2.0 * Viy / gravity);
}

/**
 * @brief 计算抛体的水平位移射程
 * @tparam T 浮点数值类型
 * @param initial_velocity 初速度 (m/s)
 * @param angle 发射仰角 (度)
 * @param time 飞行总时间 (s)
 * @return 水平射程 (m)
 */
template <typename T>
T horizontal_range(T initial_velocity, T angle, T time) {
    double Vix = initial_velocity * (std::cos(degrees_to_radians(angle))); 
    return static_cast<T>(Vix * time);
}

/**
 * @brief 计算抛体能够到达的最大高度
 * @tparam T 浮点数值类型
 * @param initial_velocity 初速度 (m/s)
 * @param angle 发射仰角 (度)
 * @param gravity 重力加速度
 * @return 最大高度 (m)
 */
template <typename T>
T max_height(T initial_velocity, T angle, double gravity = GRAVITY) {
    // 核心修复：防止重力加速度为 0 导致除零异常
    assert(gravity > 0.0 && "Gravity must be strictly positive!");
    double Viy = initial_velocity * (std::sin(degrees_to_radians(angle))); 
    return static_cast<T>(std::pow(Viy, 2) / (2.0 * gravity));
}

}  // namespace ground_to_ground_projectile_motion
}  // namespace physics

/**
 * @brief 单元自测用例
 */
static void test() {
    double initial_velocity = 5.0;  
    double angle = 40.0;            

    // 测试 1：验证飞行时间计算
    double expected_time_of_flight = 0.655;  
    double flight_time_output =
        std::round(physics::ground_to_ground_projectile_motion::time_of_flight(initial_velocity, angle) * 1000.0) /
        1000.0;  

    std::cout << "Projectile Flight Time (double)" << std::endl;
    std::cout << "Input Initial Velocity: " << initial_velocity << std::endl;
    std::cout << "Input Angle: " << angle << std::endl;
    std::cout << "Expected Output: " << expected_time_of_flight << std::endl;
    std::cout << "Output: " << flight_time_output << std::endl;
    assert(flight_time_output == expected_time_of_flight);
    std::cout << "TEST PASSED" << std::endl << std::endl;

    // 测试 2：验证水平范围计算
    double expected_horizontal_range = 2.51; 
    double horizontal_range_output =
        std::round(physics::ground_to_ground_projectile_motion::horizontal_range(initial_velocity, angle,
                                             flight_time_output) *
                   100.0) /
        100.0;  

    std::cout << "Projectile Horizontal Range (double)" << std::endl;
    std::cout << "Input Initial Velocity: " << initial_velocity << std::endl;
    std::cout << "Input Angle: " << angle << std::endl;
    std::cout << "Input Time Of Flight: " << flight_time_output << std::endl;
    std::cout << "Expected Output: " << expected_horizontal_range << std::endl;
    std::cout << "Output: " << horizontal_range_output << std::endl;
    assert(horizontal_range_output == expected_horizontal_range);
    std::cout << "TEST PASSED" << std::endl << std::endl;

    // 测试 3：验证最大高度计算
    double expected_max_height = 0.527; 
    double max_height_output =
        std::round(physics::ground_to_ground_projectile_motion::max_height(initial_velocity, angle) * 1000.0) /
        1000.0;  

    std::cout << "Projectile Max Height (double)" << std::endl;
    std::cout << "Input Initial Velocity: " << initial_velocity << std::endl;
    std::cout << "Input Angle: " << angle << std::endl;
    std::cout << "Expected Output: " << expected_max_height << std::endl;
    std::cout << "Output: " << max_height_output << std::endl;
    assert(max_height_output == expected_max_height);
    std::cout << "TEST PASSED" << std::endl << std::endl;
}

/**
 * @brief 主函数
 */
int main() {
    test();  // 运行测试用例确认正确性
    return 0;
}
