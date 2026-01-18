#include "trajectory.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

// Глобальные аэродинамические таблицы
std::vector<double> M_table = {0.01, 0.55, 0.8, 0.9, 1.0, 1.06, 1.1, 1.2, 
                               1.3, 1.4, 2.0, 2.6, 3.4, 6.0, 10.2};

std::vector<double> Cxa_table = {0.30, 0.30, 0.55, 0.70, 0.84, 0.86, 0.87, 
                                 0.83, 0.80, 0.79, 0.65, 0.55, 0.50, 0.45, 0.41};

std::vector<double> Cya_alpha_table = {0.25, 0.25, 0.25, 0.20, 0.30, 0.31, 0.25,
                                      0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25};

// Вспомогательные функции
double deg2rad(double deg) {
    return deg * M_PI / 180.0;
}

double rad2deg(double rad) {
    return rad * 180.0 / M_PI;
}

double interpolate_linear(double x, const std::vector<double>& x_vals, 
                         const std::vector<double>& y_vals) {
    if (x <= x_vals.front()) return y_vals.front();
    if (x >= x_vals.back()) return y_vals.back();
    
    for (size_t i = 0; i < x_vals.size() - 1; ++i) {
        if (x >= x_vals[i] && x <= x_vals[i+1]) {
            double t = (x - x_vals[i]) / (x_vals[i+1] - x_vals[i]);
            return y_vals[i] + t * (y_vals[i+1] - y_vals[i]);
        }
    }
    return y_vals.back();
}

double interpolate_Cxa(double M) {
    return interpolate_linear(M, M_table, Cxa_table);
}

double interpolate_Cya_alpha(double M) {
    return interpolate_linear(M, M_table, Cya_alpha_table);
}

// Конструктор TrajectoryCalculator
TrajectoryCalculator::TrajectoryCalculator(double V0, double theta_c0, double m_dot, double W,
                                          double y0, double omega_z0, double theta0,
                                          double t_end, double m0, double I_d, double S_a, double S_m)
    : V0(V0), theta_c0(theta_c0), m_dot(m_dot), W(W),
      y0(y0), omega_z0(omega_z0), theta0(theta0),
      t_end(t_end), m0(m0), I_d(I_d), S_a(S_a), S_m(S_m) {
}

// Добавление точки траектории (ИСПРАВЛЕННАЯ ВЕРСИЯ)
void TrajectoryCalculator::addTrajectoryPoint(std::vector<TrajectoryPoint>& trajectory, double t, 
                                             const std::vector<double>& state, 
                                             const std::vector<double>& derivatives,
                                             AlphaLaw alpha_law) const {
    TrajectoryPoint point;
    point.t = t;
    point.V = state[0];
    point.theta_c = state[1];
    point.x = state[2];
    point.y = state[3];
    point.omega_z = state[4];
    point.theta = state[5];
    point.m = state[6];
    
    // Сохраняем производные
    if (derivatives.size() >= 7) {
        point.V_dot = derivatives[0];   // dV/dt
        // point.theta_c_dot = derivatives[1]; // если нужно
        point.x_dotc = derivatives[2];  // dx/dt
        point.y_dotc = derivatives[3];  // dy/dt
    } else {
        point.V_dot = 0.0;
        point.x_dotc = 0.0;
        point.y_dotc = 0.0;
    }
    
    // Тяга (упрощённая формула)
    point.P = m_dot * W;
    
    // Защита от отрицательной высоты
    if (point.y < 0) point.y = 0;
    
    try {
        // Параметры атмосферы
        AtmosphereParams atm = calculate_atmosphere(point.y);
        point.g = atm.g;
        point.M = point.V / atm.a;
        
        // Аэродинамические коэффициенты
        point.Cxa = interpolate_Cxa(point.M);
        point.Cya_alpha = interpolate_Cya_alpha(point.M);
    } catch (const std::exception& e) {
        // Если ошибка при расчёте атмосферы, используем значения по умолчанию
        point.g = 9.80665;
        point.M = point.V / 340.0;  // Примерная скорость звука
        point.Cxa = 0.3;
        point.Cya_alpha = 0.25;
    }
    
    // Угол атаки
    if (alpha_law == ALPHA_THETA_MINUS_THETAC) {
        point.alpha = point.theta - point.theta_c;
    } else {
        point.alpha = 0.0;
    }
    
    trajectory.push_back(point);
}

// Расчёт производных (ИСПРАВЛЕННЫЕ УРАВНЕНИЯ)
void TrajectoryCalculator::calculateDerivatives(double t, const std::vector<double>& state,
                                               std::vector<double>& derivatives, 
                                               AlphaLaw alpha_law) const {
    double V = state[0];
    double theta_c = state[1];  // в градусах
    double y = state[3];
    double theta = state[5];    // в градусах
    double m = state[6];
    
    // Преобразуем углы в радианы
    double theta_c_rad = deg2rad(theta_c);
    double theta_rad = deg2rad(theta);
    
    // Защита от нулевой или отрицательной массы
    if (m <= 0.01 * m0) {
        m = 0.01 * m0;
    }
    
    // Получаем параметры атмосферы
    AtmosphereParams atm;
    try {
        atm = calculate_atmosphere(y);
    } catch (const std::exception& e) {
        // Используем значения по умолчанию
        atm.g = 9.80665;
        atm.ro = 1.225;
        atm.a = 340.0;
    }
    
    // Число Маха и аэродинамические коэффициенты
    double M = V / atm.a;
    if (M < 0.01) M = 0.01;
    if (M > 10.2) M = 10.2;
    
    double Cxa = interpolate_Cxa(M);
    double Cya_alpha_val = interpolate_Cya_alpha(M);
    
    // Угол атаки
    double alpha_rad;
    if (alpha_law == ALPHA_THETA_MINUS_THETAC) {
        alpha_rad = theta_rad - theta_c_rad;
    } else {
        alpha_rad = 0.0;
    }
    
    // Динамическое давление
    double q = 0.5 * atm.ro * V * V;
    
    // Аэродинамические силы
    double Xa = q * S_m * Cxa;
    double Ya = q * S_m * Cya_alpha_val * alpha_rad;
    
    // Тяга
    double P = m_dot * W;
    
    // Производные (ИСПРАВЛЕННЫЕ ФОРМУЛЫ)
    derivatives.resize(7);
    
    // dV/dt = (P * cos(alpha) - Xa)/m - g * sin(theta_c)
    derivatives[0] = (P * cos(alpha_rad) - Xa) / m - atm.g * sin(theta_c_rad);
    
    // dtheta_c/dt = (P * sin(alpha) + Ya)/(m * V) - (g * cos(theta_c))/V
    if (V > 1.0) {  // Защита от деления на ноль
        derivatives[1] = rad2deg((P * sin(alpha_rad) + Ya) / (m * V) - (atm.g * cos(theta_c_rad)) / V);
    } else {
        derivatives[1] = 0.0;
    }
    
    // dx/dt = V * cos(theta_c)
    derivatives[2] = V * cos(theta_c_rad);
    
    // dy/dt = V * sin(theta_c)
    derivatives[3] = V * sin(theta_c_rad);
    
    // domega_z/dt = 0 (упрощенно)
    derivatives[4] = 0.0;
    
    // dtheta/dt = omega_z
    derivatives[5] = state[4];
    
    // dm/dt = -m_dot
    derivatives[6] = -m_dot;
}

// Метод Эйлера
std::vector<TrajectoryPoint> TrajectoryCalculator::integrateEuler(double dt, AlphaLaw alpha_law) const {
    std::vector<TrajectoryPoint> trajectory;
    std::vector<double> state = {V0, theta_c0, 0.0, y0, omega_z0, theta0, m0};
    double t = 0.0;
    
    // Начальная точка
    std::vector<double> initial_derivatives;
    calculateDerivatives(t, state, initial_derivatives, alpha_law);
    addTrajectoryPoint(trajectory, t, state, initial_derivatives, alpha_law);
    
    while (t < t_end && state[6] > 0.1 * m0) {
        std::vector<double> derivatives;
        calculateDerivatives(t, state, derivatives, alpha_law);
        
        // Интегрирование
        for (size_t i = 0; i < state.size(); ++i) {
            state[i] += derivatives[i] * dt;
        }
        
        // Защита от отрицательных значений
        if (state[3] < 0) state[3] = 0;  // Высота не может быть отрицательной
        if (state[0] < 0) state[0] = 0;  // Скорость не может быть отрицательной
        
        t += dt;
        
        // Сохраняем точку каждые 0.1 секунды
        if (fmod(t, 0.1) < dt/2.0 || dt <= 0.1) {
            std::vector<double> new_derivatives;
            calculateDerivatives(t, state, new_derivatives, alpha_law);
            addTrajectoryPoint(trajectory, t, state, new_derivatives, alpha_law);
        }
    }
    
    // Добавляем конечную точку
    if (trajectory.empty() || trajectory.back().t < t_end) {
        std::vector<double> final_derivatives;
        calculateDerivatives(t, state, final_derivatives, alpha_law);
        addTrajectoryPoint(trajectory, t, state, final_derivatives, alpha_law);
    }
    
    return trajectory;
}

// Модифицированный метод Эйлера
std::vector<TrajectoryPoint> TrajectoryCalculator::integrateModifiedEuler(double dt, AlphaLaw alpha_law) const {
    std::vector<TrajectoryPoint> trajectory;
    std::vector<double> state = {V0, theta_c0, 0.0, y0, omega_z0, theta0, m0};
    double t = 0.0;
    
    // Начальная точка
    std::vector<double> initial_derivatives;
    calculateDerivatives(t, state, initial_derivatives, alpha_law);
    addTrajectoryPoint(trajectory, t, state, initial_derivatives, alpha_law);
    
    while (t < t_end && state[6] > 0.1 * m0) {
        std::vector<double> k1, k2;
        
        // k1
        calculateDerivatives(t, state, k1, alpha_law);
        
        // Промежуточное состояние
        std::vector<double> state_temp = state;
        for (size_t i = 0; i < state_temp.size(); ++i) {
            state_temp[i] += k1[i] * dt;
        }
        
        // Защита промежуточных значений
        if (state_temp[3] < 0) state_temp[3] = 0;
        
        // k2
        calculateDerivatives(t + dt, state_temp, k2, alpha_law);
        
        // Интегрирование
        for (size_t i = 0; i < state.size(); ++i) {
            state[i] += (k1[i] + k2[i]) * dt / 2.0;
        }
        
        // Защита финальных значений
        if (state[3] < 0) state[3] = 0;
        if (state[0] < 0) state[0] = 0;
        
        t += dt;
        
        if (fmod(t, 0.1) < dt/2.0 || dt <= 0.1) {
            std::vector<double> new_derivatives;
            calculateDerivatives(t, state, new_derivatives, alpha_law);
            addTrajectoryPoint(trajectory, t, state, new_derivatives, alpha_law);
        }
    }
    
    if (trajectory.empty() || trajectory.back().t < t_end) {
        std::vector<double> final_derivatives;
        calculateDerivatives(t, state, final_derivatives, alpha_law);
        addTrajectoryPoint(trajectory, t, state, final_derivatives, alpha_law);
    }
    
    return trajectory;
}

// Метод Рунге-Кутта 4-го порядка
std::vector<TrajectoryPoint> TrajectoryCalculator::integrateRungeKutta4(double dt, AlphaLaw alpha_law) const {
    std::vector<TrajectoryPoint> trajectory;
    std::vector<double> state = {V0, theta_c0, 0.0, y0, omega_z0, theta0, m0};
    double t = 0.0;
    
    // Начальная точка
    std::vector<double> initial_derivatives;
    calculateDerivatives(t, state, initial_derivatives, alpha_law);
    addTrajectoryPoint(trajectory, t, state, initial_derivatives, alpha_law);
    
    while (t < t_end && state[6] > 0.1 * m0) {
        std::vector<double> k1, k2, k3, k4;
        std::vector<double> state_temp;
        
        // k1
        calculateDerivatives(t, state, k1, alpha_law);
        
        // k2
        state_temp = state;
        for (size_t i = 0; i < state_temp.size(); ++i) {
            state_temp[i] += k1[i] * dt / 2.0;
        }
        if (state_temp[3] < 0) state_temp[3] = 0;
        calculateDerivatives(t + dt/2.0, state_temp, k2, alpha_law);
        
        // k3
        state_temp = state;
        for (size_t i = 0; i < state_temp.size(); ++i) {
            state_temp[i] += k2[i] * dt / 2.0;
        }
        if (state_temp[3] < 0) state_temp[3] = 0;
        calculateDerivatives(t + dt/2.0, state_temp, k3, alpha_law);
        
        // k4
        state_temp = state;
        for (size_t i = 0; i < state_temp.size(); ++i) {
            state_temp[i] += k3[i] * dt;
        }
        if (state_temp[3] < 0) state_temp[3] = 0;
        calculateDerivatives(t + dt, state_temp, k4, alpha_law);
        
        // Интегрирование
        for (size_t i = 0; i < state.size(); ++i) {
            state[i] += (k1[i] + 2.0*k2[i] + 2.0*k3[i] + k4[i]) * dt / 6.0;
        }
        
        // Защита
        if (state[3] < 0) state[3] = 0;
        if (state[0] < 0) state[0] = 0;
        
        t += dt;
        
        if (fmod(t, 0.1) < dt/2.0 || dt <= 0.1) {
            std::vector<double> new_derivatives;
            calculateDerivatives(t, state, new_derivatives, alpha_law);
            addTrajectoryPoint(trajectory, t, state, new_derivatives, alpha_law);
        }
    }
    
    if (trajectory.empty() || trajectory.back().t < t_end) {
        std::vector<double> final_derivatives;
        calculateDerivatives(t, state, final_derivatives, alpha_law);
        addTrajectoryPoint(trajectory, t, state, final_derivatives, alpha_law);
    }
    
    return trajectory;
}

// Сохранение данных для графиков
void TrajectoryCalculator::saveGraphData(const std::vector<TrajectoryPoint>& trajectory, 
                                        const std::string& base_filename) const {
    if (trajectory.empty()) {
        std::cerr << "Траектория пуста, данные для графиков не могут быть сохранены\n";
        return;
    }
    
    // Фильтруем точки с шагом 0.1 секунды
    std::vector<const TrajectoryPoint*> filtered_points;
    for (const auto& p : trajectory) {
        double remainder = fmod(p.t + 1e-9, 0.1);
        if (remainder < 1e-6 || remainder > 0.099999) {
            filtered_points.push_back(&p);
        }
    }
    
    if (filtered_points.empty()) {
        std::cerr << "Нет точек с шагом 0.1с для сохранения графиков\n";
        return;
    }
    
    // 1. V(t) - Скорость от времени
    std::ofstream file_vt(base_filename + "_Vt.txt");
    if (file_vt.is_open()) {
        file_vt << "t(c)\tV(m/s)\n";
        for (const auto* p : filtered_points) {
            file_vt << std::fixed << std::setprecision(3) << p->t << "\t"
                   << std::setprecision(3) << p->V << "\n";
        }
        file_vt.close();
        std::cout << "Данные для графика V(t) сохранены в: " << base_filename + "_Vt.txt (шаг 0.1с)\n";
    }
    
    // 2. θ_c(t) - Угол наклона траектории от времени
    std::ofstream file_thetact(base_filename + "_thetact.txt");
    if (file_thetact.is_open()) {
        file_thetact << "t(c)\ttheta_c(grad)\n";
        for (const auto* p : filtered_points) {
            file_thetact << std::fixed << std::setprecision(3) << p->t << "\t"
                        << std::setprecision(3) << p->theta_c << "\n";
        }
        file_thetact.close();
        std::cout << "Данные для графика theta_c(t) сохранены в: " << base_filename + "_thetact.txt (шаг 0.1с)\n";
    }
    
    // 3. y(t) - Высота от времени
    std::ofstream file_yt(base_filename + "_yt.txt");
    if (file_yt.is_open()) {
        file_yt << "t(c)\ty(m)\n";
        for (const auto* p : filtered_points) {
            file_yt << std::fixed << std::setprecision(3) << p->t << "\t"
                   << std::setprecision(2) << p->y << "\n";
        }
        file_yt.close();
        std::cout << "Данные для графика y(t) сохранены в: " << base_filename + "_yt.txt (шаг 0.1с)\n";
    }
    
    // 4. x(t) - Дальность от времени
    std::ofstream file_xt(base_filename + "_xt.txt");
    if (file_xt.is_open()) {
        file_xt << "t(c)\tx(m)\n";
        for (const auto* p : filtered_points) {
            file_xt << std::fixed << std::setprecision(3) << p->t << "\t"
                   << std::setprecision(2) << p->x << "\n";
        }
        file_xt.close();
        std::cout << "Данные для графика x(t) сохранены в: " << base_filename + "_xt.txt (шаг 0.1с)\n";
    }
    
    // 5. ω_z(t) - Угловая скорость от времени
    std::ofstream file_omegazt(base_filename + "_omegazt.txt");
    if (file_omegazt.is_open()) {
        file_omegazt << "t(c)\tomega_z(1/s)\n";
        for (const auto* p : filtered_points) {
            file_omegazt << std::fixed << std::setprecision(3) << p->t << "\t"
                        << std::setprecision(4) << p->omega_z << "\n";
        }
        file_omegazt.close();
        std::cout << "Данные для графика omega_z(t) сохранены в: " << base_filename + "_omegazt.txt (шаг 0.1с)\n";
    }
    
    // 6. θ(t) - Угол тангажа от времени
    std::ofstream file_thetat(base_filename + "_thetat.txt");
    if (file_thetat.is_open()) {
        file_thetat << "t(c)\ttheta(grad)\n";
        for (const auto* p : filtered_points) {
            file_thetat << std::fixed << std::setprecision(3) << p->t << "\t"
                       << std::setprecision(3) << p->theta << "\n";
        }
        file_thetat.close();
        std::cout << "Данные для графика theta(t) сохранены в: " << base_filename + "_thetat.txt (шаг 0.1с)\n";
    }
    
    // 7. α(t) - Угол атаки от времени
    std::ofstream file_alphat(base_filename + "_alphat.txt");
    if (file_alphat.is_open()) {
        file_alphat << "t(c)\talpha(grad)\n";
        for (const auto* p : filtered_points) {
            file_alphat << std::fixed << std::setprecision(3) << p->t << "\t"
                       << std::setprecision(3) << p->alpha << "\n";
        }
        file_alphat.close();
        std::cout << "Данные для графика alpha(t) сохранены в: " << base_filename + "_alphat.txt (шаг 0.1с)\n";
    }
    
    // 8. V(x) - Скорость от дальности
    std::ofstream file_vx(base_filename + "_Vx.txt");
    if (file_vx.is_open()) {
        file_vx << "x(m)\tV(m/s)\n";
        for (const auto* p : filtered_points) {
            file_vx << std::fixed << std::setprecision(2) << p->x << "\t"
                   << std::setprecision(3) << p->V << "\n";
        }
        file_vx.close();
        std::cout << "Данные для графика V(x) сохранены в: " << base_filename + "_Vx.txt (шаг 0.1с)\n";
    }
    
    // 9. θ_c(x) - Угол наклона траектории от дальности
    std::ofstream file_thetacx(base_filename + "_thetacx.txt");
    if (file_thetacx.is_open()) {
        file_thetacx << "x(m)\ttheta_c(grad)\n";
        for (const auto* p : filtered_points) {
            file_thetacx << std::fixed << std::setprecision(2) << p->x << "\t"
                        << std::setprecision(3) << p->theta_c << "\n";
        }
        file_thetacx.close();
        std::cout << "Данные для графика theta_c(x) сохранены в: " << base_filename + "_thetacx.txt (шаг 0.1с)\n";
    }
    
    // 10. y(x) - Высота от дальности (траектория)
    std::ofstream file_yx(base_filename + "_yx.txt");
    if (file_yx.is_open()) {
        file_yx << "x(m)\ty(m)\n";
        for (const auto* p : filtered_points) {
            file_yx << std::fixed << std::setprecision(2) << p->x << "\t"
                   << std::setprecision(2) << p->y << "\n";
        }
        file_yx.close();
        std::cout << "Данные для графика y(x) сохранены в: " << base_filename + "_yx.txt (шаг 0.1с)\n";
    }
    
    // 11. Сводный файл со всеми параметрами для комплексного анализа
    std::ofstream file_summary(base_filename + "_summary.txt");
    if (file_summary.is_open()) {
        file_summary << "t(c)\tx(m)\ty(m)\tV(m/s)\ttheta_c(grad)\ttheta(grad)\talpha(grad)\t"
                    << "omega_z(1/s)\tM\tCxa\tCya_alpha\tm(kg)\tP(H)\tg(m/s2)\n";
        for (const auto* p : filtered_points) {
            file_summary << std::fixed << std::setprecision(3) << p->t << "\t"
                        << std::setprecision(2) << p->x << "\t"
                        << std::setprecision(2) << p->y << "\t"
                        << std::setprecision(3) << p->V << "\t"
                        << std::setprecision(3) << p->theta_c << "\t"
                        << std::setprecision(3) << p->theta << "\t"
                        << std::setprecision(3) << p->alpha << "\t"
                        << std::setprecision(4) << p->omega_z << "\t"
                        << std::setprecision(4) << p->M << "\t"
                        << std::setprecision(4) << p->Cxa << "\t"
                        << std::setprecision(4) << p->Cya_alpha << "\t"
                        << std::setprecision(2) << p->m << "\t"
                        << std::setprecision(1) << p->P << "\t"
                        << std::setprecision(4) << p->g << "\n";
        }
        file_summary.close();
        std::cout << "Сводные данные сохранены в: " << base_filename + "_summary.txt (шаг 0.1с)\n";
    }
}


std::vector<TrajectoryPoint> TrajectoryCalculator::calculateTrajectory(IntegrationMethod method, 
                                                                      AlphaLaw alpha_law, 
                                                                      double dt) const {
    try {
        switch (method) {
            case EULER:
                return integrateEuler(dt, alpha_law);
            case MODIFIED_EULER:
                return integrateModifiedEuler(dt, alpha_law);
            case RUNGE_KUTTA_4:
                return integrateRungeKutta4(dt, alpha_law);
            default:
                return integrateRungeKutta4(dt, alpha_law);
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при расчёте траектории: " << e.what() << std::endl;
        return std::vector<TrajectoryPoint>();
    }
}

// Сохранение результатов в файл
// Сохранение результатов в файл с шагом 0.1 секунды
void TrajectoryCalculator::saveResultsToFile(const std::vector<TrajectoryPoint>& trajectory, 
                                            const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        return;
    }
    
    // Заголовок с V_dot, x_dotc, y_dotc
    file << "N\tt(c)\tm(kg)\tP(H)\tV(m/s)\tM\tCxa\talpha(grad)\ttheta_c(grad)\t"
         << "Cya_alpha\tomega_z(1/s)\ttheta(grad)\ty(m)\tx(m)\tg(m/s2)\t"
         << "x_dotc(m/s)\ty_dotc(m/s)\tV_dot(m/s2)\n";
    
    // Счетчик строк
    int line_count = 1;
    
    // Фильтруем данные с шагом 0.1 секунды
    for (size_t i = 0; i < trajectory.size(); ++i) {
        const TrajectoryPoint& p = trajectory[i];
        
        // Оставляем только точки с t, кратным 0.1 (с небольшой погрешностью для плавающих чисел)
        double remainder = fmod(p.t + 1e-9, 0.1);
        if (remainder < 1e-6 || remainder > 0.099999) {
            file << line_count++ << "\t"
                 << std::fixed << std::setprecision(3) << p.t << "\t"
                 << std::setprecision(2) << p.m << "\t"
                 << std::setprecision(1) << p.P << "\t"
                 << std::setprecision(3) << p.V << "\t"
                 << std::setprecision(4) << p.M << "\t"
                 << std::setprecision(4) << p.Cxa << "\t"
                 << std::setprecision(2) << p.alpha << "\t"
                 << std::setprecision(2) << p.theta_c << "\t"
                 << std::setprecision(4) << p.Cya_alpha << "\t"
                 << std::setprecision(4) << p.omega_z << "\t"
                 << std::setprecision(2) << p.theta << "\t"
                 << std::setprecision(2) << p.y << "\t"
                 << std::setprecision(2) << p.x << "\t"
                 << std::setprecision(4) << p.g << "\t"
                 << std::setprecision(3) << p.x_dotc << "\t"
                 << std::setprecision(3) << p.y_dotc << "\t"
                 << std::setprecision(3) << p.V_dot << "\n";
        }
    }
    
    file.close();
    std::cout << "Результаты сохранены в файл: " << filename << " (шаг 0.1с)" << std::endl;
}

// Печать таблицы результатов
void TrajectoryCalculator::printResultsTable(const std::vector<TrajectoryPoint>& trajectory) const {
    if (trajectory.empty()) {
        std::cout << "Траектория пуста!\n";
        return;
    }
    
    std::cout << "\nТАБЛИЦА РЕЗУЛЬТАТОВ РАСЧЁТА ТРАЕКТОРИИ\n";
    std::cout << "=========================================\n";
    std::cout << std::setw(4) << "N" 
              << std::setw(8) << "t(c)" 
              << std::setw(8) << "V(m/s)"
              << std::setw(8) << "V_dot"
              << std::setw(8) << "M"
              << std::setw(8) << "alpha"
              << std::setw(10) << "theta_c"
              << std::setw(8) << "y(m)"
              << std::setw(10) << "x(m)"
              << std::setw(8) << "m(kg)\n";
    std::cout << std::string(80, '-') << "\n";
    
    // Выводим каждую 5-ю точку для компактности
    size_t step = trajectory.size() > 25 ? trajectory.size() / 25 : 1;
    if (step < 1) step = 1;
    
    for (size_t i = 0; i < trajectory.size(); i += step) {
        const TrajectoryPoint& p = trajectory[i];
        std::cout << std::setw(4) << i+1
                  << std::setw(8) << std::fixed << std::setprecision(1) << p.t
                  << std::setw(8) << std::setprecision(1) << p.V
                  << std::setw(8) << std::setprecision(2) << p.V_dot
                  << std::setw(8) << std::setprecision(3) << p.M
                  << std::setw(8) << std::setprecision(2) << p.alpha
                  << std::setw(10) << std::setprecision(2) << p.theta_c
                  << std::setw(8) << std::setprecision(0) << p.y
                  << std::setw(10) << std::setprecision(0) << p.x
                  << std::setw(8) << std::setprecision(1) << p.m << "\n";
    }
    
    // Выводим итоговые значения
    std::cout << "\nИТОГОВЫЕ ЗНАЧЕНИЯ:\n";
    const TrajectoryPoint& last = trajectory.back();
    std::cout << "Время полета: " << last.t << " с\n";
    std::cout << "Конечная скорость: " << last.V << " м/с\n";
    std::cout << "Конечное ускорение (V_dot): " << last.V_dot << " м/с²\n";
    std::cout << "Конечная высота: " << last.y << " м\n";
    std::cout << "Конечная дальность: " << last.x << " м\n";
    std::cout << "Конечная масса: " << last.m << " кг\n";
    std::cout << "Горизонтальная скорость (x_dotc): " << last.x_dotc << " м/с\n";
    std::cout << "Вертикальная скорость (y_dotc): " << last.y_dotc << " м/с\n";
}