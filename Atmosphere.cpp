#include "Atmosphere.h"
#include <cmath>
#include <stdexcept>


const double R = 287.05287;        // Газовая постоянная для воздуха, Дж/(кг·К)
const double G0 = 9.80665;         // Ускорение свободного падения на уровне моря, м/с²
const double T0 = 288.15;          // Температура на уровне моря, К
const double P0 = 101325.0;        // Давление на уровне моря, Па
const double R_EARTH = 6356767.0;  // Радиус Земли, м
const double BETA = 0.0065;        // Температурный градиент в тропосфере, К/м
const double GAMMA = 1.4;          // Показатель адиабаты для воздуха

// Вспомогательные функции
namespace {

// Ускорение свободного падения на заданной высоте
double calculate_gravity(double altitude) {
    return G0 * pow(R_EARTH / (R_EARTH + altitude), 2);
}

// Геопотенциальная высота
double calculate_geopotential_height(double geometric_height) {
    return (R_EARTH * geometric_height) / (R_EARTH + geometric_height);
}

// Температура в тропосфере (0-11000 м)
double temperature_troposphere(double H) {
    return T0 - BETA * H;
}

// Давление в тропосфере (0-11000 м)
double pressure_troposphere(double H, double T) {
    return P0 * pow(T / T0, G0 / (BETA * R));
}

// Плотность воздуха
double calculate_density(double pressure, double temperature) {
    return pressure / (R * temperature);
}

// Скорость звука
double calculate_sound_speed(double temperature) {
    return sqrt(GAMMA * R * temperature);
}

} // namespace

// Основная функция расчета
ATMOSPHERE_API AtmosphereParams calculate_atmosphere(double altitude) {
    if (altitude < 0) {
        throw std::invalid_argument("Высота не может быть отрицательной");
    }
    
    AtmosphereParams result;
    
    // Расчет геопотенциальной высоты
    result.H = calculate_geopotential_height(altitude);
    
    // Расчет ускорения свободного падения
    result.g = calculate_gravity(altitude);
    
    // Расчет температуры (для высот до 11 км)
    if (altitude <= 11000.0) {
        result.T = temperature_troposphere(result.H);
        result.p = pressure_troposphere(result.H, result.T);
    } else {
        // Для высот выше 11 км можно добавить расчет для стратосферы
        // В данной реализации ограничимся тропосферой
        throw std::invalid_argument("Высота превышает 11000 м. Реализация только для тропосферы");
    }
    
    // Расчет плотности
    result.ro = calculate_density(result.p, result.T);
    
    // Расчет скорости звука
    result.a = calculate_sound_speed(result.T);
    
    return result;
}