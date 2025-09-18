#include <iostream>
#include "Atmosphere.h"

int main() {
    try {
        double altitude;
        std::cout << "Введите высоту над уровнем моря (м): ";
        std::cin >> altitude;
        
        AtmosphereParams params = calculate_atmosphere(altitude);
        
        std::cout << "\nРезультаты расчета по ГОСТ 4401-81:\n";
        std::cout << "Высота: " << altitude << " м\n";
        std::cout << "Давление (p): " << params.p << " Па\n";
        std::cout << "Ускорение свободного падения (g): " << params.g << " м/с²\n";
        std::cout << "Плотность воздуха (ro): " << params.ro << " кг/м³\n";
        std::cout << "Скорость звука (a): " << params.a << " м/с\n";
        std::cout << "Геопотенциальная высота (H): " << params.H << " м\n";
        std::cout << "Температура (T): " << params.T << " К\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}