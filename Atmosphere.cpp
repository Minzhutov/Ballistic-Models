#include "atmosphere.h"
#include <cmath>
#include <stdexcept>

// Константы из ГОСТ 4401-81
const double R = 287.05287;        // Газовая постоянная для воздуха, Дж/(кг·К)
const double G0 = 9.80665;         // Ускорение свободного падения на уровне моря, м/с²
const double T0 = 288.15;          // Температура на уровне моря, К
const double P0 = 101325.0;        // Давление на уровне моря, Па
const double R_EARTH = 6356767.0;  // Радиус Земли, м
const double GAMMA = 1.4;          // Показатель адиабаты для воздуха

// Границы слоев атмосферы (геометрические высоты в метрах)
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

// Ускорение свободного падения на заданной высоте
double calculate_gravity(double altitude) {
    return G0 * pow(R_EARTH / (R_EARTH + altitude), 2);
}

// Геопотенциальная высота
double calculate_geopotential_height(double geometric_height) {
    return (R_EARTH * geometric_height) / (R_EARTH + geometric_height);
}

// Расчет температуры для слоя с постоянным градиентом
double temperature_with_gradient(double T_base, double beta, double H, double H_base) {
    return T_base + beta * (H - H_base);
}

// Расчет давления для слоя с постоянным градиентом
double pressure_with_gradient(double P_base, double T_base, double beta, double H, double H_base) {
    if (std::abs(beta) < 1e-10) {
        // Изотермический слой
        return P_base * exp(-G0 * (H - H_base) / (R * T_base));
    } else {
  