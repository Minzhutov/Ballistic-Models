#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#ifdef __cplusplus
extern "C" {
#endif

// Структура для хранения параметров атмосферы
typedef struct {
    double H_geom;  // Геометрическая высота, м
    double H_geo;   // Геопотенциальная высота, м
    double T;       // Температура, К
    double p;       // Давление, Па
    double ro;      // Плотность, кг/м³
    double a;       // Скорость звука, м/с
    double g;       // Ускорение свободного падения, м/с²
} AtmosphereParams;

/**
 * Расчет параметров стандартной атмосферы по ГОСТ 4401-81
 * @param altitude - геометрическая высота над уровнем моря, м [-2000, 94000]
 * @return Структура с параметрами атмосферы
 * @throws std::invalid_argument если высота вне допустимого диапазона
 */
AtmosphereParams calculate_atmosphere(double altitude);

#ifdef __cplusplus
}
#endif

#endif // ATMOSPHERE_H