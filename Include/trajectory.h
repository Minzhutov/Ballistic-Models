#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "atmosphere.h"
#include <vector>
#include <string>

// Структура для хранения параметров траектории
struct TrajectoryPoint {
    double t;
    double V;
    double theta_c;
    double x;
    double y;
    double omega_z;
    double theta;
    double m;
    double P;
    double g;
    double M;
    double Cxa;
    double Cya_alpha;
    double V_dot;
    double alpha;
    double x_dotc; // ← добавить
    double y_dotc; // ← добавить
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