#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "atmosphere.h"
#include <vector>
#include <string>

// Структура для хранения параметров траектории
struct TrajectoryPoint {
    double t;           // Время, с
    double m;           // Масса, кг
    double P;           // Тяга, Н
    double V;           // Скорость, м/с
    double M;           // Число Маха
    double Cxa;         // Коэффициент лобового сопротивления
    double alpha;       // Угол атаки, град
    double theta_c;     // Угол наклона траектории, град
    double Cya_alpha;   // Производная коэффициента подъёмной силы по углу атаки
    double omega_z;     // Угловая скорость вращения, с^-1
    double theta;       // Угол тангажа, град
    double y;           // Высота, м
    double x;           // Дальность, м
    double g;           // Ускорение свободного падения, м/с²
};

// Перечисление для выбора метода интегрирования
enum IntegrationMethod {
    EULER,
    MODIFIED_EULER,
    RUNGE_KUTTA_4
};

// Перечисление для закона управления углом атаки
enum AlphaLaw {
    ALPHA_THETA_MINUS_THETAC,  // α = θ - θс
    ALPHA_ZERO                 // α = 0
};

// Класс для расчёта траектории
class TrajectoryCalculator {
private:
    // Исходные данные
    double V0, theta_c0, m_dot, W, y0, omega_z0, theta0;
    double t_end, m0, I_d, S_a, S_m;
    
    // Вспомогательные функции
    void addTrajectoryPoint(std::vector<TrajectoryPoint>& trajectory, double t, 
                           const std::vector<double>& state, AlphaLaw alpha_law) const;
    
public:
    // Конструктор
    TrajectoryCalculator(double V0, double theta_c0, double m_dot, double W,
                        double y0, double omega_z0, double theta0,
                        double t_end, double m0, double I_d, double S_a, double S_m);
    
    // Основные функции
    void calculateDerivatives(double t, const std::vector<double>& state,
                             std::vector<double>& derivatives, AlphaLaw alpha_law) const;
    
    std::vector<TrajectoryPoint> integrateEuler(double dt, AlphaLaw alpha_law) const;
    std::vector<TrajectoryPoint> integrateModifiedEuler(double dt, AlphaLaw alpha_law) const;
    std::vector<TrajectoryPoint> integrateRungeKutta4(double dt, AlphaLaw alpha_law) const;
    
    std::vector<TrajectoryPoint> calculateTrajectory(IntegrationMethod method, 
                                                     AlphaLaw alpha_law, double dt) const;
    
    // Вспомогательные функции
    void saveResultsToFile(const std::vector<TrajectoryPoint>& trajectory, 
                          const std::string& filename) const;
    void printResultsTable(const std::vector<TrajectoryPoint>& trajectory) const;
};

// Вспомогательные функции (объявлены здесь для удобства)
double deg2rad(double deg);
double rad2deg(double rad);
double interpolate_linear(double x, const std::vector<double>& x_vals, 
                         const std::vector<double>& y_vals);
double interpolate_Cxa(double M);
double interpolate_Cya_alpha(double M);

#endif // TRAJECTORY_H