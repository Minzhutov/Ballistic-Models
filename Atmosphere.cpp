#include "atmosphere.h"
#include <cmath>
#include <stdexcept>
#include <cstdio>  

const double R = 287.05287;        // Газовая постоянная для воздуха, Дж/(кг·К)
const double G0 = 9.80665;         // Ускорение свободного падения на уровне моря, м/с²
const double T0 = 288.15;          // Температура на уровне моря, К
const double P0 = 101325.0;        // Давление на уровне моря, Па
const double R_EARTH = 6356767.0;  // Радиус Земли, м

// Границы слоев атмосферы
const double H_TROPOSPHERE = 11000.0;
const double H_STRATOSPHERE1 = 20000.0;
const double H_STRATOSPHERE2 = 32000.0;
const double H_STRATOSPHERE3 = 47000.0;
const double H_MESOSPHERE1 = 51000.0;
const double H_MESOSPHERE2 = 71000.0;
const double H_MESOSPHERE3 = 85000.0;
const double H_THERMOSPHERE = 94000.0;

// Температурные градиенты для разных слоев (К/м)
const double BETA_TROPOSPHERE = 0.0065;
const double BETA_STRATOSPHERE1 = 0.0;
const double BETA_STRATOSPHERE2 = 0.0010;
const double BETA_STRATOSPHERE3 = 0.0028;
const double BETA_MESOSPHERE1 = 0.0;
const double BETA_MESOSPHERE2 = -0.0028;
const double BETA_MESOSPHERE3 = -0.0020;

// Опорные температуры на границах слоев (К)
const double T_TROPOSPHERE = 216.65;
const double T_STRATOSPHERE1 = 216.65;
const double T_STRATOSPHERE2 = 228.65;
const double T_STRATOSPHERE3 = 270.65;
const double T_MESOSPHERE1 = 270.65;
const double T_MESOSPHERE2 = 214.65;
const double T_MESOSPHERE3 = 186.65;

// Опорные давления на границах слоев (Па)
const double P_TROPOSPHERE = 22632.0;
const double P_STRATOSPHERE1 = 5474.9;
const double P_STRATOSPHERE2 = 868.02;
const double P_STRATOSPHERE3 = 110.91;
const double P_MESOSPHERE1 = 66.939;
const double P_MESOSPHERE2 = 3.9564;
const double P_MESOSPHERE3 = 0.3734;

namespace {

// Геопотенциальная высота: H = (R * h) / (R + h)
double calculate_geopotential_height(double geometric_height) {
    return (R_EARTH * geometric_height) / (R_EARTH + geometric_height);
}

// Геометрическая высота: h = (R * H) / (R - H)
double calculate_geometric_height(double geopotential_height) {
    return (R_EARTH * geopotential_height) / (R_EARTH - geopotential_height);
}

// Ускорение свободного падения: g = g0 * (R/(R+h))^2
double calculate_gravity(double geometric_height) {
    double ratio = R_EARTH / (R_EARTH + geometric_height);
    return G0 * ratio * ratio;
}

// Расчет температуры: T = T_base + beta * (H - H_base)
double temperature_with_gradient(double T_base, double beta, double H_geo, double H_base_geo) {
    return T_base + beta * (H_geo - H_base_geo);
}

// Расчет давления по формулам с логарифмами
double pressure_with_gradient(double P_base, double T_base, double beta, double H_geo, double H_base_geo) {
    if (std::abs(beta) < 1e-10) {
        // При beta = 0: lg(p) = lg(p_base) - (0.434294 * g0 * (H - H_base)) / (R * T_base)
        double delta_H = H_geo - H_base_geo;
        double exponent = - (0.434294 * G0 * delta_H) / (R * T_base);
        return P_base * pow(10.0, exponent);
    } else {
        // При beta ≠ 0: lg(p) = lg(p_base) - g0/(beta*R) * lg((T_base + beta*(H-H_base))/T_base)
        double T_current = T_base + beta * (H_geo - H_base_geo);
        double lg_ratio = log10(T_current / T_base);
        double exponent = - (G0 / (beta * R)) * lg_ratio;
        return P_base * pow(10.0, exponent);
    }
}

// Скорость звука: a = 20.046796 * sqrt(T)
double calculate_sound_speed(double temperature) {
    return 20.046796 * sqrt(temperature);
}

// Плотность воздуха: ro = p / (R * T)
double calculate_density(double pressure, double temperature) {
    return pressure / (R * temperature);
}

// Расчет параметров для тропосферы (0-11000 м)
void calculate_troposphere(double H_geo, double& T, double& P) {
    T = temperature_with_gradient(T0, -BETA_TROPOSPHERE, H_geo, 0.0);
    P = pressure_with_gradient(P0, T0, -BETA_TROPOSPHERE, H_geo, 0.0);
}

// Расчет параметров для нижней стратосферы (11000-20000 м)
void calculate_stratosphere1(double H_geo, double& T, double& P) {
    double H_base_geo = calculate_geopotential_height(H_TROPOSPHERE);
    T = T_TROPOSPHERE;
    P = pressure_with_gradient(P_TROPOSPHERE, T_TROPOSPHERE, 0.0, H_geo, H_base_geo);
}

// Расчет параметров для средней стратосферы (20000-32000 м)
void calculate_stratosphere2(double H_geo, double& T, double& P) {
    double H_base_geo = calculate_geopotential_height(H_STRATOSPHERE1);
    T = temperature_with_gradient(T_STRATOSPHERE1, BETA_STRATOSPHERE2, H_geo, H_base_geo);
    P = pressure_with_gradient(P_STRATOSPHERE1, T_STRATOSPHERE1, BETA_STRATOSPHERE2, H_geo, H_base_geo);
}

// Расчет параметров для верхней стратосферы (32000-47000 м)
void calculate_stratosphere3(double H_geo, double& T, double& P) {
    double H_base_geo = calculate_geopotential_height(H_STRATOSPHERE2);
    T = temperature_with_gradient(T_STRATOSPHERE2, BETA_STRATOSPHERE3, H_geo, H_base_geo);
    P = pressure_with_gradient(P_STRATOSPHERE2, T_STRATOSPHERE2, BETA_STRATOSPHERE3, H_geo, H_base_geo);
}

// Расчет параметров для нижней мезосферы (47000-51000 м)
void calculate_mesosphere1(double H_geo, double& T, double& P) {
    double H_base_geo = calculate_geopotential_height(H_STRATOSPHERE3);
    T = T_STRATOSPHERE3;
    P = pressure_with_gradient(P_STRATOSPHERE3, T_STRATOSPHERE3, 0.0, H_geo, H_base_geo);
}

// Расчет параметров для средней мезосферы (51000-71000 м)
void calculate_mesosphere2(double H_geo, double& T, double& P) {
    double H_base_geo = calculate_geopotential_height(H_MESOSPHERE1);
    T = temperature_with_gradient(T_MESOSPHERE1, BETA_MESOSPHERE2, H_geo, H_base_geo);
    P = pressure_with_gradient(P_MESOSPHERE1, T_MESOSPHERE1, BETA_MESOSPHERE2, H_geo, H_base_geo);
}

// Расчет параметров для верхней мезосферы (71000-85000 м)
void calculate_mesosphere3(double H_geo, double& T, double& P) {
    double H_base_geo = calculate_geopotential_height(H_MESOSPHERE2);
    T = temperature_with_gradient(T_MESOSPHERE2, BETA_MESOSPHERE3, H_geo, H_base_geo);
    P = pressure_with_gradient(P_MESOSPHERE2, T_MESOSPHERE2, BETA_MESOSPHERE3, H_geo, H_base_geo);
}

// Расчет параметров для нижней термосферы (85000-94000 м)
void calculate_thermosphere(double H_geo, double& T, double& P) {
    double H_base_geo = calculate_geopotential_height(H_MESOSPHERE3);
    T = T_MESOSPHERE3;
    P = pressure_with_gradient(P_MESOSPHERE3, T_MESOSPHERE3, 0.0, H_geo, H_base_geo);
}

} // namespace

extern "C" AtmosphereParams calculate_atmosphere(double altitude) {
    if (altitude < -2000.0 || altitude > 94000.0) {
        char error_msg[100];
        snprintf(error_msg, sizeof(error_msg), "Высота %.1f вне диапазона [-2000, 94000] метров", altitude);
        throw std::invalid_argument(error_msg);
    }
    
    AtmosphereParams result;
    
    // Сохраняем геометрическую высоту
    result.H_geom = altitude;
    
    // Вычисляем геопотенциальную высоту
    result.H_geo = calculate_geopotential_height(altitude);
    
    // Вычисляем ускорение свободного падения
    result.g = calculate_gravity(altitude);
    
    double T, P;
    
    // Выбираем соответствующий слой атмосферы
    if (altitude <= H_TROPOSPHERE) {
        calculate_troposphere(result.H_geo, T, P);
    } else if (altitude <= H_STRATOSPHERE1) {
        calculate_stratosphere1(result.H_geo, T, P);
    } else if (altitude <= H_STRATOSPHERE2) {
        calculate_stratosphere2(result.H_geo, T, P);
    } else if (altitude <= H_STRATOSPHERE3) {
        calculate_stratosphere3(result.H_geo, T, P);
    } else if (altitude <= H_MESOSPHERE1) {
        calculate_mesosphere1(result.H_geo, T, P);
    } else if (altitude <= H_MESOSPHERE2) {
        calculate_mesosphere2(result.H_geo, T, P);
    } else if (altitude <= H_MESOSPHERE3) {
        calculate_mesosphere3(result.H_geo, T, P);
    } else {
        calculate_thermosphere(result.H_geo, T, P);
    }
    
    result.T = T;
    result.p = P;
    result.ro = calculate_density(result.p, result.T);
    result.a = calculate_sound_speed(result.T);
    
    return result;
}