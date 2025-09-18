#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#ifdef __cplusplus
extern "C" {
#endif

// Структура для возвращаемых параметров атмосферы
typedef struct {
    double p;    // давление, Па
    double g;    // ускорение свободного падения, м/с²
    double ro;   // плотность воздуха, кг/м³
    double a;    // скорость звука, м/с
    double H;    // геопотенциальная высота, м
    double T;    // температура, К
} AtmosphereParams;

// Основная функция расчета
AtmosphereParams calculate_atmosphere(double altitude);

#ifdef __cplusplus
}
#endif

#endif // ATMOSPHERE_H