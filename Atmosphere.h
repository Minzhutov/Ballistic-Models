#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
    #ifdef ATMOSPHERE_EXPORTS
        #define ATMOSPHERE_API __declspec(dllexport)
    #else
        #define ATMOSPHERE_API __declspec(dllimport)
    #endif
#else
    #define ATMOSPHERE_API
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
ATMOSPHERE_API AtmosphereParams calculate_atmosphere(double altitude);

#ifdef __cplusplus
}
#endif

#endif // ATMOSPHERE_H