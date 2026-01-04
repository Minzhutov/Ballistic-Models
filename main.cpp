#include "Include/trajectory.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <iomanip>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

int main() {
    #ifdef _WIN32
        // 65001 – кодовая страница UTF‑8
        SetConsoleOutputCP(65001);
        SetConsoleCP(65001);
        // Для корректного вывода wide‑строк (если захотите)
        _setmode(_fileno(stdout), _O_U16TEXT);
    #endif
    
    // Создаем папку для результатов
    std::filesystem::create_directory("results");
    
    // Более реалистичные исходные данные для ракеты
    double V0 = 10.0;           // Начальная скорость, м/с
    double theta_c0 = 85.0;     // Начальный угол наклона траектории, град (почти вертикально)
    double m_dot = 20.0;        // Массовый секундный расход, кг/с
    double W = 2800.0;          // Скорость истечения газов ДУ, м/с
    double y0 = 0.0;            // Начальная высота, м
    double omega_z0 = 0.0;      // Начальная угловая скорость вращения, с^-1
    double theta0 = theta_c0;   // Начальный угол тангажа, град
    double t_end = 30.0;        // Продолжительность активного участка, с
    double m0 = 1255.0;         // Начальная масса, кг
    double I_d = 0.215;         // Запас устойчивости, м
    double S_a = 0.14;          // Площадь выходного сечения сопла, м²
    double S_m = 0.231;         // Характерная площадь ЛА, м²
    
    std::cout << "РАСЧЕТ ТРАЕКТОРИИ ЛЕТАТЕЛЬНОГО АППАРАТА НА АКТИВНОМ УЧАСТКЕ\n";
    std::cout << "==========================================================\n\n";
    
    std::cout << "Исходные данные:\n";
    std::cout << "V0 = " << V0 << " м/с\n";
    std::cout << "theta_c0 = " << theta_c0 << " град\n";
    std::cout << "m_dot = " << m_dot << " кг/с\n";
    std::cout << "W = " << W << " м/с\n";
    std::cout << "y0 = " << y0 << " м\n";
    std::cout << "omega_z0 = " << omega_z0 << " с^-1\n";
    std::cout << "theta0 = " << theta0 << " град\n";
    std::cout << "t_end = " << t_end << " с\n";
    std::cout << "m0 = " << m0 << " кг\n";
    std::cout << "I_d = " << I_d << " м\n";
    std::cout << "S_a = " << S_a << " м²\n";
    std::cout << "S_m = " << S_m << " м²\n\n";
    
    // Создаем калькулятор
    TrajectoryCalculator calculator(V0, theta_c0, m_dot, W, y0, omega_z0, theta0,
                                    t_end, m0, I_d, S_a, S_m);
    
    // Задание 1: Метод Эйлера
    std::cout << "\nЗАДАНИЕ 1: МЕТОД ЭЙЛЕРА\n";
    std::cout << "========================\n";
    
    double dt_values_euler[] = {0.1, 0.01, 0.001};
    for (double dt : dt_values_euler) {
        std::cout << "\n1a. α = θ - θс, dt = " << dt << " с\n";
        try {
            auto trajectory = calculator.calculateTrajectory(EULER, ALPHA_THETA_MINUS_THETAC, dt);
            calculator.printResultsTable(trajectory);
            
            std::string filename = "results/euler_alpha_theta_dt_" + 
                                  std::to_string(dt).substr(0,4);
            calculator.saveResultsToFile(trajectory, filename + ".txt");
            
            // Сохраняем данные для графиков
            calculator.saveGraphData(trajectory, filename + "_graph");
            
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }
        
        std::cout << "\n1b. α = 0, dt = " << dt << " с\n";
        try {
            auto trajectory = calculator.calculateTrajectory(EULER, ALPHA_ZERO, dt);
            calculator.printResultsTable(trajectory);
            
            std::string filename = "results/euler_alpha_zero_dt_" + 
                                  std::to_string(dt).substr(0,4);
            calculator.saveResultsToFile(trajectory, filename + ".txt");
            
            // Сохраняем данные для графиков
            calculator.saveGraphData(trajectory, filename + "_graph");
            
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }
    }
    
    // Задание 2: Модифицированный метод Эйлера
    std::cout << "\n\nЗАДАНИЕ 2: МОДИФИЦИРОВАННЫЙ МЕТОД ЭЙЛЕРА\n";
    std::cout << "==========================================\n";
    
    double dt_values_modified[] = {0.1, 0.01};
    for (double dt : dt_values_modified) {
        std::cout << "\n2a. α = θ - θс, dt = " << dt << " с\n";
        try {
            auto trajectory = calculator.calculateTrajectory(MODIFIED_EULER, ALPHA_THETA_MINUS_THETAC, dt);
            calculator.printResultsTable(trajectory);
            
            std::string filename = "results/modified_euler_alpha_theta_dt_" + 
                                  std::to_string(dt).substr(0,4);
            calculator.saveResultsToFile(trajectory, filename + ".txt");
            
            // Сохраняем данные для графиков
            calculator.saveGraphData(trajectory, filename + "_graph");
            
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }
        
        std::cout << "\n2b. α = 0, dt = " << dt << " с\n";
        try {
            auto trajectory = calculator.calculateTrajectory(MODIFIED_EULER, ALPHA_ZERO, dt);
            calculator.printResultsTable(trajectory);
            
            std::string filename = "results/modified_euler_alpha_zero_dt_" + 
                                  std::to_string(dt).substr(0,4);
            calculator.saveResultsToFile(trajectory, filename + ".txt");
            
            // Сохраняем данные для графиков
            calculator.saveGraphData(trajectory, filename + "_graph");
            
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }
    }
    
    // Задание 3: Метод Рунге-Кутта 4-го порядка
    std::cout << "\n\nЗАДАНИЕ 3: МЕТОД РУНГЕ-КУТТА 4-ГО ПОРЯДКА\n";
    std::cout << "===========================================\n";
    
    double dt_rk4 = 0.1;
    
    std::cout << "\n3a. α = θ - θс, dt = " << dt_rk4 << " с\n";
    try {
        auto trajectory = calculator.calculateTrajectory(RUNGE_KUTTA_4, ALPHA_THETA_MINUS_THETAC, dt_rk4);
        calculator.printResultsTable(trajectory);
        
        std::string filename = "results/runge_kutta4_alpha_theta_dt_0.1";
        calculator.saveResultsToFile(trajectory, filename + ".txt");
        
        // Сохраняем данные для графиков
        calculator.saveGraphData(trajectory, filename + "_graph");
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
    
    std::cout << "\n3b. α = 0, dt = " << dt_rk4 << " с\n";
    try {
        auto trajectory = calculator.calculateTrajectory(RUNGE_KUTTA_4, ALPHA_ZERO, dt_rk4);
        calculator.printResultsTable(trajectory);
        
        std::string filename = "results/runge_kutta4_alpha_zero_dt_0.1";
        calculator.saveResultsToFile(trajectory, filename + ".txt");
        
        // Сохраняем данные для графиков
        calculator.saveGraphData(trajectory, filename + "_graph");
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
    
    // Дополнительно: сравнительный анализ для шага 0.1 с
    std::cout << "\n\nСРАВНИТЕЛЬНЫЙ АНАЛИЗ (шаг 0.1 с)\n";
    std::cout << "================================\n";
    
    std::cout << "\nДля α = θ - θс, dt = 0.1 с:\n";
    try {
        auto trajectory_euler = calculator.calculateTrajectory(EULER, ALPHA_THETA_MINUS_THETAC, 0.1);
        auto trajectory_modified = calculator.calculateTrajectory(MODIFIED_EULER, ALPHA_THETA_MINUS_THETAC, 0.1);
        auto trajectory_rk4 = calculator.calculateTrajectory(RUNGE_KUTTA_4, ALPHA_THETA_MINUS_THETAC, 0.1);
        
        std::cout << "Метод Эйлера: конечная высота = " << trajectory_euler.back().y 
                  << " м, скорость = " << trajectory_euler.back().V << " м/с\n";
        std::cout << "Мод. Эйлера: конечная высота = " << trajectory_modified.back().y 
                  << " м, скорость = " << trajectory_modified.back().V << " м/с\n";
        std::cout << "Рунге-Кутта 4: конечная высота = " << trajectory_rk4.back().y 
                  << " м, скорость = " << trajectory_rk4.back().V << " м/с\n";
        
        // Сохраняем сравнительные данные
        std::ofstream comp_file("results/comparison_alpha_theta.txt");
        if (comp_file.is_open()) {
            comp_file << "Метод\tКонечное время (с)\tКонечная высота (м)\tКонечная скорость (м/с)\t"
                     << "Конечная дальность (м)\tКонечная масса (кг)\n";
            comp_file << "Эйлер\t" << trajectory_euler.back().t << "\t" 
                     << trajectory_euler.back().y << "\t" << trajectory_euler.back().V << "\t"
                     << trajectory_euler.back().x << "\t" << trajectory_euler.back().m << "\n";
            comp_file << "Мод.Эйлер\t" << trajectory_modified.back().t << "\t" 
                     << trajectory_modified.back().y << "\t" << trajectory_modified.back().V << "\t"
                     << trajectory_modified.back().x << "\t" << trajectory_modified.back().m << "\n";
            comp_file << "Рунге-Кутта4\t" << trajectory_rk4.back().t << "\t" 
                     << trajectory_rk4.back().y << "\t" << trajectory_rk4.back().V << "\t"
                     << trajectory_rk4.back().x << "\t" << trajectory_rk4.back().m << "\n";
            comp_file.close();
            std::cout << "Сравнительные данные сохранены в results/comparison_alpha_theta.txt\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при сравнении: " << e.what() << std::endl;
    }
    
    std::cout << "\nРАСЧЕТ ЗАВЕРШЕН!\n";
    std::cout << "Все результаты сохранены в папке 'results/'\n\n";
    
    return 0;
}