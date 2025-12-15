#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <vector>
#include <string>
#include "atmosphere.h"

enum IntegrationMethod { EULER, MODIFIED_EULER, RUNGE_KUTTA_4 };
enum AlphaLaw { ALPHA_THETA_MINUS_THETAC, ALPHA_ZERO };

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
    double alpha;
    double x_dotc;  // горизонтальная скорость
    double y_dotc;  // вертикальная скорость
    double V_dot;   // производная скорости (ускорение)
};

class TrajectoryCalculator {
private:
    double V0, theta_c0, m_dot, W, y0, omega_z0, theta0;
    double t_end, m0, I_d, S_a, S_m;
    
public:
    TrajectoryCalculator(double V0, double theta_c0, double m_dot, double W,
                        double y0, double omega_z0, double theta0,
                        double t_end, double m0, double I_d, double S_a, double S_m);
    
    // Методы интегрирования
    std::vector<TrajectoryPoint> calculateTrajectory(IntegrationMethod method, 
                                                     AlphaLaw alpha_law, 
                                                     double dt) const;
    
private:
    // Вспомогательные методы
    void addTrajectoryPoint(std::vector<TrajectoryPoint>& trajectory, double t, 
                           const std::vector<double>& state, 
                           const std::vector<double>& derivatives,
                           AlphaLaw alpha_law) const;
    
    void calculateDerivatives(double t, const std::vector<double>& state,
                             std::vector<double>& derivatives, 
                             AlphaLaw alpha_law) const;
    
    std::vector<TrajectoryPoint> integrateEuler(double dt, AlphaLaw alpha_law) const;
    std::vector<TrajectoryPoint> integrateModifiedEuler(double dt, AlphaLaw alpha_law) const;
    std::vector<TrajectoryPoint> integrateRungeKutta4(double dt, AlphaLaw alpha_law) const;
    
public:
    // Методы вывода результатов
    void printResultsTable(const std::vector<TrajectoryPoint>& trajectory) const;
    void saveResultsToFile(const std::vector<TrajectoryPoint>& trajectory, 
                          const std::string& filename) const;
};

#endif